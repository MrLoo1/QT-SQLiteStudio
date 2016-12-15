#pragma once
#include <QObject>
#include "Cipher.h"
class CBCCipher :
	public Cipher
{
	Q_OBJECT
public:
	CBCCipher(QObject *parent = 0);
	virtual ~CBCCipher();
	virtual CRYPTO_STATUS DES_Encrypt(const QString cleartext, const  QString key, QString& ciphertext) override;
	virtual CRYPTO_STATUS DES_Decrypt(const QString ciphertext, const  QString key, QString& cleartext) override;
};

