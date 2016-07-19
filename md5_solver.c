/*
 * Include files
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>


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
 * Global variables
 */
unsigned long t[64]; // Used for hash process
char target_hash[33]; // Search target hash string
int target_len; // Search target length
pthread_mutex_t mutex; // Mutex
int thread_counter = 0; // A number of threads
int word_kind_num;
int all_word_comb; // A number of all word combination
int process_unit; // A number of words processed in each thread
int rest;
volatile int flag = 0;


/*
 * Word list
 */
char dictionary[] = {
  'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z'
};


/*
 * Function declarations
 */
void func_f(unsigned long *a, unsigned long *b, unsigned long *c, unsigned long *d, unsigned long *x, int s, unsigned long *t);
void func_g(unsigned long *a, unsigned long *b, unsigned long *c, unsigned long *d, unsigned long *x, int s, unsigned long *t);
void func_h(unsigned long *a, unsigned long *b, unsigned long *c, unsigned long *d, unsigned long *x, int s, unsigned long *t);
void func_i(unsigned long *a, unsigned long *b, unsigned long *c, unsigned long *d, unsigned long *x, int s, unsigned long *t);

void md5(char *pt, char *output);

void *search(void *param);
void comp_target_update(unsigned int *val, int pos);


/*
 * Main function
 */
int main(int argc, char **argv){

  int i; // for loop
  int j; // for loop

  int thread_num; // A number of threads


  /*
   * Check input
   */
  if(argc != 4 || strlen(argv[1]) != 32){
    fprintf(stderr, "Usage: %s [target hash] [target length] [thread num]\n", argv[0]);
    exit(-1);
  }


  /*
   * Some preparations
   */
  strcpy(target_hash, argv[1]); // set target
  target_len = atoi(argv[2]); // set target length
  thread_num = atoi(argv[3]); // set target length
  word_kind_num = sizeof(dictionary) / sizeof(char);
  all_word_comb = pow(word_kind_num, target_len); // bottle neck
  process_unit = all_word_comb / thread_num;
  rest = all_word_comb % thread_num;


  /*
   * Prepare the table used for hash process
   */
  for(i = 0; i < 64; i++){
    t[i] = pow(2, 32) * fabs(sin(i + 1));
  }


  /*
   * Search with thread
   */
  pthread_mutex_init(&mutex, NULL);

  pthread_t thread[thread_num];
  for(i = 0; i < thread_num; i++){
    if(pthread_create(&thread[i], NULL, search, NULL) != 0){
      perror("pthread_create: ");
    }
  }

  for(i = 0; i < thread_num; i++){
    pthread_join(thread[i], NULL);
  }

  pthread_mutex_destroy(&mutex);


  /*
   * Show result
   */
  if(flag){
    printf("Finish!!\n");
  }else{
    printf("The matching string not found...\n");
  }

  return 0;

}


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
 * Hash function
 * Note:
 *  The expected length of input text is less than 56(len < 56)
 */
void md5(char *pt, char *output){

  int i; // for loop
  int j; // for loop


  /*
   * Add puddings after the inputted string
   */
  char text[LOW_MD5];
  strcpy(text, pt);

  int str_byte = strlen(pt);
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
   * Devide the arranged string 16 for hash process
   */
  unsigned long x[16];
  for(i = 0; i < 16; i++){
    for(j = 0; j < 4; j++){
      if(j == 0){
        x[i] = ((unsigned long)text[4 * i + j] & 0xFF) << j * 8;
      }else{
        x[i] ^= ((unsigned long)text[4 * i + j] & 0xFF) << j * 8;
      }
    }
  }


  /*
   * Make the arranged string be hash with MD5
   */
  unsigned long a = A;
  unsigned long b = B;
  unsigned long c = C;
  unsigned long d = D;

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
   * Make the result hash from processed hash seeds
   */
  char hash[16];

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

  char tmp[3];
  char result[32 + 1];
  for(i = 0; i < 16; i++){
    sprintf(tmp, "%02hhx", hash[i]);
    for(j = 0; j < 2; j++){
      result[i * 2 + j] = tmp[j];
    }
  }
  result[32] = '\0';

  strcpy(output, result);

}


/*
 * Thread function
 */
void *search(void *param){

  int i; // for loop
  int j; // for loop

  int my_num;
  unsigned int start_num;
  unsigned int pos[target_len];
  char comp_target[target_len + 1];
  char comp_target_hash[33];


  /*
   * Get this thread number
   */
  pthread_mutex_lock(&mutex);
  my_num = thread_counter;
  thread_counter++;
  pthread_mutex_unlock(&mutex);


  /*
   * Calculate the first string
   */
  start_num = process_unit * my_num;

  unsigned int pos_low;
  for(i = target_len - 1; i >= 0; i--){
    pos_low = pow(word_kind_num, i); // bottle neck
    pos[i] = start_num / pos_low;
    start_num -= pos[i] * pos_low;
  }


  /*
   * Create comparison target word from dictionary
   *  & Search the matching string
   */
  for(i = 0; i < process_unit + rest; i++){
    if(flag){
      break;
    }

    for(j = 0; j < target_len; j++){
      comp_target[j] = dictionary[pos[j]];
    }
    comp_target[target_len] = '\0';
    //printf("%03d: %s\n", my_num, comp_target);

    md5(comp_target, comp_target_hash); // Make comp_target be hash
    if(strcmp(target_hash, comp_target_hash) == 0){
      flag = 1;
      printf("%s\n", comp_target);
      break;
    }

    comp_target_update(pos, 0);
  }


  return NULL;

}

void comp_target_update(unsigned int *val, int pos){

  val[pos] += 1;
  if(val[pos] == word_kind_num){
    if(pos == target_len - 1){
      return;
    }
    val[pos] = 0;
    pos += 1;
    comp_target_update(val, pos);
  }

}
