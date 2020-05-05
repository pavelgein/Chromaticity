#include "optparser.h"

void FromString(const char * option, int& k) {
    k = std::atoi(option);
}

void FromString(const char * option, unsigned int& k) {
    k = std::atol(option);
}

void FromString(const char * option, std::string& s) {
   s = std::string(option);
}

void FromString(const char * option, unsigned long& l) {
    l = std::atoll(option);
}

void FromString(const char * option, bool& target) {
    target = std::strcmp(option, "true") == 0;
}