#ifndef HMAC_H_
#define HMAC_H_

uint8_t *HMAC(const uint8_t *, uint64_t, const uint8_t *, uint64_t, uint8_t *(*)(const uint8_t *, uint64_t), uint8_t, uint8_t);
uint8_t *HMACstring(const char *, const char *, uint8_t *(*)(const uint8_t *, uint64_t), uint8_t, uint8_t);
/* TODO HMAC file? */

/* TODO put B here... should go elsewhere? */
/* TODO put digest_length here... should go elsewhere? */
/* HMAC_MD6 */

/* HMAC_SHA0 */
#define HMAC_SHA256(k, m) (HMACstring((k), (m), SHA256,  64, 32))
#define HMAC_SHA384(k, m) (HMACstring((k), (m), SHA384, 128, 48))
#define HMAC_SHA512(k, m) (HMACstring((k), (m), SHA512, 128, 64))
/* etc. */

#if 0
#  /* TODO */ define HMAC_SHA224(k, m) (HMACstring((k), (m), SHA224, 64, 24))
#endif

#endif /* HMAC_H_ */
