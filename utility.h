#ifndef UTILITY_H
#define UTILITY_H

#include <iostream>
#include <vector>
#include <sys/time.h>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlRecord>
#include <QtCore/QVariant>
#include <QtCore/QTextCodec>
#include <QtCore/QFile>
#include <QtCore/QString>


// Values for the second argument to `fcntl'.
#define F_DUPFD		0	// Duplicate file descriptor.
#define F_GETFD		1	// Get file descriptor flags.
#define F_SETFD		2	// Set file descriptor flags.
#define F_GETFL		3	// Get file status flags.
#define F_SETFL		4	// Set file status flags.


// open/fcntl - O_SYNC is only implemented on blocks devices and on files located on an ext2 file system
#define O_ACCMODE	   0003
#define O_RDONLY	     00
#define O_WRONLY	     01
#define O_RDWR		     02
#define O_CREAT		   0100	// not fcntl
#define O_EXCL		   0200	// not fcntl
#define O_NOCTTY	   0400	// not fcntl
#define O_TRUNC		  01000	// not fcntl
#define O_APPEND	  02000
#define O_NONBLOCK	  04000
#define O_NDELAY	O_NONBLOCK
#define O_SYNC		 010000
#define O_FSYNC		 O_SYNC
#define O_ASYNC		 020000

#ifdef __USE_GNU
# define O_DIRECTORY	 040000	// Must be a directory.
# define O_NOFOLLOW		0100000	// Do not follow links.
# define O_DIRECT		0200000	// Direct disk access.
# define O_NOATIME     01000000 // Do not set atime.
# define O_CLOEXEC     02000000 // Set close_on_exec.
#endif

using namespace std;

class Utility
{
	string getIfaceMacStr(const char *ifname);

public:
	Utility();
	~Utility();

	string getMacAddr();
	vector<string> readFile(string filepath); //line by line
	bool tcpConnectSocket(int *sockHandle, const char *address, int port);
	int httpPost(const char *address, int port, const char *data, int len, char *respBuf, int respBufLen, int timeout);
	string getCurrentTimeStr(const char *format);
	int timeDiffSecs(vector<string> vDate, vector<string> vTime);
	vector<string> split(string str, char delimiter);

};

#endif // UTILITY_H
