#include <stdio.h>
#include <stdlib.h>

#include "serial.h"
#include "rs232.h"


#define Serial_Mode

#ifdef Serial_Mode

// Open port with checking
int CanRS232PortBeOpened ( void )
{
    char mode[]= {'8','N','1',0};
    if(RS232_OpenComport(cport_nr, bdrate, mode))
    {
        printf("Can not open comport\n");

        return(-1);
    }
    return (0);      // Success
}

// Function to close the COM port
void CloseRS232Port (void)
{
    RS232_CloseComport(cport_nr);
}

// Write text out via the serial port
int PrintBuffer (char *buffer)
{
    RS232_cputs(cport_nr, buffer);
    printf("sent: %s\n", buffer);

    return (0);

}


int WaitForDollar (void)
{


    int i, n;

    unsigned char buf[4096];

    while(1)
    {
        printf (".");
        n = RS232_PollComport(cport_nr, buf, 4095);

        if(n > 0)
        {
            printf ("RCVD: N = %d ", n);
            buf[n] = 0;   /* always put a "null" at the end of a string! */

            for(i=0; i < n; i++)
            {
                if(buf[i] == '$')  /* replace unreadable control-codes by dots */
                {
                    printf("received %i bytes: %s \n", n, (char *)buf);
                    printf("\nSaw the Dollar");
                    return 0;
                }
            }

            printf("received %i bytes: %s \n", n, (char *)buf);


            if ( (buf[0] == 'o') && (buf[1] == 'k') )
                return 0;
        }


        Sleep(100);

    }

    return(0);

}


int WaitForReply (void)
{


    int i, n;

    unsigned char buf[4096];

    while(1)
    {
        printf (".");
        n = RS232_PollComport(cport_nr, buf, 4095);

        if(n > 0)
        {
            printf ("RCVD: N = %d ", n);
            buf[n] = 0;   /* always put a "null" at the end of a string! */

            for(i=0; i < n; i++)
            {
                if(buf[i] < 32)  /* replace unreadable control-codes by dots */
                {
                    buf[i] = '.';
                }
            }

            printf("received %i bytes: %s\n", n, (char *)buf);


            if ( (buf[0] == 'o') && (buf[1] == 'k') )
                return 0;
        }


        Sleep(100);

    }

    return(0);

}

// Error was here - this should be 'ELSE' not 'ELSEIF'

#else


 //Open port with checking
int CanRS232PortBeOpened ( void )
{
    return (0);      // Success
}

 //Function to close the COM port
void CloseRS232Port (void)
{
    return;
}

// JIB: you MUST specify variable types in function definitions
int PrintBuffer (char *buffer)
{
    printf("%s \n",buffer);
    return (0);
}


int WaitForReply (void)
{
    char c;
    c = getchar();
    return (0);
}

int WaitForDollar (void)
{
    char c;
    c = getchar();
    return (0);
}








#endif // SM

int penStateOrigin(float* currentX, float* currentY, int* penState) {
    if (currentX == NULL || currentY == NULL || penState == NULL) { //if parameters are empty or invalid, provide error message and return -1
        printf( "Error in penStateOrigin: pointers invalid\n");
        return -1;
    }

    *penState = 0;
    *currentX = 0.0;    //set all parameters to 0 for origin point
    *currentY = 0.0;
    
    if (*currentX !=0.0 || *currentY != 0.0) {
        printf("Error: pen not returned to origin upon process completion (X = %.2f, Y = %.2f)\n", *currentX, *currentY);
        return -1;  //recommendation to rerun penStateOrigin, or manually reset pen to origin state before beginning again
    }
    return 0; //success
}

int openText(const char* filePath, char* textHold, int maxLength) {
    FILE *fp = fopen(filePath, "r");
    if (fp == NULL) {
        printf("Error: No text file found: %s\n", filePath);  //Error returned if no file found with expected filepath linked, returns -1
        return -1;
    }
    int i = 0;
    int c;
    while ((c = fgetc(fp)) != EOF && i < maxLength - 1){    //returns EOF when reaching end of file, stopping the loop. maxLen - 1 ensures we don't write past the end of the set buffer
        textHold[i++] = (char)c;
    }
    textHold[i] = '\0';     //reserves final slot for null terminator '\0' to ensure string is valid

    fclose(fp);
    return 0;   //success
}


int fetchFont(int asciiValue, FontChar* fontSet) {
    FILE *fp = fopen("SingleStrokeFont.txt", "r");
    if (fp == NULL) {
        printf("Error: no font data found\n");  //returns -1 and prints error if trouble opening font data, e.g. from SingleStrokeFont.txt
        return -1; 
    }
    int marker, ascval, numMoves;   //new variable ascval holds ASCII code read from file, numMoves holds the number of movements required for the pen for obtained character

        while (fscanf(fp, "%d %d %d", &marker, &ascval, &numMoves) ==3) {
            if (marker != 999) {    //identify marker 999 indicating start line of character font data
                continue;
            }
            
        if (ascval == asciiValue) {
            fontSet->asciiValue = ascval;       //ascval and numMoves compared to their expected equivalent from the font data, if correct they are stored
            fontSet->numberMovements = numMoves;

            fontSet->x = malloc(numMoves * sizeof(float));
            fontSet->y = malloc(numMoves * sizeof(float));
            fontSet->penState = malloc(numMoves * sizeof(int));     //allocate memory to X, Y and penState

            if (!fontSet->x || !fontSet->y || !fontSet->penState) {     //reports error in case allocation of memory fails
                free(fontSet->x);
                free(fontSet->y);
                free(fontSet->penState);            //free any allocated memory to X, Y and penState to prevent leaks
                printf("Error: memory allocation failed\n");
                fclose(fp);
                return -1;
            }

            for (int i = 0; i < numMoves; i++) {
                if (fscanf(fp, "%f %f %d", &fontSet->x[i], &fontSet->y[i], &fontSet->penState[i]) != 3) {   //check all 3 items are read successfully, it not return -1 and print error in reading font data
                    free(fontSet->x);
                    free(fontSet->y);
                    free(fontSet->penState);        //free any allocated memory to X, Y and penState to prevent leaks
                    printf("Error in reading stroke data\n");
                    fclose(fp);
                    return -1; 
                }
            }
            fclose(fp);
            return 0;   //in successful case
        }   
        else {
                for (int i = 0; i < numMoves; i++) {    //if incorrect ASCII value is retrieved we read and discard the values with dummy commands. Loop continues to read next values until correct one is found, or EOF is reached
                    float dummyX, dummyY;
                    int dummyPen;
                    fscanf(fp, "%f %f %d", &dummyX, &dummyY, &dummyPen);
                }
            }
        }             
        fclose(fp);
        return -1; //failure, character could not be found 
    }


int applyScaleFactor(FontChar* fontSet, float scaleFactor) {
    if (scaleFactor<4.0 || scaleFactor>10.0) {
        printf("Error: scale factor out of range, please input between 4mm and 10mm\n");    //according to brief scale factor must be between 4mm and 10mm
        return -1;
    }

    float factor = scaleFactor / 18.0;      //error was here, hadn't added in conversion of /18.0 meaning all values were massively scaled
    for (int i = 0; i<fontSet-> numberMovements; i++) {     //multiply values in font array by scale factor to increase to desired font size
        fontSet->x[i] *= factor;
        fontSet->y[i] *= factor;
    }
    return 0;   //success
}


int newPosition(float* currentX, float* currentY, float *x, float *y, int numberMovements) {
    if (currentX == NULL || currentY == NULL || x == NULL || y == NULL) {    //error catch to highlight exact issue in newPosition function - currentX or currentY, or x or y arrays invalid or empty
        printf("Error in newPosition: invalid inputs\n");
        return -1;
    }
    if (numberMovements <= 0) {     //error catch to highlight exact issue in newPosition function - number movements input invalid
        printf("Error in newPosition: number of movements invalid (%d)\n", numberMovements);
        return -1;
    }
    
    for (int i = 0; i<numberMovements; i++) {   //increment/decrement stroke coordinates based on currentX or currentY
    x[i] += *currentX;
    y[i] += *currentY;
    }
    return 0; //success
}


int applyCharacterSpacing(float* currentX, float charSpacing, float wordSpacing, int asciiValue) {
    if (currentX == NULL) {
        printf("Error in applyCharacterSpacing: pointer invalid\n");    //if currentX is invalid, return -1 and print error
        return -1;
    }
    if (asciiValue == 32) {
        *currentX += wordSpacing;   //32 ASCII value = Space, so apply word spacing of 5mm
    } else{
        *currentX += charSpacing;   //apply 2mm spacing between characters
    }
    return 0; //success
}


int checkWidth(float currentX, int maxWidth) {
    if (currentX > maxWidth) {
        return -1; //line break required
    }
    return 0;   //width OK to continue
}


int applyLineBreak(float* currentX, float* currentY) {
    if (currentX == NULL || currentY == NULL) {
        printf("Error in applyLineBreak: pointers invalid\n");
        return -1;
    }
    *currentX = 0.0;
    *currentY -= 5.0;   //reset coordinate points to 0 in X, -5 lower in Y

    //**MUST CHECK AXIS DIRECTION, IF -5.0 MOVES UP THIS MUST CHANGE SIGNS

    return 0; //success
}


int checkWordFit(const char *textHold, int startIndex, float currentX, int maxWidth) {
    if (textHold == NULL) {
        printf("Error in checkWordFit: invalid text hold\n");   //If text hold is invalid, return -1 and error
        return -1;
    }

    int wordLength = 0;
    int i = startIndex;
    while (textHold[i] != '\0' && textHold[i] != ' ') { //count all characters until next space, or string ends
        wordLength++;   //increment wordLength and startIndex
        i++;
    }
    if (wordLength == 0) {
        return 0;   //nothing to check
    }

    float requiredWidth = (wordLength * 2.0) + 5.0;
    if ((currentX + requiredWidth) > maxWidth) {
        return -1;  //word does not fit, line break will be applied
    }
    return 0;   //word fits, great success
}


int generateGcode(int asciiValue, float *x, float *y, int *penState, int numberMovements, int lastPenState) {
    if (x == NULL || y == NULL || penState == NULL || numberMovements <= 0) {   
        printf("Error in generateGcode: inputs invalid\n");     //if parameters are empty or invalid, provide error message and return -1
        return -1;
    }
    
    char buffer[128];       //buffer to hold formatted G-code strings
    
    for (int i = 0; i < numberMovements; i++) {     //loop through all stroke movements for current character
        float xi = x[i];                            //current X
        float yi = y[i];                            //current Y
        int pen = penState[i];                      //current pen position (up/down)

        if (pen != lastPenState) {
            if (pen == 1) {
                SendCommands("S1000");      //send command to move pen down whenever pen = 1, and pen does not equal its last pen state
                printf("Pen moved down\n");
            } else {
                SendCommands("S0");         //send command to move pen up whenever pen = 0, and pen does not equal its last pen state
                printf("Pen moved up\n");
            }
            lastPenState = pen;            //update lastpenstate with final pen position in this loop, so next loop same process can occur
        }

        if (pen == 1) {         //format and send movement commands depending on pen position
            snprintf(buffer, sizeof(buffer), "G1 X%.2f Y%.2f", xi, yi);     //G1 = pen down, + drawing movement in X and Y
            SendCommands(buffer);
        } else {
            snprintf(buffer, sizeof(buffer), "G0 X%.2f Y%.2f", xi, yi);    //G0 = pen up, + drawing movement in X and Y
            SendCommands(buffer);
            }
        }
    return 0;
    }
