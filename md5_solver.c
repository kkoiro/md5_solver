/*
 * Include files
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

/*
 * Constants definition
 */
#define FIRST_PADDING 0x80
#define PADDING 0x00
#define LOW_MD5 64

/*
 * Initial values of hash seed
 */
#define A 0x67452301
#define B 0xEFCDAB89
#define C 0x98BADCFE
#define D 0x10325476


/*
 * Making hash functions
 */
void func_f(unsigned long *a, unsigned long *b, unsigned long *c, unsigned long *d, unsigned long *x, int s, unsigned long *t){
  *a = (*a + ((*b & *c) | (~*b & *d)) + *x + *t) & 0xFFFFFFFF;
  *a = ((*a << s) | (*a >> (32-s))) & 0xFFFFFFFF;
  *a = (*a + *b) & 0xFFFFFFFF;
}
void func_g(unsigned long *a, unsigned long *b, unsigned long *c, unsigned long *d, unsigned long *x, int s, unsigned long *t){
  *a = (*a + ((*b & *d) | (*c & ~*d)) + *x + *t) & 0xFFFFFFFF;
  *a = ((*a << s) | (*a >> (32-s))) & 0xFFFFFFFF;
  *a = (*a + *b) & 0xFFFFFFFF;
}
void func_h(unsigned long *a, unsigned long *b, unsigned long *c, unsigned long *d, unsigned long *x, int s, unsigned long *t){
  *a = (*a + (*b ^ *c ^ *d) + *x + *t) & 0xFFFFFFFF;
  *a = ((*a << s) | (*a >> (32-s))) & 0xFFFFFFFF;
  *a = (*a + *b) & 0xFFFFFFFF;
}
void func_i(unsigned long *a, unsigned long *b, unsigned long *c, unsigned long *d, unsigned long *x, int s, unsigned long *t){
  *a = (*a + (*c ^ (*b | ~*d)) + *x + *t) & 0xFFFFFFFF;
  *a = ((*a << s) | (*a >> (32-s))) & 0xFFFFFFFF;
  *a = (*a + *b) & 0xFFFFFFFF;
}


/*
 * Note:
 * - The expected length of input text is less than 56(len < 56)
 */
int main(int argc, char **argv){

  int i; // for loop
  int j; // for loop

  /*
   * Check argments count
   */
  if(argc != 2){
    fprintf(stderr, "Usage: %s [text]\n", argv[0]);
    exit(-1);
  }


  /*
   * Prepare the table used for hash process
   */
  unsigned long t[64];
  for(i = 0; i < 64; i++){
    t[i] = pow(2, 32) * fabs(sin(i + 1));
  }


  /*
   * Make the target string from the inputted string
   */
  char *text = (char *)malloc(LOW_MD5);
  if(!text){
    perror("malloc: ");
    exit(1);
  }
  strcpy(text, argv[1]);

  int str_byte = strlen(argv[1]);
  int except_parity = LOW_MD5 - 8; // the bit length of input string is put into the last 8 bytes
  text[str_byte] = FIRST_PADDING;
  for(i = str_byte + 1; i < except_parity; i++){
    text[i] = PADDING;
  }

  unsigned long long str_bit = (unsigned long long)str_byte * 8;
  for(i = 0; i < 8; i++){
    text[except_parity + i] = (char)(str_bit >> i * 8) & 0xFF;
  }


  /*
   * Devide the target into 16 for hash process
   */
  unsigned long x[16];
  for(i = 0; i < 16; i++){
    for(j = 0; j < 4; j++){
      x[i] ^= ((unsigned long)text[4 * i + j] & 0xFF) << j * 8;
    }
  }


  /*
   * Make the target be hash with MD5
   */
  unsigned long a = A, b = B, c = C, d = D;

  //FF(a, b, c, d, x[ 0],  7, t[ 0]);
  //FF(d, a, b, c, x[ 1], 12, t[ 1]);
  //FF(c, d, a, b, x[ 2], 17, t[ 2]);
  //FF(b, c, d, a, x[ 3], 22, t[ 3]);
  //FF(a, b, c, d, x[ 4],  7, t[ 4]);
  //FF(d, a, b, c, x[ 5], 12, t[ 5]);
  //FF(c, d, a, b, x[ 6], 17, t[ 6]);
  //FF(b, c, d, a, x[ 7], 22, t[ 7]);
  //FF(a, b, c, d, x[ 8],  7, t[ 8]);
  //FF(d, a, b, c, x[ 9], 12, t[ 9]);
  //FF(c, d, a, b, x[10], 17, t[10]);
  //FF(b, c, d, a, x[11], 22, t[11]);
  //FF(a, b, c, d, x[12],  7, t[12]);
  //FF(d, a, b, c, x[13], 12, t[13]);
  //FF(c, d, a, b, x[14], 17, t[14]);
  //FF(b, c, d, a, x[15], 22, t[15]);

  func_f(&a, &b, &c, &d, &x[ 0],  7, &t[ 0]);
  func_f(&d, &a, &b, &c, &x[ 1], 12, &t[ 1]);
  func_f(&c, &d, &a, &b, &x[ 2], 17, &t[ 2]);
  func_f(&b, &c, &d, &a, &x[ 3], 22, &t[ 3]);
  func_f(&a, &b, &c, &d, &x[ 4],  7, &t[ 4]);
  func_f(&d, &a, &b, &c, &x[ 5], 12, &t[ 5]);
  func_f(&c, &d, &a, &b, &x[ 6], 17, &t[ 6]);
  func_f(&b, &c, &d, &a, &x[ 7], 22, &t[ 7]);
  func_f(&a, &b, &c, &d, &x[ 8],  7, &t[ 8]);
  func_f(&d, &a, &b, &c, &x[ 9], 12, &t[ 9]);
  func_f(&c, &d, &a, &b, &x[10], 17, &t[10]);
  func_f(&b, &c, &d, &a, &x[11], 22, &t[11]);
  func_f(&a, &b, &c, &d, &x[12],  7, &t[12]);
  func_f(&d, &a, &b, &c, &x[13], 12, &t[13]);
  func_f(&c, &d, &a, &b, &x[14], 17, &t[14]);
  func_f(&b, &c, &d, &a, &x[15], 22, &t[15]);

  func_g(&a, &b, &c, &d, &x[ 1],  5, &t[16]);
  func_g(&d, &a, &b, &c, &x[ 6],  9, &t[17]);
  func_g(&c, &d, &a, &b, &x[11], 14, &t[18]);
  func_g(&b, &c, &d, &a, &x[ 0], 20, &t[19]);
  func_g(&a, &b, &c, &d, &x[ 5],  5, &t[20]);
  func_g(&d, &a, &b, &c, &x[10],  9, &t[21]);
  func_g(&c, &d, &a, &b, &x[15], 14, &t[22]);
  func_g(&b, &c, &d, &a, &x[ 4], 20, &t[23]);
  func_g(&a, &b, &c, &d, &x[ 9],  5, &t[24]);
  func_g(&d, &a, &b, &c, &x[14],  9, &t[25]);
  func_g(&c, &d, &a, &b, &x[ 3], 14, &t[26]);
  func_g(&b, &c, &d, &a, &x[ 8], 20, &t[27]);
  func_g(&a, &b, &c, &d, &x[13],  5, &t[28]);
  func_g(&d, &a, &b, &c, &x[ 2],  9, &t[29]);
  func_g(&c, &d, &a, &b, &x[ 7], 14, &t[30]);
  func_g(&b, &c, &d, &a, &x[12], 20, &t[31]);

  func_h(&a, &b, &c, &d, &x[ 5],  4, &t[32]);
  func_h(&d, &a, &b, &c, &x[ 8], 11, &t[33]);
  func_h(&c, &d, &a, &b, &x[11], 16, &t[34]);
  func_h(&b, &c, &d, &a, &x[14], 23, &t[35]);
  func_h(&a, &b, &c, &d, &x[ 1],  4, &t[36]);
  func_h(&d, &a, &b, &c, &x[ 4], 11, &t[37]);
  func_h(&c, &d, &a, &b, &x[ 7], 16, &t[38]);
  func_h(&b, &c, &d, &a, &x[10], 23, &t[39]);
  func_h(&a, &b, &c, &d, &x[13],  4, &t[40]);
  func_h(&d, &a, &b, &c, &x[ 0], 11, &t[41]);
  func_h(&c, &d, &a, &b, &x[ 3], 16, &t[42]);
  func_h(&b, &c, &d, &a, &x[ 6], 23, &t[43]);
  func_h(&a, &b, &c, &d, &x[ 9],  4, &t[44]);
  func_h(&d, &a, &b, &c, &x[12], 11, &t[45]);
  func_h(&c, &d, &a, &b, &x[15], 16, &t[46]);
  func_h(&b, &c, &d, &a, &x[ 2], 23, &t[47]);

  func_i(&a, &b, &c, &d, &x[ 0],  6, &t[48]);
  func_i(&d, &a, &b, &c, &x[ 7], 10, &t[49]);
  func_i(&c, &d, &a, &b, &x[14], 15, &t[50]);
  func_i(&b, &c, &d, &a, &x[ 5], 21, &t[51]);
  func_i(&a, &b, &c, &d, &x[12],  6, &t[52]);
  func_i(&d, &a, &b, &c, &x[ 3], 10, &t[53]);
  func_i(&c, &d, &a, &b, &x[10], 15, &t[54]);
  func_i(&b, &c, &d, &a, &x[ 1], 21, &t[55]);
  func_i(&a, &b, &c, &d, &x[ 8],  6, &t[56]);
  func_i(&d, &a, &b, &c, &x[15], 10, &t[57]);
  func_i(&c, &d, &a, &b, &x[ 6], 15, &t[58]);
  func_i(&b, &c, &d, &a, &x[13], 21, &t[59]);
  func_i(&a, &b, &c, &d, &x[ 4],  6, &t[60]);
  func_i(&d, &a, &b, &c, &x[11], 10, &t[61]);
  func_i(&c, &d, &a, &b, &x[ 2], 15, &t[62]);
  func_i(&b, &c, &d, &a, &x[ 9], 21, &t[63]);

  a = (a + A) & 0xFFFFFFFF;
  b = (b + B) & 0xFFFFFFFF;
  c = (c + C) & 0xFFFFFFFF;
  d = (d + D) & 0xFFFFFFFF;


  /*
   * Make the result hash from hash seeds
   */
  char *hash = (char *)malloc(16);
  if(!hash){
    perror("malloc: ");
    exit(1);
  }

  unsigned long obj;
  for(i = 0; i < 4; i++){
    switch(i){
      case 0:
        obj = a;
        break;
      case 1:
        obj = b;
        break;
      case 2:
        obj = c;
        break;
      case 3:
        obj = d;
        break;
    }
    for(j = 0; j < 4; j++){
      hash[i * 4 + j] = (char)(obj >> j * 8) & 0xFF;
    }
  }

  for(i = 0; i < 16; i++){
    printf("%02hhx", hash[i]);
  }
  printf("\n");

  return 0;
}
