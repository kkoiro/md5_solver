#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>


#define FIRST_PADDING 0b10000000
#define PADDING 0b00000000
#define LOW_MD5 64

#define BUF_A 0x67452301
#define BUF_B 0xEFCDAB89
#define BUF_C 0x98BADCFE
#define BUF_D 0x10325476

#define FUNC_F(x, y, z) (((x) & (y)) | ((~x) & (z)))
#define FUNC_G(x, y, z) (((x) & (z)) | ((y) & (~z)))
#define FUNC_H(x, y, z) ((x) ^ (y) ^ (z))
#define FUNC_I(x, y, z) ((y) ^ ((x) | (~z)))


// 入力は55文字以下と想定
int main(int argc, char **argv){

  unsigned long table[64];
  int l;
  for(l = 0; l < 64; l++){
    table[l] = 4294967296 * fabs(sin(l)); // 4294967296 = 2^32
  }

  char *input = (char *)malloc(LOW_MD5);
  strcpy(input, argv[1]); //入力文字列をcopy

  unsigned int str_len_byte = strlen(argv[1]);
  int except_parity = LOW_MD5 - 8; // 入力bit数を入れる8を除いた数
  int i;
  input[str_len_byte] = FIRST_PADDING;
  for(i = str_len_byte + 1; i < except_parity; i++){
    input[i] = PADDING;
  }
  unsigned long long str_len_bit = str_len_byte * 8; // 入力bit数を計算
  int j;
  for(j = 0; j < 8; j++){
    input[except_parity + j] = (char)(str_len_bit >> j * 8) & 0xFF; //入力もbit長を下位byteから付加
  }

  int k;
  for(k = 0; k < 64; k++){
    printf("%x\n", input[k]);
  }

  return 0;
}
