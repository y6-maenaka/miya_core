#ifndef ECEC754B_A279_4E21_839C_A23580D6E2FA 
#define ECEC754B_A279_4E21_839C_A23580D6E2FA

#include "openssl/evp.h"
#include <iostream>
#include <stdlib.h>

#define DIGEST_NAME_SHA512 "sha512"
#define DIGEST_NAME_SHA256 "sha256"
#define DIGEST_NAME_SHA1 "sha1"

namespace hash{

unsigned char* SHAHash( unsigned char* HashMessage , size_t HashMessageSize, char* SHAtype );

}
#endif // ECEC754B_A279_4E21_839C_A23580D6E2FA
