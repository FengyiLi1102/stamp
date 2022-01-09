#include <iostream>
#include <openssl/sha.h>
#include <cstdio>
#include <cstring>
#include <cassert>
#include <fstream>
#include "stamp.h"

const int MAX_TEXT_LENGTH = 2000;


using namespace std;

/* You are pre-supplied with the functions below. Add your own 
   function definitions to the end of this file. */

// helper function for internal use only
// transforms raw binary hash value into human-friendly hexademical form
void convert_hash(const unsigned char *str, char *output, int hash_length) {
  char append[16];
  strcpy (output, "");
  for (int n=0; n<hash_length; n++) {
    sprintf(append,"%02x",str[n]);
    strcat(output, append);
  }
}

// pre-supplied helper function
// generates the SHA1 hash of input string text into output parameter digest
// ********************** IMPORTANT **************************
// ---> remember to include -lcrypto in your linking step <---
// ---> so that the definition of the function SHA1 is    <---
// ---> included in your program                          <---
// ***********************************************************
void text_to_SHA1_digest(const char *text, char *digest) {
  unsigned char hash[SHA_DIGEST_LENGTH];
  SHA1( (const unsigned char *) text, strlen(text), hash);
  convert_hash(hash, digest, SHA_DIGEST_LENGTH);
}

/* add your function definitions here */
int leading_zeros(const char *digest) {
    int counter = 0;

    for(auto chIndex = 0; chIndex < strlen(digest); ++chIndex) {
        if (digest[chIndex] == '0') ++counter;
        else return counter;
    }
}


bool file_to_SHA1_digest(char *filename, char *digest) {
    char text[MAX_TEXT_LENGTH];
    int length = 0;

    ifstream in_stream(filename);
    if (in_stream.fail()) {
        digest = "error";
        in_stream.close();
        return false;
    }

    while (!in_stream.eof() && length < MAX_TEXT_LENGTH) {
        in_stream.get(text[length]);
        length++;
    }

    text_to_SHA1_digest(text, digest);

    in_stream.close();
    return true;
}


bool make_header(const char *recipient, const char *filename, char *header) {
    char digest[SHA_DIGEST_LENGTH], digestCopy[SHA_DIGEST_LENGTH];

    if (!file_to_SHA1_digest(filename, digest)) return false;

    strncpy(digestCopy, digest, SHA_DIGEST_LENGTH);


}