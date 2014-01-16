#ifndef MATASANO_COMMON_H_
#define MATASANO_COMMON_H_

#ifdef DEBUG
#  define print_d(f, ...)  printf("[debug] " f, __VA_ARGS__)
#else
#  define print_d(f, ...)  /* NO-OP */
#endif

#define STR_EQ(s1,s2) (strcmp(s1, s2) == 0)

void StringToHex(const char * const, uint8_t * const);

void PrintHex(const uint8_t * const, const uint64_t);
void PrintHexWithSpace(const uint8_t * const, const uint64_t);

#endif /* MATASANO_COMMON_H_ */
