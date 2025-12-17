#include <stdio.h>
#include <stdlib.h>
//#include <conio.h>
//#include <windows.h>
#include "rs232.h"
#include "serial.h"

#define bdrate 115200               /* 115200 baud */

int main(void)
{

    //char mode[]= {'8','N','1',0};
    char buffer[100];

    //planned function variables
    char textHold[256];         //preset 256, can be increased with larger file sizes
    FontChar fontSet;
    float charSpacing = 2.0;        //2mm spacing between characters (not spaces)
    float wordSpacing = 5.0;        //5mm spacing between words
    float cursorX = 0.0, cursorY = 0.0;
    int penState = 0;
    int maxWidth = 100; //width of writing space 100mm
    float scaleFactor;
    int lastPenState = 0;

     //If we cannot open the port then give up immediately
    if ( CanRS232PortBeOpened() == -1 )
    {
        printf ("\nUnable to open the COM port (specified in serial.h) ");
        exit (0);
    }

    // Time to wake up the robot
    printf ("\nAbout to wake up the robot\n");

    // We do this by sending a new-line
    sprintf (buffer, "\n");
     // printf ("Buffer to send: %s", buffer); // For diagnostic purposes only, normally comment out
    PrintBuffer (&buffer[0]);
    Sleep(100);

    // This is a special case - we wait  until we see a dollar ($)
    WaitForDollar();

    printf ("\nThe robot is now ready to draw\n");

        //These commands get the robot into 'ready to draw mode' and need to be sent before any writing commands
    sprintf (buffer, "G1 X0 Y0 F1000\n");
    SendCommands(buffer);
    sprintf (buffer, "M3\n");
    SendCommands(buffer);
    sprintf (buffer, "S0\n");
    SendCommands(buffer);

    printf("Please enter text height value in mm, between 4mm and 10mm:\n");            //request user input for text height, for scaleFactor to work off
    if (scanf("%f", &scaleFactor) != 1 || scaleFactor < 4.0 || scaleFactor > 10.0) {
        printf("Error: text height must be between 4mm and 10mm\n");
        return -1;
    }

    if (openText("test.txt", textHold, sizeof(textHold)) != 0) {
        printf("Error: could not open text file\n");        //similar to in serial.c, check txt file can be opened prior to starting loop to quickly verify
        CloseRS232Port();
        return -1;
    }

    for (int i = 0; textHold[i] != '\0'; i++) {
        if (checkWordFit(textHold, i, cursorX, maxWidth) != 0) {        //checkWordFit function to verify word fits in available space
            applyLineBreak(&cursorX, &cursorY);                         //applyLineBreak occurs as this loop is dependent on the returned value not being equal to 0, which would not require a line break
        }
        if (fetchFont((int)textHold[i], &fontSet) ==0) {                //fetchFont function called
            if (applyScaleFactor(&fontSet, scaleFactor) == 0) {         //applyScaleFactor function dependent on the user input scaleFactor
                newPosition(&cursorX, &cursorY, fontSet.x, fontSet.y, fontSet.numberMovements);     //newPosition function called to output required position based on input values from SingleStrokeFont and scaleFactor
                generateGcode((int)textHold[i], fontSet.x, fontSet.y, fontSet.penState, fontSet.numberMovements, lastPenState);   //G code generated to be received by Arduino
                applyCharacterSpacing(&cursorX, charSpacing, wordSpacing, (int)textHold[i]);        //spacing applied after each character, 2mm between characters and 5mm if space (ASCII = 32) detected
            }
            free(fontSet.x);
            free(fontSet.y);
            free(fontSet.penState);
        }
    }
    SendCommands("G0 X0 Y0");   //return to origin
    penStateOrigin(&cursorX, &cursorY, &penState);
    SendCommands("S0");      //finish in pen-up state
    // Before we exit the program we need to close the COM port
    CloseRS232Port();
    printf("Com port now closed\n");

    return (0);
}

// Send the data to the robot - note in 'PC' mode you need to hit space twice
// as the dummy 'WaitForReply' has a getch() within the function.
void SendCommands (char *buffer )
{
    //printf ("Buffer to send: %s", buffer); // For diagnostic purposes only, normally comment out
    PrintBuffer (&buffer[0]);
    WaitForReply();
    Sleep(100); // Can omit this when using the writing robot but has minimal effect
    // getch(); // Omit this once basic testing with emulator has taken place
}
