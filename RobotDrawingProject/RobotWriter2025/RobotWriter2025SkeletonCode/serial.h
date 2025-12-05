#include <stdio.h>
#include <string.h>


#ifndef SERIAL_H_INCLUDED
#define SERIAL_H_INCLUDED


#define cport_nr    5                  /* COM number minus 1 */
#define bdrate      115200              /* 115200  */

//Predefined functions for RS232 communication
int PrintBuffer (char *buffer);                 //JIB: Needed to match the function
int WaitForReply (void);                        // Wit for OK function
int WaitForDollar (void);                       // Wait for '$' function (for startup)
int CanRS232PortBeOpened ( void );              // Port open check
void CloseRS232Port (void);

//Defining FontChar structure for a character's movement data
typedef struct {
    int asciiValue;
    int numberMovements;
    float x[100];
    float y[100];
    int penState[100];
} FontChar;

//Programme function prototypes according to robot planning work
int openText(const char* filePath, char* textHold, int maxLength);
int fetchFont(int asciiValue, FontChar* fontSet);
int applyScaleFactor(FontChar* fontSet, float scaleFactor);
int newPosition(float* currentX, float* currentY, float x[], float y[], int numberMovements);
int checkWidth(float currentX, int maxWidth);
int applyLineBreak(float* currentX, float* currentY);
int generateGcode(int asciiValue, float x[], float y[], int penState[], int numberMovements);
int applyCharacterSpacing(float* currentX, float charSpacing, float wordSpacing, int asciiValue);
int penStateOrigin(float* currentX, float* currentY, int* penState);
int checkWordFit(char textHold[], int startIndex, float currentX, int maxWidth);
void SendCommands(char *buffer);

#endif // SERIAL_H_INCLUDED
