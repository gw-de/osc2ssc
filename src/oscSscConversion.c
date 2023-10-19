#include "../include/oscSscConversion.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

char *getSSCMessageFromreceivedOscMessage(char *receivedOscMessage) {

    // Initiate pairing requests
    if (isStringEqual(receivedOscMessage, "/rx1/pair/enable")) {
        return "{\"rx1\":{\"pair\":{\"enable\":true}}}";
    }
    if (isStringEqual(receivedOscMessage, "/rx2/pair/enable")) {
        return "{\"rx2\":{\"pair\":{\"enable\":true}}}";
    }
    if (isStringEqual(receivedOscMessage, "/rx3/pair/enable")) {
        return "{\"rx3\":{\"pair\":{\"enable\":true}}}";
    }

    // Get state requests
    if (isStringEqual(receivedOscMessage, "/rx1/state")) {
        return "{\"rx1\":{\"state\":null}}";
    }
    if (isStringEqual(receivedOscMessage, "/rx2/state")) {
        return "{\"rx2\":{\"state\":null}}";
    }
    if (isStringEqual(receivedOscMessage, "/rx3/state")) {
        return "{\"rx3\":{\"state\":null}}";
    }

    // Message not known
    printf("Message not found!!!");
    return "";
}

void getreceivedOscMessageFromSSCMessage(char *sscMessage, StringWithLength *receivedOscMessage) {

    // Get state responses - RX1
    if (isStringEqual(sscMessage, "{\"rx1\":{\"state\":\"NOT_CONNECTED\"}}")) {
        char *str = "/rx1/state\0\0,s\0\0NOT_CONNECTED\0\0"; // C inserts an additional \0 at the end
        receivedOscMessage->length = 32;
        receivedOscMessage->pStart = malloc(receivedOscMessage->length);
        memcpy(receivedOscMessage->pStart, str, receivedOscMessage->length);

        return;
    }
    if (isStringEqual(sscMessage, "{\"rx1\":{\"state\":\"CONNECTED\"}}")) {
        char *str = "/rx1/state\0\0,s\0\0CONNECTED\0\0"; // C inserts an additional \0 at the end
        receivedOscMessage->length = 28;
        receivedOscMessage->pStart = malloc(receivedOscMessage->length);
        memcpy(receivedOscMessage->pStart, str, receivedOscMessage->length);

        return;
    }
    if (isStringEqual(sscMessage, "{\"rx1\":{\"state\":\"PAIRING\"}}")) {
        char *str = "/rx1/state\0\0,s\0\0PAIRING"; // C inserts an additional \0 at the end
        receivedOscMessage->length = 24;
        receivedOscMessage->pStart = malloc(receivedOscMessage->length);
        memcpy(receivedOscMessage->pStart, str, receivedOscMessage->length);

        return;
    }


    // Get state responses - RX2
    if (isStringEqual(sscMessage, "{\"rx2\":{\"state\":\"NOT_CONNECTED\"}}")) {
        char *str = "/rx2/state\0\0,s\0\0NOT_CONNECTED\0\0"; // C inserts an additional \0 at the end
        receivedOscMessage->length = 32;
        receivedOscMessage->pStart = malloc(receivedOscMessage->length);
        memcpy(receivedOscMessage->pStart, str, receivedOscMessage->length);

        return;
    }
    if (isStringEqual(sscMessage, "{\"rx2\":{\"state\":\"CONNECTED\"}}")) {
        char *str = "/rx2/state\0\0,s\0\0CONNECTED\0\0"; // C inserts an additional \0 at the end
        receivedOscMessage->length = 28;
        receivedOscMessage->pStart = malloc(receivedOscMessage->length);
        memcpy(receivedOscMessage->pStart, str, receivedOscMessage->length);

        return;
    }
    if (isStringEqual(sscMessage, "{\"rx2\":{\"state\":\"PAIRING\"}}")) {
        char *str = "/rx2/state\0\0,s\0\0PAIRING"; // C inserts an additional \0 at the end
        receivedOscMessage->length = 24;
        receivedOscMessage->pStart = malloc(receivedOscMessage->length);
        memcpy(receivedOscMessage->pStart, str, receivedOscMessage->length);

        return;
    }

    // Get state responses - RX3
    if (isStringEqual(sscMessage, "{\"rx3\":{\"state\":\"NOT_CONNECTED\"}}")) {
        char *str = "/rx3/state\0\0,s\0\0NOT_CONNECTED\0\0"; // C inserts an additional \0 at the end
        receivedOscMessage->length = 32;
        receivedOscMessage->pStart = malloc(receivedOscMessage->length);
        memcpy(receivedOscMessage->pStart, str, receivedOscMessage->length);

        return;
    }
    if (isStringEqual(sscMessage, "{\"rx3\":{\"state\":\"CONNECTED\"}}")) {
        char *str = "/rx3/state\0\0,s\0\0CONNECTED\0\0"; // C inserts an additional \0 at the end
        receivedOscMessage->length = 28;
        receivedOscMessage->pStart = malloc(receivedOscMessage->length);
        memcpy(receivedOscMessage->pStart, str, receivedOscMessage->length);

        return;
    }
    if (isStringEqual(sscMessage, "{\"rx3\":{\"state\":\"PAIRING\"}}")) {
        char *str = "/rx3/state\0\0,s\0\0PAIRING"; // C inserts an additional \0 at the end
        receivedOscMessage->length = 24;
        receivedOscMessage->pStart = malloc(receivedOscMessage->length);
        memcpy(receivedOscMessage->pStart, str, receivedOscMessage->length);

        return;
    }

    printf("WARNING: Message not known\n");
    receivedOscMessage->pStart = NULL;
    receivedOscMessage->length = 0;
}
