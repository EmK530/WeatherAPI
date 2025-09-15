#include <stdio.h>
#include <string.h>
#include <openssl/sha.h> /* install libssl-dev */

void sha256_hash(const char *str, char *outHash) {
    unsigned char digest[SHA256_DIGEST_LENGTH];
    SHA256((unsigned char*)str, strlen(str), digest);
    int i;
    for (i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        sprintf(&outHash[i*2], "%02x", digest[i]);
    }
    outHash[SHA256_DIGEST_LENGTH*2] = '\0';
}