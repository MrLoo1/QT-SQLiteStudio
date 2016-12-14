#ifndef ECBCIPHER_H
#define ECBCIPHER_H

#include "Cipher.h"

class ECBCipher : public Cipher
{
	Q_OBJECT

public:
	ECBCipher(QObject *parent = 0);
	virtual ~ECBCipher();
	virtual CRYPTO_STATUS DES_Encrypt(const QString cleartext, const  QString key, QString& ciphertext) override;
	virtual CRYPTO_STATUS DES_Decrypt(const QString ciphertext, const  QString key, QString& cleartext) override;
	
};

#endif // ECBCIPHER_H
