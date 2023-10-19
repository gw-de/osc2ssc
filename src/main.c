#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/ip.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "../include/stringHelper.h"
#include "../include/validators.h"
#include "../include/oscSscConversion.h"

//Buffer size for OSC messages.
#define OSC_MESSAGE_MAX_LENGTH 10000

//Buffer size for SSC messages.
#define SSC_MESSAGE_MAX_LENGTH 10000

//Struct definition for config-file
typedef struct config {
    char ip[11][15];
    int port[11];
} config;

//Struct definition for server parameters
typedef struct server_arg {
    int sock;
    int port;
    char *ip_basestation;
} server_arg;

typedef struct RequestHandlerParams {
    //OSC message that was given to the server.
    char oscInputMessage[OSC_MESSAGE_MAX_LENGTH];

    //Information about the client IP address. Stage Control
    struct sockaddr_in clientAddr;

    //Socket that should be used to send a response back to the client.
    int* clientSocket;

    //IP Adress for Sennheiser Base Station
    char ip_basestation[15];
} RequestHandlerParams;

/**
 * Called in a separate thread to handle a request sent by a client.
 *
 * @param paramsP Parameters for the thread
 * @return TODO
 */
void *requestHandler(void *paramsP) {
    printf("Client started\n");

    RequestHandlerParams *params = (RequestHandlerParams *) paramsP;

    // Socket for sending and receiving SSC messages to the Sennheiser device.
    int sscClientSocket = socket(AF_INET, SOCK_DGRAM, 0);

    struct sockaddr_in BaseStationAddr;
    BaseStationAddr.sin_family = AF_INET;
    BaseStationAddr.sin_port = htons(45);
    printf("IP Sennheiser: %s\n", params->ip_basestation);
    inet_aton(params->ip_basestation, &BaseStationAddr.sin_addr);
    
    //Convert OSC to SSC message
    printf("Converting OSC message...: %s\n", params->oscInputMessage);
    char *sscMessage = getSSCMessageFromreceivedOscMessage(params->oscInputMessage);

    printf("SSC message: %s\n", sscMessage);

    if (!isStringEqual(sscMessage, "")) {
        
        //Send message to Sennheiser device
        if (sendto(sscClientSocket, sscMessage, strlen(sscMessage) + 1, 0, (struct sockaddr *) &BaseStationAddr,
                   sizeof(BaseStationAddr)) > 0) {
            printf("SSC message sent to Sennheiser device\n");
            
            //Get response from Sennheiser device
            char receivedSscPayload[SSC_MESSAGE_MAX_LENGTH];
            memset(receivedSscPayload, 0, SSC_MESSAGE_MAX_LENGTH);
            socklen_t len = sizeof(BaseStationAddr);

            recvfrom(sscClientSocket, (char *) receivedSscPayload, sizeof(receivedSscPayload), MSG_WAITALL,
                     (struct sockaddr *) &BaseStationAddr, &len);

            printf("Received from Sennheiser device: %s !\n", receivedSscPayload);
            
            //Convert SSC to OSC message
            StringWithLength receivedOscMessage = {NULL, 0};
            getreceivedOscMessageFromSSCMessage(receivedSscPayload, &receivedOscMessage);

            if (receivedOscMessage.pStart != NULL) {
                printf("Success, OSC message was FOUND\n");
                printf("> Output OSC message: length=%d, contentUnilFirstZeroByteChar=%s\n", receivedOscMessage.length,
                       receivedOscMessage.pStart);
                
                //Send OSC message back to client
                printf("Sending back to IP: %s:\n", inet_ntoa(params->clientAddr.sin_addr));
                if (sendto(*params->clientSocket, receivedOscMessage.pStart, receivedOscMessage.length, MSG_CONFIRM,
                           (struct sockaddr *) &params->clientAddr,
                           sizeof(params->clientAddr)) <= 0) {

                    printf("Error sending back to StageControl\n");
                } else {
                    printf("Message sent to StageControl\n");
                }
            } else {
                printf("Error, OSC message is UNKNOWN\n");
            }
        } else {
            printf("Error sending back to Sennheiser receiver\n");
        }
    } else {
        printf("Error, SSC message is UNKNOWN\n");
    }
    printf("RequestHandler finished\n\n");
    pthread_exit(NULL);
}

_Bool loadFile(config *destination) {
    //Define variables
    char const *const fileName = "config.ini";
    char line[256] = "";
    char *ipStart = "";
    char *ipEnd = "";
    char portAdress[5] = "";
    int i = 0;
    int j = 0;

    //Open file
    FILE *file = fopen(fileName, "r");
    if (file == NULL) {
        return 0;
    }

    //Read file line by line (ip and port in alternation)
    while (fgets(line, sizeof(line), file)) {
        ipStart = strchr(line, (int) ':') + 1;

        if (ipStart == NULL) {
            printf("Pointerstart nicht gefunden bei %s \n", line);
            return 0;
        }

        ipEnd = strchr(ipStart, (int) '\n') ;

        if (ipEnd == NULL) {
            printf("Pointerende nicht gefunden bei %s \n", line);
            return 0;
        }

        if (i % 2 == 0) {
            if (ipEnd - ipStart > sizeof(destination->ip[j])) {
                printf("Falsche Gr��e bei %s \n", line);
                return 0;
            }

            memcpy(destination->ip[j], ipStart, ipEnd - ipStart);

            if (!isValidIpAddress(destination->ip[j])) {
                printf("Keine g�ltige IP bei %s \n In Zeile %d \n", line, j);
                return 0;
            }
        } else {
            if (ipEnd - ipStart > sizeof(portAdress)) {
                printf("Falsche Gr��e bei %s %lu \n", line, sizeof(portAdress));
                return 0;
            }
            memcpy(portAdress, ipStart, ipEnd - ipStart);
            destination->port[j] = strtol(portAdress, &portAdress + (char) 5, 10);

            //Increment counter j
            j++;
        }
        //Increment counter i
        i++;
    }

    fclose(file);
    return 1;
}

//Function for server threads
void *server(void *server_connect) {
    //Retrieve arguments
    server_arg *arguments = (server_arg *) server_connect;
    int sock = arguments->sock;
    int port = arguments->port;
    char *ip_basestation= arguments->ip_basestation;

    // Adress-Struktur fuer lokalen Server
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddr.sin_port = htons(port);

    // Socket an Adresse binden
    bind(sock, (struct sockaddr *) &serverAddr, sizeof(serverAddr));

     //Print arguments
    printf("\nServer Socket Nr. %d startet und hört auf Port: %d\n", sock, port);

    // Prepare handling client requests
    RequestHandlerParams requestHandlerParams;
    requestHandlerParams.clientSocket = &sock;
    memset(requestHandlerParams.oscInputMessage, 0, OSC_MESSAGE_MAX_LENGTH);
    memset(requestHandlerParams.ip_basestation, 0, 15);
    memcpy(requestHandlerParams.ip_basestation,arguments->ip_basestation,15);
    unsigned int clientAddrLength = sizeof(requestHandlerParams.clientAddr);

    while (1) {
        // Auf Nachrichten des Clients warten und diese puffern
        // Wait for client requests
        recvfrom(sock, requestHandlerParams.oscInputMessage, OSC_MESSAGE_MAX_LENGTH, 0,
             (struct sockaddr *) &requestHandlerParams.clientAddr, &clientAddrLength);
    	
        //Print received Data
        printf("Received OSC message on Port %d from StageControl (%s): %s\n",port, inet_ntoa(requestHandlerParams.clientAddr.sin_addr),
           requestHandlerParams.oscInputMessage);

        //Open requestHandler
        pthread_t id;
        pthread_create(&id, NULL, requestHandler, &requestHandlerParams);

        // Block current thread until the request handler thread exists
        pthread_join(id, NULL);
        
        //Clear Buffer
        memset(requestHandlerParams.oscInputMessage, 0, OSC_MESSAGE_MAX_LENGTH );
    }

    // Socket schliessen
    close(sock);
}

int main(void) {
    //Declare config structure and counter variable
    config myConfig = {};
    unsigned short number = 0;

    //Load config file
    if (loadFile(&myConfig)) {

        //Determine array size of config structure
        size_t n = sizeof(myConfig.port) / sizeof(myConfig.port[0]);

        //Count valid port numbers in array and store value in number variable
        for (int i = 0; i < n; i++) {
            if (myConfig.port[i] != 0) {
                number++;
            }
        }

        //Print detected config information
        for (int i = 0; i < number; i++) {
            printf("IP %d: %s \n", i, myConfig.ip[i]);
            printf("Port %d: %d \n\n", i, myConfig.port[i]);
        }
    } else {
        printf("Konfiguration konnte nicht geladen werden! \n");
    }

    //Create server thread for every valid port config found
    if (number > 0) {
        printf("Konfiguration erfolgreich beendet. Es werden %d Serverthreads gestaret.\n",number);
        //Define thread handlers and connection structure for server
        pthread_t server_thrd[number];
        server_arg server_connect[number];
        void *thr[number];
        int status[number];

        //Create server config array
        for (int i = 0; i < number; i++) {
            server_connect[i].sock = socket(AF_INET, SOCK_DGRAM, 0);
            server_connect[i].port = myConfig.port[i];
            server_connect[i].ip_basestation = myConfig.ip[i];
            //Open server thread
            status[i] = pthread_create(&server_thrd[i], NULL, server, &server_connect[i]);
        }

        //Create server config array
        for (int i = 0; i < number; i++) {
            pthread_join(server_thrd[i], thr[i]);
            printf("Thread %d beendet mit Status %d \n", i, status[i]);
        }
    } else {
        printf("Keine gültige Konfiguration gefunden \n");
    }

    //Exit program
    printf("Alle Threads beendet\n");
    exit(0);
}
