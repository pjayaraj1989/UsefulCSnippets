#include <stdio.h>
#include <string.h>
#include <openssl/evp.h>
#include <openssl/rand.h>

#define AES_KEYLEN 32 // 256 bits
#define AES_IVLEN 12  // 96 bits for GCM
#define TAG_LEN 16

void handleErrors()
{
    ERR_print_errors_fp(stderr);
    abort();
}

int main()
{
    EVP_CIPHER_CTX *ctx;
    /* hardcode key, iv */
    unsigned char key[AES_KEYLEN] = {
        0x60, 0x3d, 0xeb, 0x10, 0x15, 0xca, 0x71, 0xbe,
        0x2b, 0x73, 0xae, 0xf0, 0x85, 0x7d, 0x77, 0x81,
        0x1f, 0x35, 0x2c, 0x07, 0x3b, 0x61, 0x08, 0xd7,
        0x2d, 0x98, 0x10, 0xa3, 0x09, 0x14, 0xdf, 0xf4};
    unsigned char iv[AES_IVLEN] = {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0a, 0x0b};
    unsigned char tag[TAG_LEN];

    // Generate random key and IV
    RAND_bytes(key, sizeof(key));
    RAND_bytes(iv, sizeof(iv));

    // Sample plaintext
    unsigned char plaintext[] = "This is a test message for AES-GCM multi-update encryption.";
    int plaintext_len = strlen((char *)plaintext);

    // Buffers for ciphertext and decrypted text
    unsigned char ciphertext[128];
    unsigned char decryptedtext[128];
    int len, ciphertext_len = 0, decryptedtext_len = 0;

    // --- Encryption ---
    /* print plaintext in bytes */
    printf("Plaintext length: %d\n", plaintext_len);
    printf("Plaintext (hex): ");
    for (int i = 0; i < plaintext_len; i++)
    {
        printf("%02x", plaintext[i]);
    }
    printf("\n");

    ctx = EVP_CIPHER_CTX_new();
    EVP_EncryptInit_ex(ctx, EVP_aes_256_gcm(), NULL, NULL, NULL);
    EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_IVLEN, AES_IVLEN, NULL);
    EVP_EncryptInit_ex(ctx, NULL, NULL, key, iv);

    /* generate a random int between 0 and 3 for encrypt update */
    int random_update = rand() % 4;
    printf("Random update count for encryption: %d\n", random_update);

    // Multi-update encryption
    int chunk_size = plaintext_len / random_update;
    int offset = 0;
    for (int i = 0; i < random_update; i++)
    {
        int current_chunk = (i == random_update - 1) ? (plaintext_len - offset) : chunk_size;
        EVP_EncryptUpdate(ctx, ciphertext + ciphertext_len, &len, plaintext + offset, current_chunk);
        ciphertext_len += len;
        offset += current_chunk;
        printf("Ciphertext length after update %d: %d\n", i + 1, ciphertext_len);
        printf("Ciphertext (hex) after update %d: ", i + 1);
        for (int j = 0; j < ciphertext_len; j++)
        {
            printf("%02x", ciphertext[j]);
        }
        printf("\n");
    }

    // Finalize encryption
    EVP_EncryptFinal_ex(ctx, ciphertext + ciphertext_len, &len);
    ciphertext_len += len;
    printf("Final ciphertext length: %d\n", ciphertext_len);
    printf("Final ciphertext: ");
    for (int i = 0; i < ciphertext_len; i++)
    {
        printf("%02x", ciphertext[i]);
    }
    printf("\n");

    // Get the tag
    EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_GET_TAG, TAG_LEN, tag);
    printf("Generated tag after encryption: ");
    for (int i = 0; i < TAG_LEN; i++)
    {
        printf("%02x", tag[i]);
    }
    printf("\n");
    EVP_CIPHER_CTX_free(ctx);

    // --- Decryption ---
    ctx = EVP_CIPHER_CTX_new();
    EVP_DecryptInit_ex(ctx, EVP_aes_256_gcm(), NULL, NULL, NULL);
    EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_IVLEN, AES_IVLEN, NULL);
    EVP_DecryptInit_ex(ctx, NULL, NULL, key, iv);

    random_update = rand() % 4;
    printf("Random update count for decryption: %d\n", random_update);
    chunk_size = ciphertext_len / random_update;
    offset = 0;
    for (int i = 0; i < random_update; i++)
    {
        int current_chunk = (i == random_update - 1) ? (ciphertext_len - offset) : chunk_size;
        EVP_DecryptUpdate(ctx, decryptedtext + decryptedtext_len, &len, ciphertext + offset, current_chunk);
        decryptedtext_len += len;
        offset += current_chunk;
        printf("Decrypted text length after update %d: %d\n", i + 1, decryptedtext_len);
        printf("Decrypted text (hex) after update %d: ", i + 1);
        for (int j = 0; j < decryptedtext_len; j++)
        {
            printf("%02x", decryptedtext[j]);
        }
        printf("\n");
    }
    printf("Decrypted text (before finalization): ");
    for (int i = 0; i < decryptedtext_len; i++)
    {
        printf("%02x", decryptedtext[i]);
    }
    printf("\n");
    // Set expected tag value
    EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_TAG, TAG_LEN, tag);
    printf("Expected tag: ");
    for (int i = 0; i < TAG_LEN; i++)
    {
        printf("%02x", tag[i]);
    }
    printf("\n");

    // Finalize decryption
    if (EVP_DecryptFinal_ex(ctx, decryptedtext + decryptedtext_len, &len) > 0)
    {
        decryptedtext_len += len;
        printf("Decrypted text length after finalization: %d\n", decryptedtext_len);
        decryptedtext[decryptedtext_len] = '\0';
        printf("Decrypted text: %s\n", decryptedtext);

        // Compare decrypted text with the original plaintext
        int compare_result = memcmp(plaintext, decryptedtext, plaintext_len);
        if (compare_result == 0)
        {
            printf("Decryption successful: Decrypted text matches original plaintext\n");
            EVP_CIPHER_CTX_free(ctx);
            return 0;
        }
        else
        {
            printf("Decryption error: Decrypted text does not match original plaintext\n");
            EVP_CIPHER_CTX_free(ctx);
            return 1;
        }
    }
    else
    {
        printf("Decryption failed: Tag mismatch!\n");
        EVP_CIPHER_CTX_free(ctx);
        return 1;
    }

    EVP_CIPHER_CTX_free(ctx);
    return 0;
}
