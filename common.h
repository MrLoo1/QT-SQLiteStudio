#ifndef COMMON_H
#define COMMON_H

#define RS_ICON_WINDOW					":/SQLiteStudio/Resources/images/DB_remote.ico"
#define RS_ICON_Clear					":/SQLiteStudio/Resources/images/clear.png"
#define RS_ICON_OPENFILE				":/SQLiteStudio/Resources/images/folder.png"

#define RS_Dlg_Info						":/SQLiteStudio/Resources/images/info.png"
#define RS_Dlg_Finish					":/SQLiteStudio/Resources/images/finish.png"
#define RS_Dlg_Warning					":/SQLiteStudio/Resources/images/warning.png"
#define RS_Dlg_Error					":/SQLiteStudio/Resources/images/error.png"
#define RS_Dlg_Confirm					":/SQLiteStudio/Resources/images/confirm.png"
#define RS_Tab_Close                    ":/SQLiteStudio/Resources/images/tab_close"
#define RS_Tab_Close_H                  ":/SQLiteStudio/Resources/images/tab_close_h"

#define QSS_MAINWINDOW					":/qss/Resources/qss/mainwindow.qss"

#define SETTING_NAME_CREATEDB_FIELPATH	"CreatDBFilePath"
#define SETTING_NAME_SQL_FIELPATH		"SQLFilePath"
#define SETTING_NAME_SEEDB_FIEL_PATH	"SeeDBFilePath"
#define SETTING_NAME_PASSWORD_PATH		"Password"

#define SETTING_VALUE_SPLIT				"|"

#define QSL(str)						QStringLiteral(str)

#define PW_KEY							QSL("cheng668")

#define  ROWHEIGHT_DEF			28
#define  SQLITECIPHER_BUSY_TIMEOUT			"QSQLITE_BUSY_TIMEOUT"
#define  SQLITECIPHER_UPDATE_KEY(key)		QSL("QSQLITE_UPDATE_KEY=%1").arg(key)
#define  SQLITECIPHER_OPEN_READONLY			"QSQLITE_OPEN_READONLY"
#define  SQLITECIPHER_OPEN_URI				"QSQLITE_OPEN_URI"
#define  SQLITECIPHER_ENABLE_SHARED_CACHE   "QSQLITE_ENABLE_SHARED_CACHE"
#define  SQLITECIPHER_CREATE_KEY			"QSQLITE_CREATE_KEY"
#define  SQLITECIPHER_REMOVE_KEY			"QSQLITE_REMOVE_KEY"

#define  EDPASSWORD_MAX						32

#endif // COMMON_H