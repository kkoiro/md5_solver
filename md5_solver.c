#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>


#define FIRST_PADDING 0b10000000
#define PADDING 0b00000000
#define LOW_MD5 64

#define A 0x67452301
#define B 0xEFCDAB89
#define C 0x98BADCFE
#define D 0x10325476

#define F(x, y, z) (((x) & (y)) | ((~x) & (z)))
#define G(x, y, z) (((x) & (z)) | ((y) & (~z)))
#define H(x, y, z) ((x) ^ (y) ^ (z))
#define I(x, y, z) ((y) ^ ((x) | (~z)))

#define ROTATE_LEFT(x, n) (((x) << (n)) | ((x) >> (32-(n))))

#define FF(a, b, c, d, x, s, ac){ \
  (a) += F((b), (c), (d)) + (x) + (unsigned long)(ac); \
  (a) = ROTATE_LEFT((a), (s)); \
  (a) += (b); \
}
#define GG(a, b, c, d, x, s, ac){ \
  (a) += G((b), (c), (d)) + (x) + (unsigned long)(ac); \
  (a) = ROTATE_LEFT((a), (s)); \
  (a) += (b); \
}
#define HH(a, b, c, d, x, s, ac){ \
  (a) += H((b), (c), (d)) + (x) + (unsigned long)(ac); \
  (a) = ROTATE_LEFT((a), (s)); \
  (a) += (b); \
}
#define II(a, b, c, d, x, s, ac){ \
  (a) += I((b), (c), (d)) + (x) + (unsigned long)(ac); \
  (a) = ROTATE_LEFT((a), (s)); \
  (a) += (b); \
}


// 入力は55文字以下と想定
int main(int argc, char **argv){

  unsigned long table[64];
  int l;
  for(l = 0; l < 64; l++){
    table[l] = 4294967296 * fabs(sin(l)); // 4294967296 = 2^32
  }

  char *input = (char *)malloc(LOW_MD5);
  if(!input){
    perror("malloc: ");
    exit(1);
  }
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

  unsigned long x[16];
  int m;
  int n;
  unsigned long tmp;
  for(m = 0; m < 16; m++){
    for(n = 0; n < 4; n++){
      x[m] ^= ((unsigned long)input[4 * m + n] & 0xFF) << (3 - n) * 8;
    }
  }

  unsigned long a = A, b = B, c = C, d = D;

  FF(a, b, c, d, input[ 0],  7, table[ 0]);
  FF(d, a, b, c, input[ 1], 12, table[ 1]);
  FF(c, d, a, b, input[ 2], 17, table[ 2]);
  FF(b, c, d, a, input[ 3], 22, table[ 3]);
  FF(a, b, c, d, input[ 4],  7, table[ 4]);
  FF(d, a, b, c, input[ 5], 12, table[ 5]);
  FF(c, d, a, b, input[ 6], 17, table[ 6]);
  FF(b, c, d, a, input[ 7], 22, table[ 7]);
  FF(a, b, c, d, input[ 8],  7, table[ 8]);
  FF(d, a, b, c, input[ 9], 12, table[ 9]);
  FF(c, d, a, b, input[10], 17, table[10]);
  FF(b, c, d, a, input[11], 22, table[11]);
  FF(a, b, c, d, input[12],  7, table[12]);
  FF(d, a, b, c, input[13], 12, table[13]);
  FF(c, d, a, b, input[14], 17, table[14]);
  FF(b, c, d, a, input[15], 22, table[15]);

  GG(a, b, c, d, input[ 1],  5, table[16]);
  GG(d, a, b, c, input[ 6],  9, table[17]);
  GG(c, d, a, b, input[11], 14, table[18]);
  GG(b, c, d, a, input[ 0], 20, table[19]);
  GG(a, b, c, d, input[ 5],  5, table[20]);
  GG(d, a, b, c, input[10],  9, table[21]);
  GG(c, d, a, b, input[15], 14, table[22]);
  GG(b, c, d, a, input[ 4], 20, table[23]);
  GG(a, b, c, d, input[ 9],  5, table[24]);
  GG(d, a, b, c, input[14],  9, table[25]);
  GG(c, d, a, b, input[ 3], 14, table[26]);
  GG(b, c, d, a, input[ 8], 20, table[27]);
  GG(a, b, c, d, input[13],  5, table[28]);
  GG(d, a, b, c, input[ 2],  9, table[29]);
  GG(c, d, a, b, input[ 7], 14, table[30]);
  GG(b, c, d, a, input[12], 20, table[31]);

  HH(a, b, c, d, input[ 5],  4, table[32]);
  HH(d, a, b, c, input[ 8], 11, table[33]);
  HH(c, d, a, b, input[11], 16, table[34]);
  HH(b, c, d, a, input[14], 23, table[35]);
  HH(a, b, c, d, input[ 1],  4, table[36]);
  HH(d, a, b, c, input[ 4], 11, table[37]);
  HH(c, d, a, b, input[ 7], 16, table[38]);
  HH(b, c, d, a, input[10], 23, table[39]);
  HH(a, b, c, d, input[13],  4, table[40]);
  HH(d, a, b, c, input[ 0], 11, table[41]);
  HH(c, d, a, b, input[ 3], 16, table[42]);
  HH(b, c, d, a, input[ 6], 23, table[43]);
  HH(a, b, c, d, input[ 9],  4, table[44]);
  HH(d, a, b, c, input[12], 11, table[45]);
  HH(c, d, a, b, input[15], 16, table[46]);
  HH(b, c, d, a, input[ 2], 23, table[47]);

  II(a, b, c, d, input[ 0],  6, table[48]);
  II(d, a, b, c, input[ 7], 10, table[49]);
  II(c, d, a, b, input[14], 15, table[50]);
  II(b, c, d, a, input[ 5], 21, table[51]);
  II(a, b, c, d, input[12],  6, table[52]);
  II(d, a, b, c, input[ 3], 10, table[53]);
  II(c, d, a, b, input[10], 15, table[54]);
  II(b, c, d, a, input[ 1], 21, table[55]);
  II(a, b, c, d, input[ 8],  6, table[56]);
  II(d, a, b, c, input[15], 10, table[57]);
  II(c, d, a, b, input[ 6], 15, table[58]);
  II(b, c, d, a, input[13], 21, table[59]);
  II(a, b, c, d, input[ 4],  6, table[60]);
  II(d, a, b, c, input[11], 10, table[61]);
  II(c, d, a, b, input[ 2], 15, table[62]);
  II(b, c, d, a, input[ 9], 21, table[63]);

  a = a + A;
  b = b + B;
  c = c + C;
  d = d + D;

  char *output = (char *)malloc(16);
  if(!output){
    perror("malloc: ");
    exit(1);
  }
  int p;
  int q;
  unsigned long target;
  for(p = 0; p < 4; p++){
    switch(p){
      case 0:
        target = a;
        break;
      case 1:
        target = b;
        break;
      case 2:
        target = c;
        break;
      case 3:
        target = d;
        break;
    }
    for(q = 0; q < 4; q++){
      output[p * 4 + q] = (char)(target >> q * 8) & 0xFF;
    }
  }

  int k;
  for(k = 0; k < 16; k++){
    printf("%02hhx\n", output[k]);
  }
  printf("\n");

  return 0;
}
