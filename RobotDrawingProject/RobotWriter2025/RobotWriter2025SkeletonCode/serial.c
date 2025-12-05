#include <stdio.h>
#include <stdlib.h>

#include "serial.h"
//#include "rs232.h"


//#define Serial_Mode

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


// Open port with checking
int CanRS232PortBeOpened ( void )
{
    return (0);      // Success
}

// Function to close the COM port
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
    return 0;   //Success
}





#endif // SM




