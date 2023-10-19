#ifndef NEP_SOSE2021_OSCSSCCONVERSION_H
#define NEP_SOSE2021_OSCSSCCONVERSION_H

#include "stringHelper.h"

/**
 * Converts an OSC message to an SSC message.
 *
 * @param receivedOscMessage Zero-byte terminated string in OSC message format.
 * @return Zero-byte terminated string in SSC message format. Empty string if OSC message is unknown.
 */
char *getSSCMessageFromreceivedOscMessage(char *receivedOscMessage);

/**
 * Converts an SSC message to an OSC message.
 *
 * @param sscMessage Input SSC message, zero-byte terminated string.
 * @param receivedOscMessage Output OCC message.
 */
void getreceivedOscMessageFromSSCMessage(char *sscMessage, StringWithLength *receivedOscMessage);

#endif //NEP_SOSE2021_OSCSSCCONVERSION_H
