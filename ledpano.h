#ifndef LEDPANO_H
#define LEDPANO_H

#include <stdio.h>
#include <iostream>
#include <vector>
#include <map>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
//#include <QtSql/QSqlQuery>
//#include <QtSql/QSqlRecord>
#include <QtCore/QVariant>
#include <QtCore/QTextCodec>
#include <QtCore/QFile>
#include <QtCore/QString>
#include "stdint.h"
#include "stddef.h"
#include "utility.h"
#include "dbabstraction.h"
#include "device.h"
#include "i2c.h"

using namespace std;

//enum islemType
//{
//	GenelBilgiGonder = 1,
//	VersiyonKontrolu,
//	VersiyonIndirildiBilgisiGonder
//};

class LedPano
{
	string md5HashOfFile(string filename);
	int moveFileToInbox(string filename);
	void xmlReader(QString xml, vector<string>&respLst);

public:
	LedPano();
	~LedPano();

	int clearScreen();
	int prepareAndPrintPackage(const string strRemainingTime, uint8_t satir_no, uint8_t font = 1, uint8_t scrool = 0);
	string createHttpRequest(string hat_id, string url_path);
	vector<string> getFirst2RemainingTimes(string resp);
	int getVersionFromHost(map<string, string> &versions, string current_version);
	int yeniVersiyonIndir(string name);
	int versiyonIndirildiBilgisiGonder(string dosya_no);
	int genelBilgileriGonder(string current_version);

	vector<string> nameLst;
	vector<string> versionLst;
	vector<string> dosyaNameLst;
	vector<string> md5Lst;
	vector<string> dosyaNoLst;

};

#endif // LEDPANO_H
