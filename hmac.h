#ifndef HMAC_H_
#define HMAC_H_

uint8_t *HMAC(const uint8_t *, uint64_t, const uint8_t *, uint64_t, uint8_t *(*)(const uint8_t *, uint64_t), uint8_t);
uint8_t *HMACstring(const char *, const char *, uint8_t *(*)(const uint8_t *, uint64_t), uint8_t);
/* TODO HMAC file? */

/* TODO put B here... should go elsewhere? */
#define HMAC_MD2(k, m)  (HMACstring((k), (m), MD2, 16))
#define HMAC_MD4(k, m)  (HMACstring((k), (m), MD4, 64))
#define HMAC_MD5(k, m)  (HMACstring((k), (m), MD5, 64))
/* HMAC_MD6 */

/* HMAC_SHA0*/
#define HMAC_SHA1(k, m) (HMACstring((k), (m), SHA1, 64))
/* etc. */

#endif /* HMAC_H_ */
