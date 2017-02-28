/*
 * dbabstraction.cpp
 *
 * author:fadil
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
//#include <sys/types.h>
//#include <sys/socket.h>
//#include <netinet/in.h>
//#include <arpa/inet.h>
//#include <netdb.h>
#include <linux/kernel.h>

//#include <QtSql/QSqlQuery>
//#include <QtSql/QSqlRecord>
//#include <QtSql/QSqlDatabase>
//#include <QtCore/QVariant>
//#include <QtCore/QTextCodec>
//#include <QtCore/QFile>
//#include <QtCore/QString>
//#include <QtCore/QStringList>
#include <QUrl>

//local headers
//#include "main.h"
#include "utility.h"
#include "dbabstraction.h"
#include "logger.h"


#define ID_TABLE			"DEVICE_HAT_ID"
#define PARAMETERS_TABLE	"DEVICE_PARAMETERS"
#define EKENT_DB_NAME		"ekentdb"
#define LEDPANO_DB_PATH		"/ekent/db/LedPanoDB.s3db"

using namespace std;

DBAbstraction::DBAbstraction(void)
{
	printf("DBAbstraction Constructor!\n");

//	QTextCodec::setCodecForCStrings(QTextCodec::codecForName("ISO 8859-9"));

	QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
	db.setDatabaseName(LEDPANO_DB_PATH);

	if (!db.open()) {
		FATAL("DB Connection Failed!\n");
		return;
	}

	if (!getParameters())
	{
		FATAL("Couldn't get parameters from DB!\n");
	}
}

DBAbstraction::~DBAbstraction(void)
{
	printf("DBAbstraction Distructor\n");
}


bool DBAbstraction::getHatId(string macAddr, string &hat_id)
{
	QString mac_addr = QString::fromStdString(macAddr);
	QString str = "select * from DEVICE_HAT_ID where MAC='" + mac_addr + "'";
	QSqlQuery query(str);

	if (query.next())
	{
		QSqlRecord rec = query.record();
		hat_id = rec.value("HAT_ID").toString().toUtf8().data();
		return true;
	}
	else
		return false;
}

bool DBAbstraction::getParameters(void)
{
	try
	{
		QString str = "select * from DEVICE_PARAMETERS";
		QSqlQuery query(str);

		if (query.next())
		{
			QSqlRecord rec = query.record();
			url = rec.value("WEBSERVICE_URL").toString().toUtf8().data();
			server_ip = rec.value("SERVER_IP_PROD").toString().toUtf8().data();
			update_period = rec.value("UPDATE_PERIOD").toInt();
			city_id = rec.value("CITY_ID").toInt();
			db_version = rec.value("DB_VER").toString().toUtf8().data();
		}

		if (url.size() < 7)
		{
			//printf("Couldn't get url from DB! Url:%s\n", url.c_str());
			ERROR("Couldn't get url from DB! Url:%s\n", url.c_str());
			return false;
		}
		string http = url.substr(0,7);
		if(http != "http://")
			url = "http://" + url;

		DEBUG("url:%s\n", url.c_str());

		QUrl web_url(url.c_str());
//		web_url.setScheme("http");

		QString host = web_url.host();
		url_host = host.toStdString();
		DEBUG("host:%s\n", url_host.c_str());

		QString path = web_url.path();
		url_path = path.toStdString();
		if(url.size() > url_host.size() + url_path.size() + 7) // "http://".size() = 7
			url_path = url_path + url.substr(url_host.size() + url_path.size() + 7);

		DEBUG("path:%s\n", url_path.c_str());

	}
	catch (...)
	{
		FATAL("Couldn't select device parameters from db!\n");
		return false;
	}

	return true;
}
