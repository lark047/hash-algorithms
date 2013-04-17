#ifndef HMAC_H_
#define HMAC_H_

uint8_t *HMAC(const uint8_t *, uint64_t, const uint8_t *, uint64_t, uint8_t *(*)(const uint8_t *, uint64_t));
uint8_t *HMACstring(const char *, const char *, uint8_t *(*)(const uint8_t *, uint64_t));
/* TODO HMAC file */

#define HMAC_MD5(k, m) (HMACstring(k, m, MD5))
/* etc. */

#endif /* HMAC_H_ */
