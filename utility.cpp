/*
 *  utility.cpp
 *
 *  author:fadil
 *
 * */

#include <stdio.h>
#include <iostream>
#include <vector>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <algorithm>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <netinet/in.h>
#include <netinet/if_ether.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <linux/kernel.h>
//#include <QtSql/QSqlQuery>
//#include <QtSql/QSqlRecord>
//#include <QtSql/QSqlDatabase>
#include <QtCore/QVariant>
#include <QtCore/QTextCodec>
#include <QtCore/QFile>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <sstream>
#include <fstream>

//#include "main.h"
#include "utility.h"
#include "logger.h"

#define MYPORT "4950"    // the port users will be connecting to
#define MAXBUFLEN 128
#define MINPACKETSIZE	64

#define ID_TABLE			"DEVICE_HAT_ID"
#define PARAMETERS_TABLE	"DEVICE_PARAMETERS"

#define LOGFILE				"/tmp/logfile.txt"
#define MAC_INTERFACE		"usb0"

using namespace std;

Utility::Utility()
{
	printf("Utility Constructor\n");
}

Utility::~Utility()
{
	printf("Distructor");
}


/* //Listener'ı kapattık, i2c üzerinden panoya yazdırıyoruz
 * //we use i2c, so listener is out of order for us
void Utility::sendPackageToListener(int sockfd, struct sockaddr *their_addr, int addr_len, char* buf, int buflen)
{
	if (buflen>0)
	{
		try
		{
			if (buflen<MINPACKETSIZE)
			{
				//__u8 *send_buf = (__u8*) malloc(64 * sizeof(__u8));
				unsigned char *send_buf = (unsigned char*) malloc(64 * sizeof(unsigned char));
				if (send_buf!=NULL)
				{
					bzero(send_buf,MINPACKETSIZE);
					memcpy(send_buf,buf,buflen);
					sendto(sockfd,send_buf,MINPACKETSIZE,0,their_addr,addr_len);
				}
			}
			else
				sendto(sockfd,buf,buflen,0,their_addr,addr_len);

			//cout << "Package has been sent to listener" << endl;
		}
		catch (...)
		{
			cout << "Can not send package to listener!" << endl;
		}
	}
}*/

vector<string> Utility::readFile(string filepath) // line by line
{
	ifstream file(filepath.c_str());
	string str;
	vector<string> strVec;
	while (std::getline(file, str))
	{
		strVec.push_back(str);
	}
	return strVec;
}


string Utility::getIfaceMacStr(const char *ifname)
{
	int err;
	struct ifreq ifr;
	int skfd = -1;
	char buf[18] = {0};

	skfd = socket(AF_INET, SOCK_DGRAM, 0);

	if (skfd < 0) {
		FATAL("Soket acilamadi, MAC adresi alinamadi!\n");
		return "000000000000";
	}

	memset(&ifr, 0, sizeof(ifr));
	sprintf(ifr.ifr_name, "%s", ifname);

	err = ioctl(skfd, SIOCGIFHWADDR, &ifr);
	if (err) {
		FATAL("MAC adresi alinamadi!\n");
		return "000000000000";
	}

	close(skfd);

	sprintf(buf, "%02X%02X%02X%02X%02X%02X",
		(unsigned char)ifr.ifr_hwaddr.sa_data[0],
		(unsigned char)ifr.ifr_hwaddr.sa_data[1],
		(unsigned char)ifr.ifr_hwaddr.sa_data[2],
		(unsigned char)ifr.ifr_hwaddr.sa_data[3],
		(unsigned char)ifr.ifr_hwaddr.sa_data[4],
		(unsigned char)ifr.ifr_hwaddr.sa_data[5]);

	return string(buf);
}

string Utility::getMacAddr(void)
{
	string macAddr = getIfaceMacStr(MAC_INTERFACE);

	transform(macAddr.begin(), macAddr.end(), macAddr.begin(), ::toupper);
	//printf("\nMac Address: %s\n\n", macAddr.c_str());
	return macAddr;
}

bool Utility::tcpConnectSocket(int *sockHandle, const char *address, int port)
{
#define TCP_RETRY	3
#define TCP_TIMEOUT	2
	struct hostent *he;
	int flags, res, error;
	socklen_t len;
	fd_set rset, wset;
	struct timeval tv;
	int i = 0;
	sockaddr_in addr;
	int sock;

	if ((he = gethostbyname(address)) == 0) {
		FATAL("gethostbyname error! address=%s\n", address);
		return FALSE;
	}

	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr = *((in_addr *) he->h_addr);
	memset(&(addr.sin_zero), 0, 8);

	for (i = 0; i < TCP_RETRY; i++) {
		error = 0;
		if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
			ERROR("cannot open stream socket!\n");
			return FALSE;
		}
		flags = fcntl(sock, F_GETFL, 0);
		fcntl(sock, F_SETFL, flags | O_NONBLOCK | O_CLOEXEC);
		if ((res = connect(sock, (struct sockaddr *) &addr, sizeof (addr))) < 0) {
			if (errno != EINPROGRESS && errno != EALREADY) {
				close(sock);
				usleep(50000);
				continue;
			}
		}

		// Bağlantı çok hızlı gerçekleşmiş, nadir olabilecek bir durum
		// Bu durumda selecte gelmeden buradan çıkabiliriz
		if (res == 0) {
			fcntl(sock, F_SETFL, flags);
			*sockHandle = sock;
			DEBUG("Connected!\n");
			//TRACE("Connected!\n");
			return TRUE;
		}

		FD_ZERO(&rset);
		FD_SET(sock, &rset);
		wset = rset;
		tv.tv_sec = TCP_TIMEOUT;
		tv.tv_usec = 0;

		if ((res = select(sock + 1, &rset, &wset, NULL, &tv)) == 0) {
			// Select'ten 0 dönmüş == timeout
			// tekrar deneyelim, onceki baglantiyi kapatalim
			close(sock);
			usleep(50000);
			continue;
		}

		if (FD_ISSET(sock, &rset) || FD_ISSET(sock, &wset)) {
			// Bağlantı tamam, porta bilgi geldi yada connect hatalı bir duruma düştü
			// O yüzden socket readable ve writeable olarak set edildi
			// sockopt ile error varsa alalım
			len = sizeof (error);
			if (getsockopt(sock, SOL_SOCKET, SO_ERROR, &error, &len) < 0) {
				close(sock);
				usleep(50000);
				continue;
			}

			// Connect() hataya mı düşmüş
			if (error) {
				close(sock);
				usleep(50000);
				continue;
			}

			// Buraya geldiysek her şey ok demektir.
			fcntl(sock, F_SETFL, flags);
			*sockHandle = sock;
//			TRACE("Connected!\n");
			return TRUE;
		}
	}

	ERROR("cannot connect to tcp socket!\n");

	return FALSE;
}


int Utility::httpPost(const char *address, int port, const char *data, int len, char *respBuf, int respBufLen, int timeout)
{

	int result = -1;
	int sock;
	if (!tcpConnectSocket(&sock, address, port)) {
		ERROR("Cannot open socket. address=%s port=%d\n\n", address, port);
		return -2;
	}

	//printf("sock %d\n", sock);

	memset(respBuf, 0, respBufLen);

	//TRACE("Sending %d bytes to %s\n", len, address);

	if (send(sock, data, len, 0) != len) {
		FATAL("send returned %d\n", len);
		close(sock);
		return -3;
	}

	int recvd, total = 0;
	struct timeval begin, now;
	int timediff;

	// make socket non blocking
	fcntl(sock, F_SETFL, O_NONBLOCK);

	//beginning time
	gettimeofday(&begin, NULL);

	while (respBufLen > total) {
		gettimeofday(&now, NULL);

		// time elapsed in msecs
		timediff = (now.tv_sec - begin.tv_sec) * 1000 + ((now.tv_usec - begin.tv_usec) / 1000);

		// break after timeout
		if (timeout && (timediff > timeout)) {
			ERROR("Timeout occured! total=%d\n", total);
			result = -4;
			break;
		}

		if ((recvd =  recv(sock, &respBuf[total], respBufLen - total, 0)) < 0) {
			if (errno == EAGAIN) {
				// if nothing was received then we want to wait a little before trying again, 10 ms
				usleep(10000);
				continue;
			}
			break;
		} else {
			if (recvd == 0) {
				result = -5;
				break;
			}

			//TRACE("recvd=%d total=%d\n", recvd, total);
			total += recvd;
		}

		usleep(5000);
	}

	if (total > 0) {
		result = 0;
	}

	close(sock);

	//TRACE("Received %d bytes response\n", total);

	return result;
}

string Utility::getCurrentTimeStr(const char *format)
{
	time_t curtime;
	struct tm curtm;
	char buf[256] = {0};

	/* Get the current time in seconds */
	time(&curtime);
	/* Break it down & store it in the structure tm */
	localtime_r(&curtime, &curtm);

	strftime(buf, sizeof(buf)-1, format, &curtm);

	return string(buf);
}

int Utility::timeDiffSecs(vector<string> vDate, vector<string> vTime)
{
	time_t curtime, time1;
	struct tm timeinfo;

	/* Get the current time in seconds */
	time(&curtime);

	time(&time1);
	localtime_r(&time1, &timeinfo);

	timeinfo.tm_year = atoi(vDate[2].c_str())-1900;
	timeinfo.tm_mon	 = atoi(vDate[1].c_str())-1;
	timeinfo.tm_mday = atoi(vDate[0].c_str());
	timeinfo.tm_hour = atoi(vTime[0].c_str());
	timeinfo.tm_min	 = atoi(vTime[1].c_str());
	timeinfo.tm_sec  = atoi(vTime[2].c_str());

	time1 = mktime(&timeinfo);

	// 2038 yilindan sonraki tarihler time_t structure'ina sigmadigi icin sorun cikiyor
	if (time1 == -1) {
		return -1;
	}

	double diff = difftime(curtime, time1);

	return (int)diff;
}

vector<string> Utility::split(string str, char delimiter)
{
	vector<string> internal;
	stringstream ss(str); // Turn the string into a stream.
	string tok;

	while(getline(ss, tok, delimiter))
		internal.push_back(tok);

	return internal;
}


