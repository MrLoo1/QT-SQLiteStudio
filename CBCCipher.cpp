#include "CBCCipher.h"

using namespace std;
static unsigned char cbc_iv[8] = { '0', '1', 'A', 'B', 'a', 'b', '9', '8' };

CBCCipher::CBCCipher(QObject *parent) :Cipher(parent)
{
}


CBCCipher::~CBCCipher()
{
}

CRYPTO_STATUS CBCCipher::DES_Encrypt(const QString cleartext, const QString key, QString& ciphertext)
{
	if (cleartext.isEmpty())
	{
		return ENCRYPT_SUCCESS;
	}
	DES_cblock keyEncrypt, ivec;
	memset(keyEncrypt, 0, 8);

	if (key.length() <= 8)
		memcpy(keyEncrypt, key.toLatin1().data(), key.length());
	else
		memcpy(keyEncrypt, key.toLatin1().data(), 8);

	DES_key_schedule keySchedule;
	DES_set_key_unchecked(&keyEncrypt, &keySchedule);

	memcpy(ivec, cbc_iv, sizeof(cbc_iv));

	int iLength = cleartext.length() % 8 ? (cleartext.length() / 8 + 1) * 8 : cleartext.length();
	unsigned char* tmp = new unsigned char[iLength + 16];
	memset(tmp, 0, iLength);

	DES_ncbc_encrypt((const unsigned char*)cleartext.toLatin1().data(), tmp, cleartext.length(), &keySchedule, &ivec, DES_ENCRYPT);

	string s = string(reinterpret_cast<char*>(tmp));
	ciphertext = QString::fromLatin1(s.c_str(),s.length());



	delete[] tmp;
	return ENCRYPT_SUCCESS;
}

CRYPTO_STATUS CBCCipher::DES_Decrypt(const QString ciphertext, const QString key, QString& cleartext)
{
	if (ciphertext.isEmpty())
	{
		return DECRYPT_SUCCESS;
	}
	DES_cblock keyEncrypt, ivec;
	memset(keyEncrypt, 0, 8);

	if (key.length() <= 8)
		memcpy(keyEncrypt, key.toLatin1().data(), key.length());
	else
		memcpy(keyEncrypt, key.toLatin1().data(), 8);

	DES_key_schedule keySchedule;
	DES_set_key_unchecked(&keyEncrypt, &keySchedule);

	memcpy(ivec, cbc_iv, sizeof(cbc_iv));

	int iLength = ciphertext.length() % 8 ? (ciphertext.length() / 8 + 1) * 8 : ciphertext.length();
	unsigned char* tmp = new unsigned char[iLength];
	memset(tmp, 0, iLength);

	DES_ncbc_encrypt((const unsigned char*)ciphertext.toLatin1().data(), tmp, ciphertext.length(), &keySchedule, &ivec, DES_DECRYPT);

	string s = string(reinterpret_cast<char*>(tmp));
	cleartext = QString::fromLatin1(s.c_str());

	delete[] tmp;
	return DECRYPT_SUCCESS;
}