#ifndef CMPT785_G5_SECURE_FILESYSTEM_ENCRYPTION_H
#define CMPT785_G5_SECURE_FILESYSTEM_ENCRYPTION_H

#include <openssl/aes.h>
#include <openssl/pem.h>
#include <openssl/rsa.h>
#include <openssl/evp.h>
#include <openssl/bio.h>
#include <openssl/err.h>
#include <iostream>
#include <iomanip>

#ifndef PRIV_KEY_LOC
#define PRIV_KEY_LOC "filesystem/metadata/private_keys/"
#endif

#ifndef PUB_KEY_LOC
#define PUB_KEY_LOC "filesystem/public_keys/"
#endif

int create_user_key(const std::string &username) {
    int ret = 0, bits = 4096;
    RSA *rsa = nullptr;
    BIGNUM *bne = nullptr;
    BIO *bp_public = nullptr, *bp_private = nullptr;
    unsigned long e = RSA_F4;

    bne = BN_new();
    ret = BN_set_word(bne, e);

    if (ret != 1) goto free_all;

    rsa = RSA_new();
    ret = RSA_generate_key_ex(rsa, bits, bne, nullptr);
    if (ret != 1) goto free_all;

    bp_public = BIO_new_file((username + "_pub.pem").c_str(), "w+");
    if (!bp_public) {
        std::cerr << "Failed to create new file" << std::endl;
        goto free_all;
    }
    ret = PEM_write_bio_RSAPublicKey(bp_public, rsa);
    if (ret != 1) goto free_all;

    bp_private = BIO_new_file((username + "_priv.pem").c_str(), "w+");
    if (!bp_private) {
        std::cout << "Failed to create new file" << std::endl;
        goto free_all;
    }
    ret = PEM_write_bio_RSAPrivateKey(bp_private, rsa, nullptr, nullptr, 0, nullptr, nullptr);

    free_all:
        BIO_free(bp_public);
        BIO_free(bp_private);
        RSA_free(rsa);
        BN_free(bne);

    return (ret == 1);

}

std::string encrypt_plaintext(const std::string &plaintext, const std::string &username) {
    int bits = 4096;
    int max_length = bits / 8 - 66; //Max length for OAEP Padding is 446 bytes
    BIO *pubKeyFile = BIO_new_file((username + "_pub.pem").c_str(), "r");

    RSA *rsaPubKey = PEM_read_bio_RSAPublicKey(pubKeyFile, nullptr, nullptr, nullptr);


    if (rsaPubKey == nullptr) {
        std::cerr << "Error loading public key." << std::endl;
        ERR_print_errors_fp(stderr);
        //throw error
    }
    if (plaintext.empty() || plaintext.length() > max_length) {
        std::cerr << "Plaintext is invalid" << std::endl;
    }
    int padding = RSA_PKCS1_OAEP_PADDING;
    int encryptLen = RSA_size(rsaPubKey);
    std::vector<unsigned char> encryptedData(encryptLen);

    int result = RSA_public_encrypt(plaintext.length(),
                                    reinterpret_cast<const unsigned char *>(plaintext.c_str()),
                                    encryptedData.data(), rsaPubKey, padding);
    if (result == -1) {
        std::cerr << "Encryption failed." << std::endl;
        ERR_print_errors_fp(stderr);
        RSA_free(rsaPubKey);
        //throw err
    }

    RSA_free(rsaPubKey);
    EVP_cleanup();
    ERR_free_strings();

    return std::string(reinterpret_cast<const char *>(encryptedData.data()), RSA_size(rsaPubKey));
}

std::string decrypt_ciphertext(std::string ciphertext, const std::string &location) {
    if (ciphertext.empty()) {
        std::cerr << "Ciphertext is empty" << std::endl;
    }
    BIO *privateKeyFile = BIO_new_file((location).c_str(), "r");
    RSA *rsaPrivKey = PEM_read_bio_RSAPrivateKey(privateKeyFile, nullptr, nullptr, nullptr);
    if (rsaPrivKey == nullptr) {
        std::cerr << "Error loading private key." << std::endl;
        ERR_print_errors_fp(stderr);
    }
    int padding = RSA_PKCS1_OAEP_PADDING;

    int decryptLen = RSA_size(rsaPrivKey);
    std::vector<unsigned char> decryptedData(decryptLen);
    int result = RSA_private_decrypt(ciphertext.size(), reinterpret_cast<const unsigned char *>(ciphertext.data()),
                                     decryptedData.data(), rsaPrivKey, padding);
    if (result == -1) {
        std::cerr << "Decryption failed." << std::endl;
        ERR_print_errors_fp(stderr);
        RSA_free(rsaPrivKey);
    }

    RSA_free(rsaPrivKey);
    EVP_cleanup();
    ERR_free_strings();

    return std::string(reinterpret_cast<char *>(decryptedData.data()), result);
}

#endif //CMPT785_G5_SECURE_FILESYSTEM_ENCRYPTION_H