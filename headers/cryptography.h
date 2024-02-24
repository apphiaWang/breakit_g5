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
#define PRIV_KEY_LOC "/.metadata/private_keys/"
#endif

#ifndef PUB_KEY_LOC
#define PUB_KEY_LOC "public_keys/"
#endif

bool create_user_key(const std::string &username, bool isAdmin) {
    bool ret = 0;
    int bits = 4096;
    RSA *rsa = nullptr;
    BIGNUM *bne = nullptr;
    BIO *bp_public = nullptr, *bp_private = nullptr;
    unsigned long e = RSA_F4;

    bne = BN_new();
    if (!BN_set_word(bne, e)) {
        BN_free(bne);
        return false;
    }

    rsa = RSA_new();

    if (!RSA_generate_key_ex(rsa, bits, bne, nullptr)) {
        RSA_free(rsa);
        BN_free(bne);
        return false;
    }

    bp_public = BIO_new_file((std::string(PUB_KEY_LOC) + username + "_pub.pem").c_str(), "w+");
    if (!PEM_write_bio_RSAPublicKey(bp_public, rsa)) {
        BIO_free_all(bp_public);
        RSA_free(rsa);
        BN_free(bne);
        return false;
    }
    bp_private = BIO_new_file(("./filesystem/" + username + std::string(PRIV_KEY_LOC) + username + "_priv.pem").c_str(), "w+");
    if (!PEM_write_bio_RSAPrivateKey(bp_private, rsa, nullptr, nullptr, 0, nullptr, nullptr)) {
        BIO_free_all(bp_private);
        RSA_free(rsa);
        BN_free(bne);
        return false;
    }
    std::filesystem::permissions(("./filesystem/" + username + std::string(PRIV_KEY_LOC) + username + "_priv.pem").c_str(), std::filesystem::perms::owner_read | std::filesystem::perms::group_read| std::filesystem::perms::others_read, std::filesystem::perm_options::replace);

    BIO_free(bp_public);
    BIO_free(bp_private);
    RSA_free(rsa);
    BN_free(bne);

    std::cout << "Keys generated and saved for user: " << username << std::endl;
    return true;
}

std::string encrypt_plaintext(const std::string &plaintext, const std::string &username) {
    const int bits = 4096;
    const int max_length = bits / 8 - 42; // Adjusted for RSA_PKCS1_OAEP_PADDING
    if (plaintext.empty() || plaintext.length() > max_length) {
        std::cerr << "Plaintext is invalid due to length constraints." << std::endl;
        return "";
    }

    // Load the public key
    std::string publicKeyPath = std::string(PUB_KEY_LOC) + username + "_pub.pem";
    BIO *pubKeyFile = BIO_new_file(publicKeyPath.c_str(), "r");
    if (!pubKeyFile) {
        std::cerr << "Error opening public key file." << std::endl;
        return "";
    }

    RSA *rsaPubKey = PEM_read_bio_RSAPublicKey(pubKeyFile, nullptr, nullptr, nullptr);
    BIO_free_all(pubKeyFile); // Free BIO object once we're done with it

    if (!rsaPubKey) {
        std::cerr << "Error loading public key." << std::endl;
        ERR_print_errors_fp(stderr);
        return "";
    }

    // Prepare for encryption
    int padding = RSA_PKCS1_OAEP_PADDING;
    size_t encryptLen = RSA_size(rsaPubKey);
    std::vector<unsigned char> encryptedData(encryptLen);

    // Perform the encryption
    int result = RSA_public_encrypt(plaintext.length(), reinterpret_cast<const unsigned char *>(plaintext.c_str()),
                                    encryptedData.data(), rsaPubKey, padding);

    // Free the RSA key
    RSA_free(rsaPubKey);

    if (result == -1) {
        std::cerr << "Encryption failed." << std::endl;
        ERR_print_errors_fp(stderr);
        return "";
    }

    // Return the encrypted data as a string
    return {reinterpret_cast<char *>(encryptedData.data()), static_cast<size_t>(result)};
}

std::string decrypt_ciphertext(std::string ciphertext, const std::string &username) {
    if (ciphertext.empty()) {
        std::cerr << "Ciphertext is empty" << std::endl;
    }
    BIO *privateKeyFile = BIO_new_file(("./filesystem/" + username + std::string (PRIV_KEY_LOC) + username + "_priv.pem").c_str(), "r");
    if(!privateKeyFile){
        BIO_free_all(privateKeyFile);
        ERR_print_errors_fp(stderr);
        return "";
    }
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

    return std::string{reinterpret_cast<char *>(decryptedData.data()), static_cast<size_t>(result)};
}

#endif //CMPT785_G5_SECURE_FILESYSTEM_ENCRYPTION_H