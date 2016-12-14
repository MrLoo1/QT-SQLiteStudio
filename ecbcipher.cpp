#include "ecbcipher.h"
using namespace std;
ECBCipher::ECBCipher(QObject *parent)
	: Cipher(parent)
{

}

ECBCipher::~ECBCipher()
{

}

CRYPTO_STATUS ECBCipher::DES_Encrypt(const QString cleartext, const QString key, QString& ciphertext)
{
	DES_cblock keyEncrypt;
	memset(keyEncrypt, 0, 8);
	const char* ch = key.toLatin1().data();
	if (key.length() <= 8)
		memcpy(keyEncrypt, key.toLatin1().data(), key.length());
	else
		memcpy(keyEncrypt, key.toLatin1().data(), 8);

	DES_key_schedule keySchedule;
	DES_set_key_unchecked(&keyEncrypt, &keySchedule);

	const_DES_cblock inputText;
	DES_cblock outputText;
	vector<unsigned char> vecCiphertext;
	unsigned char tmp[8];

	for (int i = 0; i < cleartext.length() / 8; i++) {
		memcpy(inputText, cleartext.toLatin1().data() + i * 8, 8);
		DES_ecb_encrypt(&inputText, &outputText, &keySchedule, DES_ENCRYPT);
		memcpy(tmp, outputText, 8);

		for (int j = 0; j < 8; j++)
			vecCiphertext.push_back(tmp[j]);
	}

	if (cleartext.length() % 8 != 0) {
		int tmp1 = cleartext.length() / 8 * 8;
		int tmp2 = cleartext.length() - tmp1;
		memset(inputText, 0, 8);
		memcpy(inputText, cleartext.toLatin1().data() + tmp1, tmp2);

		DES_ecb_encrypt(&inputText, &outputText, &keySchedule, DES_ENCRYPT);
		memcpy(tmp, outputText, 8);

		for (int j = 0; j < 8; j++)
			vecCiphertext.push_back(tmp[j]);
	}

	ciphertext.clear();
	string s = string(vecCiphertext.begin(), vecCiphertext.end());
	for (string::iterator i = s.begin(); i != s.end(); i++)
	{
		ciphertext += QString::fromLatin1(&(*i),1);
		//char ccc = *i;
		//ccc = ' ';
	}
	return ENCRYPT_SUCCESS;
}

CRYPTO_STATUS ECBCipher::DES_Decrypt(const QString ciphertext, const QString key, QString& cleartext)
{
	DES_cblock keyEncrypt;
	memset(keyEncrypt, 0, 8);

	if (key.length() <= 8)
		memcpy(keyEncrypt, key.toLatin1().data(), key.length());
	else
		memcpy(keyEncrypt, key.toLatin1().data(), 8);

	DES_key_schedule keySchedule;
	DES_set_key_unchecked(&keyEncrypt, &keySchedule);

	const_DES_cblock inputText;
	DES_cblock outputText;
	vector<unsigned char> vecCleartext;
	unsigned char tmp[8];

	for (int i = 0; i < ciphertext.length() / 8; i++) {
		memcpy(inputText, ciphertext.toLatin1().data() + i * 8, 8);
		DES_ecb_encrypt(&inputText, &outputText, &keySchedule, DES_DECRYPT);
		memcpy(tmp, outputText, 8);

		for (int j = 0; j < 8; j++)
			vecCleartext.push_back(tmp[j]);
	}

	if (ciphertext.length() % 8 != 0) {
		int tmp1 = ciphertext.length() / 8 * 8;
		int tmp2 = ciphertext.length() - tmp1;
		memset(inputText, 0, 8);
		memcpy(inputText, ciphertext.toLatin1().data() + tmp1, tmp2);

		DES_ecb_encrypt(&inputText, &outputText, &keySchedule, DES_DECRYPT);
		memcpy(tmp, outputText, 8);

		for (int j = 0; j < 8; j++)
			vecCleartext.push_back(tmp[j]);
	}

	cleartext.clear();
	cleartext = QString::fromLatin1(string(vecCleartext.begin(), vecCleartext.end()).c_str());
	return DECRYPT_SUCCESS;
}