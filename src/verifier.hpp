#include <openssl/evp.h>
#include <vector>
#include <stdexcept>

class SignatureVerifier {
public:
    SignatureVerifier(const std::vector<unsigned char>& public_key_data);
    ~SignatureVerifier();
    
    bool Verify(const std::vector<unsigned char>& data, const std::vector<unsigned char>& signature);

private:
    EVP_PKEY* public_key = nullptr;
};

#include <openssl/bio.h>
#include <openssl/pem.h>

SignatureVerifier::SignatureVerifier(const std::vector<unsigned char>& public_key_data) {
    BIO* bio = BIO_new_mem_buf(public_key_data.data(), public_key_data.size());
    if (!PEM_read_bio_PUBKEY(bio, &public_key, nullptr, nullptr)) {
        BIO_free(bio);
        throw std::runtime_error("Failed to load public key");
    }
    BIO_free(bio);
}

SignatureVerifier::~SignatureVerifier() {
    if (public_key) EVP_PKEY_free(public_key);
}

bool SignatureVerifier::Verify(
    const std::vector<unsigned char>& data,
    const std::vector<unsigned char>& signature
) {
    EVP_MD_CTX* hash_ctx = EVP_MD_CTX_new();
    EVP_DigestInit_ex(hash_ctx, EVP_sha512(), nullptr);
    EVP_DigestUpdate(hash_ctx, data.data(), data.size());
    
    unsigned char hash[EVP_MAX_MD_SIZE];
    unsigned int hash_len;
    EVP_DigestFinal_ex(hash_ctx, hash, &hash_len);
    EVP_MD_CTX_free(hash_ctx);

    EVP_PKEY_CTX* pkey_ctx = EVP_PKEY_CTX_new(public_key, nullptr);
    EVP_PKEY_verify_init(pkey_ctx);
    EVP_PKEY_CTX_set_rsa_padding(pkey_ctx, RSA_PKCS1_PSS_PADDING);
    EVP_PKEY_CTX_set_signature_md(pkey_ctx, EVP_sha512());

    int result = EVP_PKEY_verify(
        pkey_ctx,
        signature.data(),
        signature.size(),
        hash,
        hash_len
    );

    EVP_PKEY_CTX_free(pkey_ctx);
    return (result == 1);
}