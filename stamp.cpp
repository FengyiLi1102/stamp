#include <iostream>
#include <openssl/sha.h>
#include <cstdio>
#include <cstring>
#include <cassert>
#include <fstream>
#include "stamp.h"

const int MAX_TEXT_LENGTH = 2000;
const int MAX_RECIPIENT_LENGTH = 100;
const int LENGTH_DIGEST = 41;

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


bool file_to_SHA1_digest(const char *filename, char *digest) {
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
    memset(header, 0 ,511);
    char digest[LENGTH_DIGEST], recipientAndDigest[MAX_TEXT_LENGTH];
    long i = 0;

    bool success = file_to_SHA1_digest(filename, digest);
    if (!success) return false;

    strcat(header, recipient);
    int length = strlen(header);
    header[length] = ':';
    header[length + 1] = '\0';
    strcat(header, digest);
    length = strlen(header);
    header[length] = ':';
    header[length+1] = '\0';
    strcpy(recipientAndDigest, header);

    for (; i < 10000000; ++i) {
        char processedHeader[MAX_TEXT_LENGTH];
        strcpy(processedHeader, recipientAndDigest);
        strcat(processedHeader, to_string(i).c_str());
        processedHeader[strlen(processedHeader)] = '\0';
        text_to_SHA1_digest(processedHeader, digest);

        if (leading_zeros(digest) != 5) continue;
        else {
            strcat(header, to_string(i).c_str());
            header[strlen(header)] = '\0';
            return true;
        }
    }

    return false;
}


MessageStatus check_header(const char *email_address, const char *header, const char *filename) {
    int counterHeader = 0;
    long i = 0, j = 0;
    char recipientInHeader[MAX_RECIPIENT_LENGTH], digestInHeader[LENGTH_DIGEST];
    for (; i < strlen(header); ++i) {
        if (header[i] == ':') ++counterHeader;
        if (counterHeader == 0) recipientInHeader[i] = header[i];
        if (counterHeader == 1 && header[i+1] != ':') {
            digestInHeader[j] = header[i+1];
            j++;
        }

    }
    if (counterHeader != 2) return INVALID_HEADER;
    recipientInHeader[strlen(recipientInHeader)] = '\0';
    digestInHeader[LENGTH_DIGEST-1] = '\0';
    if (strcmp(recipientInHeader, email_address) != 0) return WRONG_RECIPIENT;

    char digest[LENGTH_DIGEST];
    if (!file_to_SHA1_digest(filename, digest)) return INVALID_MESSAGE_DIGEST;
    if(strcmp(digestInHeader, digest) != 0) return INVALID_MESSAGE_DIGEST;

    memset(digestInHeader, 0, LENGTH_DIGEST);
    text_to_SHA1_digest(header, digestInHeader);
    if (leading_zeros(digestInHeader) != 5) return INVALID_HEADER_DIGEST;

    return VALID_EMAIL;
}