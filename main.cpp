/*
 *  main.cpp
 *
 *  author:fadil
 *
 * */

#include <QCoreApplication>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
//#include <sys/types.h>
//#include <sys/socket.h>
//#include <netinet/in.h>
//#include <arpa/inet.h>
//#include <netdb.h>
#include <linux/kernel.h>

//#include <QtSql/QSqlQuery>
//#include <QtSql/QSqlRecord>
//#include <QtSql/QSqlDatabase>
#include <QtCore/QVariant>
#include <QtCore/QTextCodec>
#include <QtCore/QFile>
//#include <QtCore/QString>
//#include <QtCore/QStringList>

#include <pthread.h>
//#include <sstream>
#include <fstream>
#include <sys/resource.h>
#include <map>
#include <cmath>

#include "main.h"

using namespace std;


LedPano *ledpanoObj;
Utility *utilObj;
DBAbstraction *databaseObj;
bool downloadAndReboot = false;
bool clearScreen = false;


int versionKontrol(void) // get version from host and control new one is exist
{
	map<string, string> versions;
	int iRsltVersion;
	iRsltVersion = ledpanoObj->getVersionFromHost(versions, PROGRAM_VERSIYON_FULL);
	if(iRsltVersion != 0)
	{
		ERROR("Sunucudan versiyon bilgisi alinamadi!\n");
		return -1;
	}

	map<string, string>::iterator it;
	for(it=versions.begin(); it!=versions.end() ; it++)
	{
		if(it->first == "LED_PANO_BILET")
		{
			if(it->second != PROGRAM_VERSIYON_FULL)
			{
				//printf("New version:%s for %s\nVersion %s indiriliyor...\n", (it->second).c_str(), (it->first).c_str(), (it->second).c_str());
				DEBUG("New version:%s for %s\nVersion %s indiriliyor...\n", (it->second).c_str(), (it->first).c_str(), (it->second).c_str());
				if(ledpanoObj->yeniVersiyonIndir(it->first) < 0)
					return -2;
			}
			else
				DEBUG("Bilet versiyon guncel!\n");
		}
		else if(it->first == "LED_PANO_DB")
		{
			if(it->second != databaseObj->db_version)
			{
				//printf("New version:%s for %s\nVersion %s indiriliyor...\n", (it->second).c_str(), (it->first).c_str(), (it->second).c_str());
				DEBUG("New version:%s for %s\nVersion %s indiriliyor...\n", (it->second).c_str(), (it->first).c_str(), (it->second).c_str());
				if(ledpanoObj->yeniVersiyonIndir(it->first) < 0)
					return -3;
			}
			else
				DEBUG("DB versiyon guncel!\n");
		}
	}
	return 0;
}

void genelBilgiGonder(void)
{
	int iRsltPing;
	iRsltPing = ledpanoObj->genelBilgileriGonder(PROGRAM_VERSIYON_FULL);
	if(iRsltPing < 0)
		ERROR("Genel bilgiler e-RMS sunucusuna gönderilemedi!\n");
}

void saatKontrol(void)
{
	char respbuf[8192] = {0};
	char data[8192] = {0};
	int iRslt = 0;

	sprintf(data, "GET /getdatetime.aspx HTTP/1.0\r\nHost: %s\r\nConnection: close\r\n\r\n",
			databaseObj->server_ip.c_str());

	iRslt = utilObj->httpPost(databaseObj->server_ip.c_str(), 80, data, strlen(data), respbuf, sizeof(respbuf), 5000); // get real time from host
	if(iRslt < 0)
	{
		ERROR("Saat kontrolu icin sunucu ile baglanti kurulamadi!\n"); //couldn't connect to host
		return;
	}

	string response = string(respbuf);
	if (response.size() < 10)
	{
		//printf("Invalid response for time control! Response:%s\n", response.c_str());
		DEBUG("Invalid response for time control! Response:%s\n", response.c_str());
		return;
	}
	size_t httpPos = response.find("\n");
	string httpResp = response.substr(9, httpPos-10);
	if(httpResp != "200 OK")
	{
		ERROR("Sunucudan tarih ve saat bilgisi alinamadi!\n"); // couldn't get time info from host
		ERROR("HTTP Response for saat kontrol:%s\n", response.substr(0, httpPos).c_str());
		return;
	}

	size_t textPos = response.find("\r\n\r\n");
	string text = response.substr(textPos+4);
	size_t respPos = text.find("\r\n");
	string dateTime = text.substr(0, respPos);
	vector<string> vDateTime = utilObj->split(dateTime, ' '); // vDateTime[0]=12.01.2017 , vDateTime[1]16:09:51

	vector<string> vDate = utilObj->split(vDateTime[0], '.'); // vDate[2]:yil , vDate[1]:ay , vDate[0]:gun
	vector<string> vTime = utilObj->split(vDateTime[1], ':'); // vTime[0]:saat , vTime[1]:dakika , vTime[2]:saniye

	int diff = utilObj->timeDiffSecs(vDate, vTime);
	if((int)fabs(diff) > TIMEDIFF_SECS)
	{
		TRACE("Sistem saati ile global saat arasindaki fark 30 sn'den buyuk:%d sn\n", (int)fabs(diff)); //diffrence is more than 30 secs.
		string setTimeCmd = "date --set=\"" + vDate[2] + vDate[1] + vDate[0] + " "  + vDateTime[1] + "\"";
		//printf("\nset time cmd: %s\n\n", setTimeCmd.c_str());
		if(::system(setTimeCmd.c_str()) != 0) //run set time command
		{
			printf("Saat set edilemedi!\n");
			ERROR("Saat set edilemedi!\n"); // couldn't set time
		}
		else
			DEBUG("Saat basariyla set edildi!\n"); //time is set succesfully
	}
}

bool isRebootTime(void)
{
	bool dateCtrl = false;
	bool timeCtrl = false;

	string currentDate = utilObj->getCurrentTimeStr("%d.%m.%y"); //format: dd.mm.yy
	string currentTime = utilObj->getCurrentTimeStr("%H:%M:%S"); //format: hh:mm:ss

//	vector<string> dateVec = utilObj->split(currentDate, '.');
	vector<string> timeVec = utilObj->split(currentTime, ':');

	vector<string> rebootDateVec = utilObj->readFile(LAST_REBOOTDATE_FILE);
	string lastRebootDate = rebootDateVec[rebootDateVec.size()-1];
	//printf("\nlast reboot date:%s\ncurrent date:%s\n\n", lastRebootDate.c_str(), currentDate.c_str());
	if(lastRebootDate != currentDate)
		dateCtrl = true;

	int hour = atoi(timeVec[0].c_str());
	int min  = atoi(timeVec[1].c_str());

	if(hour == REBOOT_HOUR && min >= REBOOT_MIN)
		timeCtrl = true;


	return (dateCtrl & timeCtrl);
}


void *timePingAndVersionThread(void *)
{
	int counter = 0;
	while (true)
	{
		if(counter%CNTR_VERSION == 0)
		{
			if(versionKontrol() == 0 && downloadAndReboot)
			{
				printf("REBOOTING...");
				int iRslt = -1;
				while(iRslt != 0)
				{
					iRslt = system("reboot");
					sleep(2);
				}
			}
		}

		if(counter%CNTR_GENELBILGI == 0)
			genelBilgiGonder(); // send general data(like heartbeat/ping)

		if(counter%CNTR_SAAT == 0)
			saatKontrol(); // time control

		if(isRebootTime())
		{
			printf("REBOOT TIME!\n");
			ofstream rebootDateFile;
			rebootDateFile.open(LAST_REBOOTDATE_FILE);
			rebootDateFile << utilObj->getCurrentTimeStr("%d.%m.%y");
			rebootDateFile.close();

			int iRslt = -1;
			while(iRslt != 0)
			{
				iRslt = system("reboot");
				sleep(2);
			}
		}

		sleep(60);
		counter++;
	}

	return NULL;
}

void *runLedPano(void *)
{
	sleep(2);
	int retryCounter = 0;
	string host = databaseObj->url_host;
	string macAddr = utilObj->getMacAddr();
	DEBUG("Host: %s\nMac Adresi: %s\n", host.c_str(), macAddr.c_str());
	char data[8192] = {0};
	string hatId;
	if(databaseObj->getHatId(macAddr, hatId))
	{
		if(hatId == "")
		{
			string errorCode;
			map<string, string> versions;
			ledpanoObj->getVersionFromHost(versions, PROGRAM_VERSIYON_FULL);
			map<string, string>::iterator it;
			for(it=versions.begin(); it!=versions.end() ; it++)
			{
				if(it->first == "LED_PANO_DB" && it->second == databaseObj->db_version)
					errorCode = "E4";
				else
					errorCode = "E2";
			}

			FATAL("MAC adrese(%s) hat Id atanmamis!\n", macAddr.c_str());
			ledpanoObj->prepareAndPrintPackage("iyi yolculuklar ", 1, 1, 1); // 'have a nice trip'
			ledpanoObj->prepareAndPrintPackage("                    ", 2);
			ledpanoObj->prepareAndPrintPackage(errorCode, 2);

			sleep(10);

			downloadAndReboot = true;

			return NULL;
		}
	}
	else
	{
		string errorCode;
		map<string, string> versions;
		ledpanoObj->getVersionFromHost(versions, PROGRAM_VERSIYON_FULL);
		map<string, string>::iterator it;
		for(it=versions.begin(); it!=versions.end() ; it++)
		{
			if(it->first == "LED_PANO_DB" && it->second == databaseObj->db_version)
				errorCode = "E3";
			else
				errorCode = "E1";
		}

		FATAL("MAC address '%s' tanimli degil!\n", macAddr.c_str());
		ledpanoObj->prepareAndPrintPackage("iyi yolculuklar ", 1, 1, 1); // 'have a nice trip'
		ledpanoObj->prepareAndPrintPackage("                    ", 2);
		ledpanoObj->prepareAndPrintPackage(errorCode, 2);

		sleep(10);

		downloadAndReboot = true;

		return NULL;
	}

	string httpRequest = ledpanoObj->createHttpRequest(hatId, databaseObj->url_path);
	strcpy(data, httpRequest.c_str());
	DEBUG("Request:%s\n", httpRequest.c_str());
	char respbuf[8192] = {0};
	while (true)
	{
		if(clearScreen)
		{
			ledpanoObj->clearScreen();
			clearScreen = false;
			sleep(1);
		}

		try
		{
			memset(respbuf, 0, sizeof(respbuf)); //buffer'i sifirla
			int iRslt = utilObj->httpPost(host.c_str(), WEBSERVICE_PORT, data, strlen(data), respbuf, sizeof(respbuf), 5000);
			if(iRslt < 0)
			{
				ERROR("Couldn't connect to host for remaining time!\n");
				retryCounter++;
				if (retryCounter == RETRY_COUNT)
				{
					ledpanoObj->prepareAndPrintPackage("İyi yolculuklar...  ", 1, 1, 1);
					ledpanoObj->prepareAndPrintPackage("                    ", 2);
					ledpanoObj->prepareAndPrintPackage("E5", 2);
					ERROR("%d kez ustuste host'a baglanilamadi.Panoya 'iyi yolculuklar' yazdirilacak\n", RETRY_COUNT);
					clearScreen = true;
					//reached to retry count. 'have a nice trip' will be displayed to panel
				}
				sleep(5);
				continue;
			}
			retryCounter = 0;
			string resp = string(respbuf);
			vector<string> minuteLst = ledpanoObj->getFirst2RemainingTimes(resp);
			if(minuteLst.size() < 2)
			{
				ledpanoObj->prepareAndPrintPackage("İyi yolculuklar...  ", 1, 1, 1);
				ledpanoObj->prepareAndPrintPackage("                    ", 2);
				ledpanoObj->prepareAndPrintPackage("E6", 2);
				ERROR("web servisten alinan icerik bozuk!\n"); // the package that gotten from web service is broken
				sleep(5);
				clearScreen = true;
				continue;
			}
			for(uint i=0 ; i<minuteLst.size() ; i++)
			{
				if(minuteLst[i] == " 0")
				{
					cout << "Geliyor...  " << endl;
					ledpanoObj->prepareAndPrintPackage("Geliyor...  ", i+1, 1, 1);
				}
				else
				{
					string strRemainingTime = "Kalan sure    " + minuteLst[i] + " dk";
					cout << strRemainingTime << endl;
					ledpanoObj->prepareAndPrintPackage(strRemainingTime, i+1);
				}
			}
		}
		catch (...)
		{
			ERROR("An error has occured!");
		}
		cout << endl;
		sleep(databaseObj->update_period);
	}
	return NULL;
}


int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);

	ledpanoObj	= new LedPano();
	utilObj		= new Utility();
	databaseObj = new DBAbstraction();

	LogLevel loglevel = DEFAULT_LOG_LEVEL;
	const char *logfile = DEFAULT_LOG_FILE;
	LoggerInit(loglevel, logfile, 0);

	DEBUG("Application started. Loglevel=%d LogFile=%s\n", loglevel, logfile);
	DEBUG("%s version %s\n", APP_NAME, PROGRAM_VERSIYON_FULL);

	int iRslt = ledpanoObj->clearScreen();
	if(iRslt < 0)
		ERROR("Baslangic icin ekran temizlenemedi!\n"); // couldn't make clean

	sleep(1);

	pthread_t thread1;
	int i1;
	i1 = pthread_create( &thread1, NULL, timePingAndVersionThread, (void*) ""/*, (void*) "thread 1"*/);
	//pthread_join(thread1,NULL);

	pthread_t thread2;
	int i2;
	i2 = pthread_create(&thread2, NULL, runLedPano, (void*) "");

	while (true)
	{
		sleep(1);
	}

	//return a.exec();

	return 0;
}
