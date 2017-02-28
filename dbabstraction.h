#ifndef DBABSTACTION_H
#define DBABSTACTION_H

#include <stdio.h>
#include <iostream>
#include <vector>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlRecord>
#include <QtSql/QSqlDatabase>
#include <QtCore/QVariant>
#include <QtCore/QTextCodec>
#include <QtCore/QFile>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include "stdint.h"
#include "stddef.h"
#include "utility.h"
#include "device.h"


using namespace std;

class DBAbstraction
{
	bool getParameters();

public:
	DBAbstraction(void);
	virtual ~DBAbstraction(void);

	bool getHatId(string macAddr, string &hat_id);

	string url;
	string server_ip;
	int update_period;
	int city_id;
	string db_version;
	string url_path;
	string url_host;

};

#endif // DBABSTACTION_H
