#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_MESSAGE_SIZE 100000
#define FIXED_WIDTH 8
#define KEY_WIDTH 10

bool isValidPrime(long long num, long long limit) {
  long long root = (long long)sqrtl((long double)num) + 1;
  if (num > limit || num <= 1) {
    return false;
  }

  for (long long i = 2; i < root; i++) {
    if (num % i == 0) {
      return false;
    }
  }
  return true;
}

long long modInverse(long long e, long long phi) {
  long long t = 0, newT = 1;
  long long r = phi, newR = e;
  while (newR != 0) {
    long long quotient = r / newR;
    t -= quotient * newT;
    r -= quotient * newR;
    long long tempT = t;
    t = newT;
    newT = tempT;
    long long tempR = r;
    r = newR;
    newR = tempR;
  }
  if (r > 1)
    return -1;
  if (t < 0)
    t += phi;
  return t;
}

long long modXP(long long base, long long exponent, long long mod) {
  long long result = 1;
  base = base % mod;

  while (exponent > 0) {
    if (exponent % 2 == 1) {
      result = (result * base) % mod;
    }

    base = ((base % mod) * (base % mod)) % mod;
    exponent /= 2;
  }
  return result;
}

long long gcd(long long num1, long long num2) {
  while (num2 != 0) {
    long long temp = num2;
    num2 = num1 % num2;
    num1 = temp;
  }
  return num1;
}

long long findValidE(long long n, long long phi) {

  if (gcd(3, phi) == 1 && gcd(3, n) == 1)
    return 3;
  if (gcd(65537, phi) == 1 && gcd(65537, n) == 1)
    return 65537;

  for (long long i = 2; i < phi; i++) {
    if (gcd(i, n) == 1 && gcd(i, phi) == 1) {
      return i;
    }
  }
  return 0;
}

void createKeys(long long p, long long q) {
  long long n = p * q;
  long long phi = (p - 1) * (q - 1);

  long long e = findValidE(n, phi);
  if (e == 0) {
    printf("No valid e values\n");
    return;
  }

  long long d = modInverse(e, phi);
  if (d == -1) {
    printf("No valid values for d\n");
    return;
  }

  printf("The encryption key is: %010llx%010llx\n", e, n);

  printf("The decryption key is: %010llx%010llx\n\n", d, n);
}

int main(int argc, char **argv) {
  char mode;
  printf("type e to encrypt, d to decrypt, c to create key, and g to generate "
         "random key: ");
  scanf(" %c", &mode);
  getchar();

  if (mode == 'g') {
    srand(time(NULL));
    int lowerLimit = 10000;
    int upperLimit = 50000;
    long long p = lowerLimit + rand() % (upperLimit - lowerLimit);
    long long q = lowerLimit + rand() % (upperLimit - lowerLimit);

    while (!isValidPrime(p, upperLimit)) {
      p = lowerLimit + rand() % (upperLimit - lowerLimit);
    }
    while (!isValidPrime(q, upperLimit)) {
      q = lowerLimit + rand() % (upperLimit - lowerLimit);
    }

    createKeys(p, q);
  }

  if (mode == 'c') {
    long long p;
    long long q;

    bool checkPrimes = true;

    printf("First prime number under 50000: ");
    scanf("%lld", &p);

    if (checkPrimes && !isValidPrime(p, 50000)) {
      printf("Not a valid number\n");
      return 0;
    }

    printf("Second prime number under 50000: ");
    scanf("%lld", &q);

    if (checkPrimes && !isValidPrime(q, 50000)) {
      printf("Not a valid number\n");
      return 0;
    }

    createKeys(p, q);
  }
  if (mode == 'e') {
    long long key1, key2;
    char key[KEY_WIDTH * 2 + 1];
    char filename[256];

    printf("enter the encryption key: ");
    scanf("%20s", key);

    char firstHalf[KEY_WIDTH + 1];
    char secondHalf[KEY_WIDTH + 1];

    snprintf(firstHalf, KEY_WIDTH + 1, "%.10s", key);
    snprintf(secondHalf, KEY_WIDTH + 1, "%.10s", key + 10);

    key1 = strtoull(firstHalf, NULL, 16);
    key2 = strtoull(secondHalf, NULL, 16);

    getchar();

    printf("enter input file name: ");
    scanf("%255s", filename);
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
      perror("Error opening file");
      return 1;
    }

    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    char content[(fileSize + 1)];
    char ciphertext[(fileSize + 1) * FIXED_WIDTH];
    for (int i = 0; i < strlen(ciphertext); i++)
      ciphertext[i] = 0;

    fread(content, 1, fileSize, file);

    fclose(file);

    for (int i = 0; i < strlen(content); i++) {
      long long encrypted_char = modXP((long long)content[i], key1, key2);

      char encrypted_str[FIXED_WIDTH + 1];
      snprintf(encrypted_str, sizeof(encrypted_str), "%0*llx", FIXED_WIDTH,
               encrypted_char);
      strncat(ciphertext, encrypted_str,
              sizeof(ciphertext) - strlen(ciphertext) - 1);
    }

    char *outfile = "rsa.cip";

    FILE *files = fopen("rsa.cip", "w");
    fprintf(files, "%s\n", ciphertext);
    fclose(files);

    printf("Printed output to %s\n", outfile);
  }
  if (mode == 'd') {
    long long key1, key2;
    char key[KEY_WIDTH * 2 + 1];
    char filename[256];

    printf("enter the decryption key: ");
    scanf("%20s", key);

    char firstHalf[KEY_WIDTH + 1];
    char secondHalf[KEY_WIDTH + 1];

    snprintf(firstHalf, KEY_WIDTH + 1, "%.10s", key);
    snprintf(secondHalf, KEY_WIDTH + 1, "%.10s", key + 10);

    key1 = strtoull(firstHalf, NULL, 16);
    key2 = strtoull(secondHalf, NULL, 16);

    getchar();

    printf("enter input file name: ");
    scanf("%255s", filename);
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
      perror("Error opening file");
      return 1;
    }

    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    char content[(fileSize + 1)];
    char ciphertext[(fileSize + 1) * FIXED_WIDTH];
    for (int i = 0; i < strlen(content); i++)
      content[i] = 0;

    fread(ciphertext, 1, fileSize, file);

    fclose(file);

    int blocks = strlen(ciphertext) / FIXED_WIDTH;
    for (int i = 0; i < blocks; i++) {
      char block[FIXED_WIDTH + 1] = {0};
      strncpy(block, &ciphertext[i * FIXED_WIDTH], FIXED_WIDTH);

      long long encrypted_char = strtoll(block, NULL, 16);
      long long decrypted_char = modXP(encrypted_char, key1, key2);
      content[i] = (char)decrypted_char;
    }
    char *outfile = "rsa.out";

    FILE *files = fopen("rsa.out", "w");
    fprintf(files, "%s\n", content);
    fclose(files);

    printf("Printed output to %s\n", outfile);
  }
}
