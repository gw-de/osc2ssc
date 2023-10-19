#ifndef NEP_SOSE2021_STRINGHELPER_H
#define NEP_SOSE2021_STRINGHELPER_H

#include <stdint.h>

//Useful for storing strings that contain zero-byte terminators in the middle.
typedef struct StringWithLength {
    //Pointer to the start address of the string.
    char* pStart;

    //Length of the string.
    uint8_t length;
} StringWithLength;

/**
 * Compares two strings.
 *
 * @param str1 Zero-byte terminated string to compare with str2.
 * @param str2 Zero-byte terminated string to compare with str1.
 * @return 0 if strings are not equal, otherwise not 0.
 */
uint8_t isStringEqual(char *str1, char *str2);

#endif //NEP_SOSE2021_STRINGHELPER_H
