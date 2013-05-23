#ifndef HMAC_H_
#define HMAC_H_

uint8_t *HMAC(const uint8_t *, uint64_t, const uint8_t *, uint64_t, uint8_t *(*)(const uint8_t *, uint64_t), uint8_t, uint8_t);
uint8_t *HMACstring(const char *, const char *, uint8_t *(*)(const uint8_t *, uint64_t), uint8_t, uint8_t);
/* TODO HMAC file? */

/* TODO put B here... should go elsewhere? */
/* TODO put digest_length here... should go elsewhere? */
/* HMAC_MD6 */

/* HMAC_SHA0 */

#endif /* HMAC_H_ */
