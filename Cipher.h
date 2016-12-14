#pragma once
#include <QObject>
#include "openssl/des.h"

typedef enum {
	GENERAL = 0,
	ECB,
	CBC,
	CFB,
	OFB,
	TRIPLE_ECB,
	TRIPLE_CBC
}CRYPTO_MODE;

typedef enum {
	FAIL = 0,
	ENCRYPT_SUCCESS,
	DECRYPT_SUCCESS
}CRYPTO_STATUS;

/*²ßÂÔÄ£Ê½*/
class Cipher:public QObject
{
	Q_OBJECT
public:
	Cipher(QObject* parent = 0);
	virtual ~Cipher();

	virtual CRYPTO_STATUS DES_Encrypt(const QString cleartext,const QString key, QString& ciphertext) = 0;
	virtual CRYPTO_STATUS DES_Decrypt(const QString ciphertext,const QString key, QString& cleartext) = 0;
};

