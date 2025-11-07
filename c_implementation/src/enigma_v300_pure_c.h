#ifndef ENIGMA_V300_PURE_C_H
#define ENIGMA_V300_PURE_C_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

void enigma_c_encrypt(const char* input_key, char* output_key, size_t len);
void enigma_c_decrypt(const char* input_key, char* output_key, size_t len);
int enigma_c_check_option_key(int option, const char* key, const char* serial_number);

void enigma2_c_encrypt(const char* input_key, char* output_key);
void enigma2_c_decrypt(const char* input_key, char* output_key);
int enigma2_c_check_option_key(int option, const char* key);

int run_enigma_cli(int argc, char* argv[]);

#ifdef __cplusplus
}
#endif

#endif /* ENIGMA_V300_PURE_C_H */
