#include "ECBCipher.h"
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
	if (cleartext.isEmpty())
	{
		return ENCRYPT_SUCCESS;
	}
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
	string Ciphertext;

	for (int i = 0; i < cleartext.length() / 8; i++) {
		memcpy(inputText, cleartext.toLatin1().data() + i * 8, 8);
		DES_ecb_encrypt(&inputText, &outputText, &keySchedule, DES_ENCRYPT);
		for (int j = 0; j < 8; j++)
			Ciphertext.push_back(outputText[j]);
	}

	if (cleartext.length() % 8 != 0) {
		int tmp1 = cleartext.length() / 8 * 8;
		int tmp2 = cleartext.length() - tmp1;
		memset(inputText, 0, 8);
		memcpy(inputText, cleartext.toLatin1().data() + tmp1, tmp2);

		DES_ecb_encrypt(&inputText, &outputText, &keySchedule, DES_ENCRYPT);

		for (int j = 0; j < 8; j++)
			Ciphertext.push_back(outputText[j]);
	}

	ciphertext.clear();
	ciphertext = QString::fromLatin1(Ciphertext.c_str(), Ciphertext.length());

	return ENCRYPT_SUCCESS;
}

CRYPTO_STATUS ECBCipher::DES_Decrypt(const QString ciphertext, const QString key, QString& cleartext)
{
	if (ciphertext.isEmpty())
	{
		return DECRYPT_SUCCESS;
	}
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
	string Cleartext;

	for (int i = 0; i < ciphertext.length() / 8; i++) {
		memcpy(inputText, ciphertext.toLatin1().data() + i * 8, 8);
		DES_ecb_encrypt(&inputText, &outputText, &keySchedule, DES_DECRYPT);

		for (int j = 0; j < 8; j++)
			Cleartext.push_back(outputText[j]);
	}

	if (ciphertext.length() % 8 != 0) {
		int tmp1 = ciphertext.length() / 8 * 8;
		int tmp2 = ciphertext.length() - tmp1;
		memset(inputText, 0, 8);
		memcpy(inputText, ciphertext.toLatin1().data() + tmp1, tmp2);

		DES_ecb_encrypt(&inputText, &outputText, &keySchedule, DES_DECRYPT);

		for (int j = 0; j < 8; j++)
			Cleartext.push_back(outputText[j]);
	}

	cleartext.clear();
	/*这里第二个参数不用填，原因是Cleartext不是8倍数个字节的话，
	最后会用几个`\0`补齐，这时如果用了 QString::fromLatin1(Cleartext.c_str()，Cleartext.length())形式的话，
	cleartext在最后追加空格，改变文本内容（也就多几个空格）*/
	cleartext = QString::fromLatin1(Cleartext.c_str());
	return DECRYPT_SUCCESS;
}