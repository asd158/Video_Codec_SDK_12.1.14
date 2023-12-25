/**
 * Copyright (c) 2018 rxi
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the MIT license. See LICENSE for details.
 */

#include <stdio.h>
#include <stdint.h>

#if defined(_WIN32)
#include <windows.h>
#include <wincrypt.h>

#endif
#include "uuid4.h"

bool            initOk;
static uint64_t seed[2];
static uint64_t xorshift128plus(uint64_t *s) {
    /* http://xorshift.di.unimi.it/xorshift128plus.c */
    uint64_t       s1 = s[0];
    const uint64_t s0 = s[1];
    s[0] = s0;
    s1 ^= s1 << 23;
    s[1] = s1 ^ s0 ^ (s1 >> 18) ^ (s0 >> 5);
    return s[1] + s0;
}
bool uuid4_init(void) {
    if (initOk) {
        return true;
    }
    initOk = true;
#if defined(__linux__) || defined(__APPLE__) || defined(__FreeBSD__) || defined(__EMSCRIPTEN__)
    int res;
    FILE *fp = fopen("/dev/urandom", "rb");
    if (!fp) {
      return false;
    }
    res = fread(seed, 1, sizeof(seed), fp);
    fclose(fp);
    if ( res != sizeof(seed) ) {
      return false;
    }

#elif defined(_WIN32)
    int        res;
    HCRYPTPROV hCryptProv;
    res = CryptAcquireContext(
            &hCryptProv,
            nullptr,
            nullptr,
            PROV_RSA_FULL,
            CRYPT_VERIFYCONTEXT);
    if (!res) {
        initOk = false;
        return initOk;
    }
    res = CryptGenRandom(hCryptProv,
                         (DWORD) sizeof(seed),
                         (PBYTE) seed);
    CryptReleaseContext(hCryptProv,
                        0);
    if (!res) {
        initOk = false;
        return initOk;
    }
#else
#error "unsupported platform"
#endif
    return initOk;
}
void uuid4_generate(char *dst) {
    if (!initOk) {
        uuid4_init();
    }
    if (!initOk) {
        return;
    }
    static const char *templatexx = "xxxxxxxx-xxxx-4xxx-yxxx-xxxxxxxxxxxx";
    static const char *chars      = "0123456789abcdef";
    union {
        unsigned char b[16];
        uint64_t      word[2];
    }                 s{};
    const char        *p;
    int               i, n;
    /* get random */
    s.word[0] = xorshift128plus(seed);
    s.word[1] = xorshift128plus(seed);
    /* build string */
    p    = templatexx;
    i    = 0;
    while (*p) {
        n = s.b[i >> 1];
        n = (i & 1) ? (n >> 4) : (n & 0xf);
        switch (*p) {
            case 'x'  :
                *dst = chars[n];
                i++;
                break;
            case 'y'  :
                *dst = chars[(n & 0x3) + 8];
                i++;
                break;
            default   :
                *dst = *p;
        }
        dst++, p++;
    }
    *dst = '\0';
}
