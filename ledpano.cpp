/*
 *  ledpano.cpp
 *
 *  author:fadil
 *
 * */

#include <stdio.h>
#include <iostream>
#include <vector>
#include <map>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sstream>
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
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QXmlStreamReader>
#include <QCryptographicHash>


//local headers
#include "main.h"
//#include "ledpano.h"
//#include "utility.h"
//#include "dbabstraction.h"
#include "i2c.h"
//#include "logger.h"


#define MYPORT			"4950"    // the port users will be connecting to
#define MAXBUFLEN		128
#define MINPACKETSIZE	64
#define I2C_ADDR		0x1E

#define ERR_TIMEOUT		0x01
#define ERR_SLAVEADDR	0x02
#define ERR_BYTECOUNT	0x03
#define ERR_OUTOFMEM	0x04
#define ERR_OPENDEV		0x05
#define ERR_BUSERR		0x06
#define ERR_BUSWR		0x07
#define ERR_BUSREAD		0x08

#define CIHAZ_TIPI		91
#define CIHAZ_ALT_TIPI	1


using namespace std;


LedPano::LedPano()
{
	printf("Ledpano Constructor\n");
}

LedPano::~LedPano()
{
	printf("Distructor");
}

int LedPano::clearScreen(void)
{
	unsigned char receive_buf[1024] = {0};
	unsigned char buf1[33] = {0};
	memset(buf1, 0, sizeof(buf1));
	memset(receive_buf, 0, sizeof(receive_buf));
	buf1[0] = 6, buf1[9] = 135, buf1[10] = 255, buf1[11] = 142, buf1[12] = 88;
	int iRslt1 = i2c_command(I2C_CMD_W, I2C_ADDR, sizeof(buf1), sizeof(receive_buf), buf1, receive_buf);
	//printf("iRslt1 rslt:%d\n", iRslt1);

	unsigned char buf2[33] = {0};
	memset(buf2, 0, sizeof(buf2));
	memset(receive_buf, 0, sizeof(receive_buf));
	buf2[0] = buf2[1] = buf2[2] = buf2[4] = 1, buf2[6] = 14, buf2[9] = 69, buf2[10] = 45, buf2[11] = 75, buf2[12] = 101, buf2[13] = 110, buf2[14] = 116;
	int iRslt2 = i2c_command(I2C_CMD_W, I2C_ADDR, sizeof(buf2), sizeof(receive_buf), buf2, receive_buf);
	//printf("iRslt2 rslt:%d\n", iRslt2);

	unsigned char buf3[33] = {0};
	memset(buf3, 0, sizeof(buf3));
	memset(receive_buf, 0, sizeof(receive_buf));
	buf3[0] = buf3[1] = 2, buf3[2] = buf3[4] = 1, buf3[3] = 99, buf2[6] = 14, buf3[9] = 84, buf3[10] = 101, buf3[11] = 107, buf3[12] = 110;
	buf3[13] = 111, buf2[14] = 108, buf3[15] = 111, buf3[16] = 106, buf3[17] = 105;
	int iRslt3 = i2c_command(I2C_CMD_W, I2C_ADDR, sizeof(buf3), sizeof(receive_buf), buf3, receive_buf);
	//printf("iRslt3 rslt:%d\n", iRslt3);

	//buf4,5,6 en basta calisan programi durduruyor
	unsigned char buf4[2] = {0};
	memset(buf4, 0, sizeof(buf4));
	memset(receive_buf, 0, sizeof(receive_buf));
	buf4[0] = 3;
	int iRslt4 = i2c_command(I2C_CMD_W, I2C_ADDR, sizeof(buf4), sizeof(receive_buf), buf4, receive_buf);
	//printf("buf4 rslt:%d\n", iRslt4);

	unsigned char buf5[5] = {0};
	memset(buf5, 0, sizeof(buf5));
	memset(receive_buf, 0, sizeof(receive_buf));
	buf5[0] = 4; buf5[3] = 50;
	int iRslt5 = i2c_command(I2C_CMD_W, I2C_ADDR, sizeof(buf5), sizeof(receive_buf), buf5, receive_buf);
	//printf("buf5 rslt:%d\n", iRslt5);

	unsigned char buf6[5] = {0};
	memset(buf6, 0, sizeof(buf6));
	memset(receive_buf, 0, sizeof(receive_buf));
	buf6[0] = 5; buf6[2] = 3; buf6[3] = 10;
	int iRslt6 = i2c_command(I2C_CMD_W, I2C_ADDR, sizeof(buf6), sizeof(receive_buf), buf6, receive_buf);
	//printf("buf6 rslt:%d\n", iRslt6);

//	ledpanoObj->prepareAndPrintPackage("               ", 1);
//	ledpanoObj->prepareAndPrintPackage("               ", 2);
	prepareAndPrintPackage("E-Kent", 1);
	prepareAndPrintPackage("Teknoloji", 2);


	return iRslt1 + iRslt2 + iRslt3 + iRslt4 + iRslt5 + iRslt6;
}


int LedPano::prepareAndPrintPackage(const string strRemainingTime, uint8_t satir_no, uint8_t font, uint8_t scrool)
{

/*	struct sockaddr_in {
			short   sin_family; // should be AF_INET
			u_short sin_port;
			struct  in_addr sin_addr;
			char    sin_zero[8]; // not used, must be zero
	} adr;*/

	t_TextData textData;

//	char PacketHeader[sizeof(textData)+5] = { 0x01, 0x1E, 33, 0x00, satir_no };
//	unsigned char PacketHeader[sizeof(textData)+5] = { 0x01, 0x1E, 33, 0x00, satir_no };
	unsigned char PacketHeader[sizeof(textData)+1] = {satir_no};

	textData.Ndx = satir_no;
	textData.x = 1;			// apsis
	//textData.y = satir_no == 2 ? 8:0;			// ordinat
	if(satir_no == 1)
		textData.y = 0;
	else if(satir_no == 2)
		textData.y = 9;
	else
		textData.y = 11;

	textData.scroll = scrool == 1 ? 0x01 : 0x00;	// kaydirma enale/disable
	textData.speed = 5;	// kaydirma hizi {0-15}
	textData.font = font;		// yazi fontu {0,1,2,3}

	memset(&textData.Text, 0, sizeof(textData.Text));

	memcpy(&textData.Text, strRemainingTime.c_str(), strlen(strRemainingTime.c_str()));

	memcpy(&PacketHeader[1], (unsigned char*)&textData, sizeof(textData));

/*	struct in_addr addr;
	inet_aton("127.0.0.1", &addr);

	adr.sin_family = AF_INET;
	adr.sin_port = htons(4950);
	adr.sin_addr = addr;

	int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	utilObj->sendPackageToListener(sockfd, (struct sockaddr *)&adr, sizeof(adr), PacketHeader, sizeof(PacketHeader));*/

	//comments are for sending package to 'listener'
	// we are sending package via i2c for display to ledpanel


	unsigned char receive_buf[1024] = {0};
	int iRslt;
	iRslt = i2c_command(I2C_CMD_W, I2C_ADDR, sizeof(PacketHeader), sizeof(receive_buf), PacketHeader, receive_buf);

	if (iRslt != 0)
	{
		ERROR("Bilgiler ledpanoya aktarilamadi!\n");
		if(iRslt == ERR_SLAVEADDR)
			ERROR("Slave address out of range!\n");
		else if (iRslt == ERR_BYTECOUNT)
			ERROR("Invalid number of bytes to write!\n");
		else if (iRslt == ERR_OUTOFMEM)
			ERROR("Out of memory!\n");
		else if (iRslt == ERR_OPENDEV)
			ERROR("Failed to open control file!\n");
		else if (iRslt == ERR_BUSERR)
			ERROR("Failed to access to bus!\n");
		else if (iRslt == ERR_BUSWR)
			ERROR("Failed writing to the I2C-bus!\n");
	}

	return 0;
}


string LedPano::createHttpRequest(string hat_id, string url_path)
{
	string request = "";
	string message1 = "GET " + url_path;
	string message3 = " HTTP/1.0\r\n";

	request += message1;
	request += hat_id;
	request += message3;
	request += "\r\n\r\n";

	DEBUG("HTTP Request:%s\n", request.c_str());
	return request;
}


void LedPano::xmlReader(QString xml, vector<string>&respLst)
{
	QXmlStreamReader xmlReader(xml);
	while(!xmlReader.atEnd() && !xmlReader.hasError())
	{
		xmlReader.readNext();
		if (xmlReader.name() == "Vehicle" && xmlReader.isStartElement() && !xmlReader.isWhitespace())
		{
			respLst.push_back(xmlReader.attributes().value("TimeRemaining").toString().toStdString());
			if(respLst.size() == 2) // ledpanoya 2 satir bastirabildigimiz icin
				break;
		}
	}
}


vector<string> LedPano::getFirst2RemainingTimes(string resp)
{
	size_t indexOfXml = resp.find("<SmartStop>");
	resp.erase(0, indexOfXml);
	QString strXml = QString::fromStdString(resp);

	vector<string> respLst;
	xmlReader(strXml, respLst);
	vector<string> timeLst;
	if (respLst.size() < 2)
	{
		ERROR("Web servisteki icerik bozuk\n");
		return timeLst;
	}
	for(uint i=0 ; i<respLst.size() ; i++)
	{
		//printf("Time Remaining: %s\n", respLst[i].c_str());
//		int iRemaining = (hour*60) + minute;

		vector<string> vec = utilObj->split(respLst[i], ':'); // response has this form hh:mm:ss
		if(vec.size() != 3)
		{
			//printf("Invalid response type for remaining time! Response:%s\n", respLst[i].c_str());
			ERROR("Invalid response type for remaining time! Response:%s\n", respLst[i].c_str());
		}
		int hour = atoi(vec[0].c_str());
		int minute = atoi(vec[1].c_str());
		int iRemaining = (hour * 60) + minute;

		stringstream ss;
		ss << iRemaining;
		string strMin = ss.str();
		if (strMin.size() == 1)
			strMin = " " + strMin;

		timeLst.push_back(strMin);
	}
	return timeLst;
}

int LedPano::getVersionFromHost(map<string, string> &versions, string current_version)
{
	//printf("get version from host!\n");
	string oldVersion;
	oldVersion = current_version;

	char respbuf[8192] = {0};
	char data[8192] = {0};
	int iRslt = 0;


	//"http://10.110.6.32/erms.aspx?mac=3ABE33A5CE77&Islem=9&KoseNo=0&TurnikeNo=0&SehirID=6&CihazTipiID=91&CihazAltTipiID=1";

	sprintf(data, "GET /eRMS.aspx?"
				  "Islem=2"
				  "&CihazTipiID=%u"
				  "&CihazAltTipiID=%u"
				  "&SehirID=6"
				  "&MAC=%s "
				  " HTTP/1.0\r\nHost: %s\r\nConnection: close\r\n\r\n",
			CIHAZ_TIPI,
			CIHAZ_ALT_TIPI,
			utilObj->getMacAddr().c_str(),
			databaseObj->server_ip.c_str());

	iRslt = utilObj->httpPost(databaseObj->server_ip.c_str(), 80, data, strlen(data), respbuf, sizeof(respbuf), 5000);

	if(iRslt < 0)
		return iRslt;


	string versionResp = string(respbuf);
	if (versionResp.size() < 10)
	{
		//printf("Invalid response for new version query! Response:%s", versionResp.c_str());
		ERROR("Invalid response for new version query! Response:%s", versionResp.c_str());
		return -6;
	}
	//printf("\nVERSION RESPONSE:\n%s", versionResp.c_str());

	size_t httpPos = versionResp.find("\n");
	string httpResp = versionResp.substr(9, httpPos-10);
	//printf("\nfirst line:%s\n", httpResp.c_str());
	if(httpResp != "200 OK")
	{
		ERROR("HTTP Response for version:%s\n", versionResp.substr(0,httpPos).c_str());
		ERROR("Version sorgusu icin e-RMS ile baglanti kurulamadi!\n");
		return -1;
	}

	size_t textPos = versionResp.find("\r\n\r\n");
	string text = versionResp.substr(textPos+4);

	vector<string> textLst = utilObj->split(text, '\n');
	for(uint i=0 ; i<textLst.size() ; i++)
	{
		//printf("\ntext[%d]:%s\n", i, textLst[i].c_str());
		vector<string> textLine = utilObj->split(textLst[i], ' ');
		nameLst.push_back(textLine[0]);
		versionLst.push_back(textLine[1]);
		dosyaNameLst.push_back(textLine[2]);
		md5Lst.push_back(textLine[3]);

		vector<string> parseDosyaNameLst = utilObj->split(dosyaNameLst[i], '_');
		dosyaNoLst.push_back(parseDosyaNameLst[1]);

		/*printf("\nname         :%s \nversion      :%s \ndosya name   :%s \nmd5          :%s \nprogram no   :%s \n\n",
			   nameLst[i].c_str(),
			   versionLst[i].c_str(),
			   dosyaNameLst[i].c_str(),
			   md5Lst[i].c_str(),
			   dosyaNoLst[i].c_str());*/

		versions[nameLst[i]] = versionLst[i];
	}

	return iRslt;
}


int LedPano::yeniVersiyonIndir(string name)
{
	uint i = 0;
	while(i<nameLst.size())
	{
		if(name == nameLst[i])
			break;

		i++;
	}

	string hostPrgname = dosyaNameLst[i];
	string newVersionPath = "/Versiyonlar/";
	string cmd = "wget -O /tmp/" + hostPrgname + " --timeout=300 -q -c http://" + databaseObj->server_ip + newVersionPath + hostPrgname;
	/*wget -O /tmp/guncelle_0038_160603.zip --timeout=300 -c http://10.110.5.101/Versiyonlar/guncelle_0038_160603.zip*/

	if (system(cmd.c_str()) < 0)
	{
		ERROR("Yeni versiyon indirilemedi!\n");
		return -1;
	}

	string md5Hash = md5HashOfFile(dosyaNameLst[i]);
	if(md5Hash != md5Lst[i]) // md5 control
	{
		ERROR("md5 hash'ler uyusmuyor. Dosya tamamen indirilemedi!\n");
		DEBUG("md5 from host:%s, md5 hash size:%d\n", md5Lst[i].c_str(), md5Lst[i].size());
		DEBUG("md5 from downloaded file:%s, md5 hash size:%d\n", md5Hash.c_str(), md5Hash.size());
		QString path = QString::fromStdString("/tmp/" + dosyaNameLst[i]);
		if(QFile::remove(path))
		{
			//printf("md5 uyusmayan dosya silindi\n");
			DEBUG("md5 uyusmayan dosya silindi, periyot sonunda tekrar indirilecek\n");
		}
		else
		{
			//printf("md5 uyusmayan dosya silinemedi\n");
			ERROR("md5 uyusmayan dosya %s silinemedi, /tmp/ altinda dosya duruyor\n", dosyaNameLst[i].c_str());
		}
		return -2;
	}

	/*printf("Yeni versiyon indirildi, name:%s, version:%s, dosya name:%s, md5:%s, dosya no:%s\n\n", name.c_str(),
																									versionLst[i].c_str(),
																									dosyaNameLst[i].c_str(),
																									md5Lst[i].c_str(),
																									dosyaNoLst[i].c_str());*/
	DEBUG("Yeni versiyon indirildi, name:%s\n", name.c_str());

	if(moveFileToInbox(dosyaNameLst[i]) < 0)
		return -1;

	versiyonIndirildiBilgisiGonder(dosyaNoLst[i]); // send new version is downloaded data

	return 0;
}


string LedPano::md5HashOfFile(string filename)
{
	QString path = QString::fromStdString("/tmp/" + filename);
	QFile file(path);

	QByteArray hashData;
	if (file.open(QIODevice::ReadOnly))
	{
		QByteArray fileData = file.readAll();
		hashData = QCryptographicHash::hash(fileData, QCryptographicHash::Md5);
	}
	return string(hashData.toHex());
}


int LedPano::moveFileToInbox(string filename)
{
	QString oldpath = QString::fromStdString("/tmp/" + filename);
	QString newpath = QString::fromStdString("/ekent/inbox/" + filename);
	if(QFile::rename(oldpath, newpath))
	{
		DEBUG("%s, %s'e tasindi\n", oldpath.toStdString().c_str(), newpath.toStdString().c_str());
		return 0;
	}
	else
	{
		//printf("%s, %s'e tasinamadi\n", oldpath.toStdString().c_str(), newpath.toStdString().c_str());
		ERROR("%s, %s'e tasinamadi\n", oldpath.toStdString().c_str(), newpath.toStdString().c_str());
		return -1;
	}
}


int LedPano::versiyonIndirildiBilgisiGonder(string dosya_no)
{
	char respbuf[8192] = {0};
	char data[8192] = {0};
	int iRslt = 0;

	sprintf(data, "GET /eRMS.aspx?"
				  "Islem=3" // operation type  for send new version is downloaded info
				  "&MAC=%s"
				  "&IslemSiraNo=%s"
				  "&SehirID=%u"
				  "&CihazTipiID=%u"
				  "&CihazAltTipiID=%u"
				  " HTTP/1.0\r\nHost: %s\r\nConnection: close\r\n\r\n",
			utilObj->getMacAddr().c_str(),
			dosya_no.c_str(),
			databaseObj->city_id,
			CIHAZ_TIPI,
			CIHAZ_ALT_TIPI,
			databaseObj->server_ip.c_str());// essential parameters to send new version is downloaded data

	iRslt = utilObj->httpPost(databaseObj->server_ip.c_str(), 80, data, strlen(data), respbuf, sizeof(respbuf), 5000);
	string resp = string(respbuf);
	if (resp.size() < 10)
	{
		//printf("Invalid response for sending 'new version is downloaded data! Response:%s\n", resp.c_str());
		ERROR("Invalid response for sending 'new version is downloaded data! Response:%s\n", resp.c_str());
		return -6;
	}

	size_t httpPos = resp.find("\n");
	string httpResp = resp.substr(9, httpPos-10);
	//printf("\nversion indirildi first line:%s\n", httpResp.c_str());


	if (iRslt != 0)
	{
		//printf("Versiyon indirildi bilgisi gonderilemedi!\n");
		ERROR("Versiyon indirildi bilgisi gonderilemedi!\n"); // couldn't send data for new version is downloaded
	}
	else
	{
		if(httpResp != "200 OK")
		{
			//printf("Versiyon indirildi bilgisi icin e-RMS ile baglanti kurulamadi!\n");
			ERROR("Versiyon indirildi bilgisi icin e-RMS ile baglanti kurulamadi!\n"); //couldn't connect to e-RMS server to send data
			DEBUG("HTTP Response for 'version indirildi bilgisi gonder':%s\n", resp.substr(0, httpPos).c_str());
			return -1;
		}
		//printf("Versiyon indirildi bilgisi gonderildi\n\n");
		DEBUG("Versiyon indirildi bilgisi e-RMS sunucusuna gonderildi\n"); // new version is downloaded data is sended to e-RMS server
	}

	return iRslt;
}


int LedPano::genelBilgileriGonder(string current_version)
{
	char respbuf[8192] = {0};
	char data[8192] = {0};
	int iRslt = 0;

	sprintf(data, "GET /eRMS.aspx?"
				  "Islem=1" // operation type for send general info
				  "&BiletVrs=%s"
				  "&DBVrs=%s"
				  "&CihazTarihi=%s%%20%s"
				  "&SehirID=%u"
				  "&CihazTipiID=%u"
				  "&CihazAltTipiID=%u"
				  "&MAC=%s"
				  " HTTP/1.0\r\nHost: %s\r\nConnection: close\r\n\r\n",
			current_version.c_str(),
			databaseObj->db_version.c_str(),
			utilObj->getCurrentTimeStr("%d.%m.%y").c_str(),
			utilObj->getCurrentTimeStr("%H:%M:%S").c_str(),
			databaseObj->city_id,
			CIHAZ_TIPI,
			CIHAZ_ALT_TIPI,
			utilObj->getMacAddr().c_str(),
			databaseObj->server_ip.c_str());

	iRslt = utilObj->httpPost(databaseObj->server_ip.c_str(), 80, data, strlen(data), respbuf, sizeof(respbuf), 5000);
	string infoResp = string(respbuf);
	if (infoResp.size() < 10)
	{
		//printf("Invalid response for sending general data! Response:%s\n", infoResp.c_str());
		ERROR("Invalid response for sending general data! Response:%s\n", infoResp.c_str());
		return -6;
	}
//	printf("\nGENEL BILGI RESPONSE:\n%s", infoResp.c_str());

	size_t httpPos = infoResp.find("\n");
	string httpResp = infoResp.substr(9, httpPos-10);
//	printf("\ninfo first line:%s\n", httpResp.c_str());


	if (iRslt != 0)
	{
		//printf("Genel bilgiler gonderilemedi!\n");
		ERROR("Genel bilgiler gonderilemedi! HTTP Get return value:%d\n", iRslt);
		return iRslt;
	}

	if(httpResp != "200 OK")
	{
		//printf("Genel bilgi gonderimi icin e-RMS ile baglanti kurulamadi!\n");
		ERROR("HTTP Response for 'genel bilgi gonder':%s", infoResp.substr(0, httpPos).c_str());
		ERROR("Genel bilgi gonderimi icin e-RMS ile baglanti kurulamadi!\n");
		return -10;
	}

	//printf("Genel bilgiler gonderildi\n\n");
	DEBUG("Genel bilgiler e-RMS sunucusuna gonderildi\n");

	return 0;
}


