#ifndef rabinKarp
#define rabinKarp
#include <string.h>
#include <iostream>

using namespace std;


//int rabinKarpFingerPrint(std::string, int);

int rabinKarpFingerPrint(std::string text, int prime, int prevHash, int currentLength);
int addHash(int, char, int, int);
#endif
