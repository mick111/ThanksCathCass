
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "MessageDB.h"
#include "MessageBox.h"
#include <sys/ioctl.h>	/* ioctl, TIOCGWINSZ */


extern FILE * FileDebug;
extern FILE * FileOutput;
extern FILE * FileError;

#define SLEEP_BETWEEN_TWO_BOXES 2 

const char REALISEPAR[] = "Realise par les V\x1b[2mieux\x1b[0mC\x1b[2mons\x1b[0mA\x1b[2migris\x1b[0m2010 <ThxCathCass@gmail.com>";
const char MERCI[] = "Merci cathcass !";
const char MESSAGESPAR[] = "Messages par les eleves de l'Ensimag";


void verifyTerminalSize(void);

void verifyTerminalSize(void)
{
        setScreenHeight(MIN_LINESIZE);
        setScreenWidth(MIN_COLUMNSIZE);
        
        if (FileOutput)
        {
                struct winsize ws;
                int FileOutputFD = fileno(FileOutput);
                
                if (ioctl(FileOutputFD, TIOCGWINSZ, &ws) < 0)
                {
                        fprintf(FileOutput, "La taille de votre terminal "
                                "doit faire au moins [%dx%d] et n'a pas pu etre determinee\n"
                                "Agrandissez maintenant la fenetre et appuyez sur ENTREE\n",
                                MIN_COLUMNSIZE, MIN_LINESIZE);
                        fflush(stdin);
                        getchar();     
                        fflush(stdin);
                        
                        setScreenHeight(MIN_LINESIZE);
                        setScreenWidth(MIN_COLUMNSIZE);
                }
                else
                {
                        fprintf(FileOutput, "La taille de votre terminal "
                                "doit faire au moins [%dx%d] et fait actuellement [%dx%d]\n"
                                "Agrandissez maintenant la fenetre et appuyez sur ENTREE\n",
                                MIN_COLUMNSIZE, MIN_LINESIZE,
                                ws.ws_col, ws.ws_row);
                        fflush(stdin);
                        getchar();     
                        fflush(stdin);
                        
                        
                        ioctl(FileOutputFD, TIOCGWINSZ, &ws);
                        
                        setScreenHeight(ws.ws_row);
                        setScreenWidth(ws.ws_col);
                        
                        fprintf(FileOutput, "La taille de votre terminal "
                                "enregistrée est : [%dx%d].\nLancement dans 2 secondes...\n",
                                getScreenWidth(), getScreenHeight());
                        sleep(2);       
                }
        }
}


int main()
{
        /* Local Variables */
        int l_iResult = 0;
        unsigned int l_uiMesgNb = 0;
        
#ifdef DEBUG
        FileDebug      = stderr;
        FileOutput     = stdout;
        FileError      = stderr;
#else
        FileDebug      = NULL;
        FileOutput     = stdout;
        FileError      = NULL;
#endif
        
        
        /* Verify Window size */
        verifyTerminalSize();
        
        /* Clear output screen */
        if (FileOutput) fprintf(FileOutput, "\x1b[0m\x1b[2J");
        
        /* Shuffle Messages */
        melange();
        
        /* Printing all messages */
        for (l_uiMesgNb = 0; l_uiMesgNb < NBMESSAGES; l_uiMesgNb++) 
        {
#ifdef DEBUG
                if (FileDebug)
                {
                        fprintf(FileDebug, "Msg (%d)\n", g_auiOrdre[l_uiMesgNb]);
                }
#endif
                
                l_iResult = CreateMessageBox(MESSAGES[g_auiOrdre[l_uiMesgNb]]);
#ifdef DEBUG
                if (FileDebug) 
                {
                        fprintf(FileDebug, "CreateMessageBox returned %d\n", l_iResult);
                }
#endif
                sleep(SLEEP_BETWEEN_TWO_BOXES);
        }
        
        if (FileOutput) fprintf(FileOutput, "Appuyez sur ENTREE pour quitter... \n");
        
        /* Wait for input */
        fflush(stdin);
        getchar();
        fflush(stdin);
        
        /* Last message */
        if (FileOutput) 
        {
                fflush(FileOutput);
                fprintf(FileOutput, "\x1b[0m\x1b[2J");
                fprintf(FileOutput, "\x1b[%d;%df%s", getScreenHeight()/2, (getScreenWidth() - (unsigned int) sizeof(MERCI))      /2, MERCI);
                fprintf(FileOutput, "\x1b[%d;%df%s", getScreenHeight()-1, (getScreenWidth() - (unsigned int) (sizeof(REALISEPAR)-6*4)) /2, REALISEPAR);
                fprintf(FileOutput, "\x1b[%d;%df%s", getScreenHeight()-2, (getScreenWidth() - (unsigned int) sizeof(MESSAGESPAR))/2, MESSAGESPAR);
                fprintf(FileOutput, "\x1b[%d;1f",getScreenHeight());
                fflush(FileOutput);
        }
        
        return l_iResult;
}

