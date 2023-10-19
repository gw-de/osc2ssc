#include "../include/stringHelper.h"

#include <stdio.h>
#include <string.h>

uint8_t isStringEqual(char *str1, char *str2) {
    uint8_t compareResult = strcmp(str1, str2);
    //printf("String compare result: %d\n", compareResult);

    return !compareResult;
}