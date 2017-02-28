#ifndef MAIN_H
#define MAIN_H

//local headers
#include "ledpano.h"
#include "utility.h"
#include "dbabstraction.h"
#include "logger.h"


#define APP_NAME				"ledpano"
#define VERSION_MAJOR			"1"
#define VERSION_MINOR			"0"
#define VERSION_BUILD			"0"
#define PROGRAM_VERSIYON_FULL	VERSION_MAJOR "." VERSION_MINOR "." VERSION_BUILD

#define WEBSERVICE_PORT 80
#define RETRY_COUNT		5

#define DEFAULT_LOG_LEVEL	LEVEL_TRACE
#define PID_FILE			"/tmp/" APP_NAME ".pid"
#define DEFAULT_LOG_FILE	"/tmp/" APP_NAME ".log"

#define LAST_REBOOTDATE_FILE	"/ekent/lastrebootdate.txt"
#define REBOOT_HOUR		3
#define REBOOT_MIN		58

#define CNTR_VERSION		10 // versiyon sorgulama periyodu(dk)
#define CNTR_GENELBILGI		15 // genel bilgi gonderme periyodu(dk)
#define CNTR_SAAT			30 // saat kontrol etme periyodu(dk)

#define TIMEDIFF_SECS		30 // sistem saatiyle gercek saat arasindaki maksimum fark(sn)

// ERRORS
#define E1	"MAC adresi tanimli degil"
#define E2	"Mac adresine hat id atanmamis"
#define E3	"MAC adresi yeni versiyonda da tanimli degil"
#define E4	"MAC adresine yeni versiyonda da hat id atanmamis"
#define E5	"Sunucuya uzun sure baglanilamamis"
#define E6	"Web servisten alinan icerik bozuk"


extern LedPano *ledpanoObj;
extern Utility *utilObj;
extern DBAbstraction *databaseObj;


#endif // MAIN_H

