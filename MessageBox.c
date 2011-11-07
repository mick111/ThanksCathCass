
#include "MessageBox.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>


FILE * FileDebug  = NULL;
FILE * FileOutput = NULL;
FILE * FileError  = NULL;

#define MAX(x,y) ((x)>(y)?(x):(y))
#define MIN(x,y) ((x)<(y)?(x):(y))

/******************************************************************************/
/*                              Error Management                              */
/******************************************************************************/
static void CheckReturnedStatus(const char * const par_szFunctionName,
                                const char * const par_szOperation,
                                const int          par_iRetValue)
{
        if (!FileError) return;

        if (par_iRetValue < CREATION_OK)
        {
                fprintf(FileError, "Error in function [%s] -- "
                        "%s failed with returned value : %d\n",
                        par_szFunctionName, par_szOperation, par_iRetValue);
        }
        if (par_iRetValue > CREATION_OK)
        {
                fprintf(FileError, "Warning in function [%s] -- "
                        "%s failed with returned value : %d\n",
                        par_szFunctionName, par_szOperation, par_iRetValue);
        }
}

/******************************************************************************/
/*                              Global Variables                              */
/******************************************************************************/
static unsigned int g_uiScreenWidth  = MIN_COLUMNSIZE;
static unsigned int g_uiScreenHeight = MIN_LINESIZE;

static unsigned int g_uiLastMsgBoxPosX  = 1;
static unsigned int g_uiLastMsgBoxPosY  = 1;
static unsigned int g_uiLastMsgBoxSizeX = 1;
static unsigned int g_uiLastMsgBoxSizeY = 1;


/******************************************************************************/
/*                          Screen Size Management                            */
/******************************************************************************/
unsigned int setScreenWidth(unsigned int par_uiScreenWidth)
{
        par_uiScreenWidth = g_uiScreenWidth   + par_uiScreenWidth;
        g_uiScreenWidth   = par_uiScreenWidth - g_uiScreenWidth;

        /* Returning the old value of g_uiScreenWidth */
        return (par_uiScreenWidth - g_uiScreenWidth);
}

unsigned int setScreenHeight(unsigned int par_uiScreenHeight)
{
        par_uiScreenHeight = g_uiScreenHeight   + par_uiScreenHeight;
        g_uiScreenHeight   = par_uiScreenHeight - g_uiScreenHeight;

        /* Returning the old value of g_uiScreenHeight */
        return (par_uiScreenHeight - g_uiScreenHeight);
}

unsigned int getScreenWidth()
{
        /* Returning the value of g_uiScreenWidth */
        return (g_uiScreenWidth);
}

unsigned int getScreenHeight()
{
        /* Returning the value of g_uiScreenHeight */
        return (g_uiScreenHeight);
}

/******************************************************************************/
/*                                  Colors                                    */
/******************************************************************************/

#define FOREGROUND 30
#define BACKGROUND 40
#define BLACK       0
#define RED         1
#define GREEN       2
#define YELLOW      3
#define BLUE        4
#define MAGENTA     5
#define CYAN        6
#define WHITE       7

const char AUTHORIZED_COLORS[][6] =
       {"30;41", "30;42", "30;43", "30;47",
        "32;40", "32;44", "33;41", "33;45",
        "34;40", "34;47", "35;40", "36;40", "36;44",
        "37;40", "37;41", "37;44", "37;45"};

#define RESETCOLORCOMMAND "\x1b[0m"

static void resetColor(void)
{
        if (FileOutput) fprintf(FileOutput, "%s",RESETCOLORCOMMAND);
}

static void getNewControlCommand(t_szCommandArray par_aszControlCommand, /* OUT */
                                 unsigned int     par_uiPosX,            /* IN */
                                 unsigned int     par_uiPosY,            /* IN */
                                 unsigned int     par_uiColor,           /* IN */
                                 unsigned char    par_ucBold)            /* IN */
{
        /* Command is the control command to the terminal */
	if (par_ucBold)
        {
                snprintf(par_aszControlCommand,
                         LENGTHOFCONTROLCOMMAND,
                         "\x1b[%s;1m\x1b[%d;%df",
                         AUTHORIZED_COLORS[par_uiColor],
                         par_uiPosY+1, par_uiPosX+1);
        }
        else
        {
                snprintf(par_aszControlCommand,
                         LENGTHOFCONTROLCOMMAND,
                         "\x1b[0;%sm\x1b[%d;%df",
                         AUTHORIZED_COLORS[par_uiColor],
                         par_uiPosY+1, par_uiPosX+1);
        }
        par_aszControlCommand[LENGTHOFCONTROLCOMMAND] = 0;
}




/******************************************************************************/
/*                            Messages Content                                */
/******************************************************************************/
typedef struct Message_S
{
        char             aszLogin[9];
        unsigned short   usPromo;
        unsigned short   usLargeurMessage;
        t_szOneLineArray aszLigne1;
        t_szOneLineArray aszLigne2;
        t_szOneLineArray aszLigne3;
} Message_T;

#define WRAPMESSAGE_WAR_MISWRAPPED CREATION_WAR_MISWRAPPED
#define WRAPMESSAGE_OK             CREATION_OK
static int WrapMessage(t_szMessageArray par_aszMessage,     /* IN */
                       size_t *         par_iWrapPosition1, /* OUT */
                       size_t *         par_iWrapPosition2) /* OUT */
{
        /* Local Variables */
        size_t l_WrapPosition = 0;
        int l_iStatus = CREATION_OK;

        /* Getting message length */
        const size_t l_SizeOfMessage = MIN(strlen(par_aszMessage),
                                           MSGBOX_MAX_MSG_LENGTH);

        /* If message is short enough, no need to wrap */
        if (l_SizeOfMessage < ONE_LINE_MSG_MAX_LENGTH)
        {
                *par_iWrapPosition1 = l_SizeOfMessage;
                *par_iWrapPosition2 = l_SizeOfMessage;
        }
        /* The message is long enough to be split 1 times */
        else if (l_SizeOfMessage < TWO_LINE_MSG_MAX_LENGTH)
        {
                /* Looking for a blank in a certain interval */
                for (l_WrapPosition = (l_SizeOfMessage/2 - MSGBOX_PADDING_SIZE);
                     l_WrapPosition < (l_SizeOfMessage/2 + MSGBOX_PADDING_SIZE);
                     l_WrapPosition++)
                {
                        if (par_aszMessage[l_WrapPosition] == ' ')
                                break;
                }
                if (par_aszMessage[l_WrapPosition] != ' ')
                {
                        /* No spaces found ! We wrap at the exact middle */
                        l_WrapPosition = l_SizeOfMessage/2;
                        l_iStatus = WRAPMESSAGE_WAR_MISWRAPPED;
                }
                *par_iWrapPosition1 = l_WrapPosition;
                *par_iWrapPosition2 = l_SizeOfMessage;
        }
        /* The message is long enough to be split 2 times */
        else
        {
                /* Looking for a blank in a certain interval */
                for (l_WrapPosition = (l_SizeOfMessage/3 - MSGBOX_PADDING_SIZE/2) - 1;
                     l_WrapPosition < (l_SizeOfMessage/3 + MSGBOX_PADDING_SIZE);
                     l_WrapPosition++)
                {
                        if (par_aszMessage[l_WrapPosition] == ' ')
                                break;
                }
                if (par_aszMessage[l_WrapPosition] != ' ')
                {
                        /* No spaces found ! We wrap at the exact middle */
                        l_WrapPosition = l_SizeOfMessage/3;
                        l_iStatus = WRAPMESSAGE_WAR_MISWRAPPED;
                }
                *par_iWrapPosition1 = l_WrapPosition;

                /* Looking for a blank in a certain interval */
                for (l_WrapPosition = (2*l_SizeOfMessage/3 - MSGBOX_PADDING_SIZE) - 1;
                     l_WrapPosition < (2*l_SizeOfMessage/3 + MSGBOX_PADDING_SIZE/2);
                     l_WrapPosition++)
                {
                        if (par_aszMessage[l_WrapPosition] == ' ')
                                break;
                }
                if (par_aszMessage[l_WrapPosition] != ' ')
                {
                        /* No spaces found ! We wrap at the exact middle */
                        l_WrapPosition = 2*l_SizeOfMessage/3;
                        l_iStatus = WRAPMESSAGE_WAR_MISWRAPPED;
                }
                *par_iWrapPosition2 = l_WrapPosition;
        }
        return l_iStatus;
}

#define ANALYSESTRING_WAR_TRUNCATED            CREATION_WAR_TRUNCATED
#define ANALYSESTRING_ERR_WRONG_MESSAGE_FORMAT CREATION_ERR_WRONG_MESSAGE_FORMAT
#define ANALYSESTRING_ERR_INTERNALERROR        CREATION_ERR_INTERNALERROR
#define ANALYSESTRING_OK                       CREATION_OK
static int AnalyseString(const char * const par_pszMessageString,   /* IN  */
                         Message_T  *       par_psMessageStructure) /* OUT */
{
        /* Local variables declaration */
        t_szMessageArray l_aszMessage = {0};
        size_t l_iResult              = 0;
        size_t l_iWrapPos1            = 0;
        size_t l_iWrapPos2            = 0;
        int    l_iStatus              = ANALYSESTRING_OK;
        int    l_iSizeHeader          = 0;
        unsigned int l_uiPromo        = 0;

        /* Test of the parameters */
        if (par_pszMessageString == NULL)
        {
                if (FileError) fprintf(FileError, "Error in function [%s] -- "
                        "The given message pointer is NULL\n",
                        __FUNCTION__);
                return ANALYSESTRING_ERR_INTERNALERROR;
        }
        if (par_psMessageStructure == NULL)
        {
                if (FileError) fprintf(FileError, "Error in function [%s] -- "
                        "The given message structure is not instanciated"
                        "(pointer is NULL)\n",
                        __FUNCTION__);
                return ANALYSESTRING_ERR_INTERNALERROR;
        }

        /* Read the message */
        if (strlen(par_pszMessageString) < 8)
        {
                if (FileError) fprintf(FileError, "Error in function [%s] -- "
                        "The given message string \"%s\""
                        " has an incorrect format\n",
                        __FUNCTION__, par_pszMessageString);
                return ANALYSESTRING_ERR_WRONG_MESSAGE_FORMAT;
        }

        /* Read Header */
        l_iResult = (size_t) sscanf(par_pszMessageString, "%[^_]_%4u:%n",
                                    &(par_psMessageStructure->aszLogin[0]),
                                    &l_uiPromo,
                                    &l_iSizeHeader);
        par_psMessageStructure->usPromo = l_uiPromo & 0xFFFF;

        /* Status of sscanf (%n doesn't count) */
        if (l_iResult != 2)
        {
                if (FileError)
                {
                        fprintf(FileError, "Error in function [%s] -- "
                                "The given message string \"%s\""
                                "has an incorrect format (not recognized)\n",
                                __FUNCTION__, par_pszMessageString);
                        switch (l_iResult)
                        {
                                case 0: fprintf(FileError,
                                                "Nothing has been recognized\n");
                                        break;
                                case 1: fprintf(FileError,
                                                "LOGIN '%s' has been recognized\n",
                                                par_psMessageStructure->aszLogin);
                                        break;
                                default:break;
                        }
                }
                return ANALYSESTRING_ERR_WRONG_MESSAGE_FORMAT;
        }

        /* Checking message length */
        if (strlen(par_pszMessageString + l_iSizeHeader) > MSGBOX_MAX_MSG_LENGTH)
        {
                if (FileError) fprintf(FileError, "Warning in function [%s] -- "
                                       "The given message string \"%s\" is too long "
                                       "(%lu > MSGBOX_MAX_MSG_LENGTH(%lu)): TRUNCATED\n",
                                       __FUNCTION__,
                                       par_pszMessageString+l_iSizeHeader,
                                       strlen(par_pszMessageString + l_iSizeHeader),
                                       MSGBOX_MAX_MSG_LENGTH);
                l_iStatus = ANALYSESTRING_WAR_TRUNCATED;
        }

        /* Read Message */
        strncpy(l_aszMessage,
                par_pszMessageString + l_iSizeHeader,
                MSGBOX_MAX_MSG_LENGTH);
        l_aszMessage[MSGBOX_MAX_MSG_LENGTH] = '\0';

        /* Wrapping read message */
        l_iStatus |= WrapMessage(l_aszMessage, &l_iWrapPos1, &l_iWrapPos2);

        /* Copy 1st Line */
        strncpy(par_psMessageStructure->aszLigne1,
                l_aszMessage,
                l_iWrapPos1);
        par_psMessageStructure->aszLigne1[ONE_LINE_MSG_MAX_LENGTH] = '\0';

        /* Copy 2nd Line */
        if (l_aszMessage[l_iWrapPos1] == ' ')
        {
                strncpy(par_psMessageStructure->aszLigne2,
                        l_aszMessage + l_iWrapPos1 + 1,
                        MIN(l_iWrapPos2 - (l_iWrapPos1 + 1),
                            ONE_LINE_MSG_MAX_LENGTH));
        }
        else
        {
                strncpy(par_psMessageStructure->aszLigne2,
                        l_aszMessage+l_iWrapPos1,
                        MIN(l_iWrapPos2 - l_iWrapPos1,
                            ONE_LINE_MSG_MAX_LENGTH));
        }
        par_psMessageStructure->aszLigne2[ONE_LINE_MSG_MAX_LENGTH] = '\0';

        /* Copy 3rd Line */
        if (l_aszMessage[l_iWrapPos2] == ' ')
        {
                strcpy(par_psMessageStructure->aszLigne3,
                       l_aszMessage + l_iWrapPos2 + 1);
        }
        else
        {
                strcpy(par_psMessageStructure->aszLigne3,
                       l_aszMessage + l_iWrapPos2);
        }
        par_psMessageStructure->aszLigne3[ONE_LINE_MSG_MAX_LENGTH] = '\0';

        /* Get Message Width */
        par_psMessageStructure->usLargeurMessage =
        (unsigned short) MAX (MAX(strlen(par_psMessageStructure->aszLigne1),
                                  strlen(par_psMessageStructure->aszLigne2)),
                              strlen(par_psMessageStructure->aszLigne3));

        return l_iStatus;
}



/******************************************************************************/
/*                              Messages Boxes                                */
/******************************************************************************/
#define NS_DURATION_OF_PRINTING_CHAR 15000000L
typedef struct MessageBox_S
{
        unsigned int uiPosX;
        unsigned int uiPosY;
        unsigned int uiColor;
        Message_T    sMessage;
} MessageBox_T;

#define SHOW_OK           CREATION_OK
#define SHOW_NO_OUTPUT    CREATION_NO_OUTPUT
static int showMessageBox(MessageBox_T * par_psMessageBox)
{
        /* Local Variables */
        char l_szCommandTitle   [LENGTHOFCONTROLCOMMAND + 1] = {0};
        char l_szCommandLine1   [LENGTHOFCONTROLCOMMAND + 1] = {0};
        char l_szCommandLine2   [LENGTHOFCONTROLCOMMAND + 1] = {0};
        char l_szCommandLine3   [LENGTHOFCONTROLCOMMAND + 1] = {0};
        char l_szCommandMsgLine1[LENGTHOFCONTROLCOMMAND + 1] = {0};
        char l_szCommandMsgLine2[LENGTHOFCONTROLCOMMAND + 1] = {0};
        char l_szCommandMsgLine3[LENGTHOFCONTROLCOMMAND + 1] = {0};
        char l_szCommandBottom  [LENGTHOFCONTROLCOMMAND + 1] = {0};

        char l_szTitle [MSGBOX_MAX_X_SIZE + 14 + 1] = {0};
        char l_szLine1 [MSGBOX_MAX_X_SIZE + 1]      = {0};
        char l_szLine2 [MSGBOX_MAX_X_SIZE + 1]      = {0};
        char l_szLine3 [MSGBOX_MAX_X_SIZE + 1]      = {0};
        char l_szBottom[MSGBOX_MAX_X_SIZE + 1]      = {0};

        size_t i = 0;
        size_t l_NbExtraChars     = 0;
        size_t l_paddingLeftLine1 = 0;
        size_t l_paddingLeftLine2 = 0;
        size_t l_paddingLeftLine3 = 0;
        size_t l_sizeOfMsgBox     = 0;

        const struct timespec l_sSleepInterval = {0, NS_DURATION_OF_PRINTING_CHAR};

        if (!FileOutput)
        {
                return SHOW_NO_OUTPUT;
        }

        getNewControlCommand(l_szCommandTitle,
                             par_psMessageBox->uiPosX, par_psMessageBox->uiPosY,
                             par_psMessageBox->uiColor, 0);
        getNewControlCommand(l_szCommandLine1,
                             par_psMessageBox->uiPosX, par_psMessageBox->uiPosY+1,
                             par_psMessageBox->uiColor, 0);

        if (strlen(par_psMessageBox->sMessage.aszLigne2))
        {
                getNewControlCommand(l_szCommandLine2,
                                     par_psMessageBox->uiPosX, par_psMessageBox->uiPosY+2,
                                     par_psMessageBox->uiColor, 0);
                if (strlen(par_psMessageBox->sMessage.aszLigne3))
                {
                        getNewControlCommand(l_szCommandLine3,
                                             par_psMessageBox->uiPosX, par_psMessageBox->uiPosY+3,
                                             par_psMessageBox->uiColor, 0);
                        getNewControlCommand(l_szCommandBottom,
                                             par_psMessageBox->uiPosX, par_psMessageBox->uiPosY+4,
                                             par_psMessageBox->uiColor, 0);
                }
                else
                {
                getNewControlCommand(l_szCommandBottom,
                                     par_psMessageBox->uiPosX, par_psMessageBox->uiPosY+3,
                                     par_psMessageBox->uiColor, 0);
                }
        }
        else
        {
                getNewControlCommand(l_szCommandBottom,
                                     par_psMessageBox->uiPosX, par_psMessageBox->uiPosY+2,
                                     par_psMessageBox->uiColor, 0);
        }

        l_sizeOfMsgBox = MAX(MSGBOX_MIN_X_SIZE,
                             par_psMessageBox->sMessage.usLargeurMessage + 2);

        /* Title */
        l_szTitle[0] = '#';
        for (i = 1; i < l_sizeOfMsgBox/2-7; i++)
        {
                l_szTitle[i] = '=';
        }

        l_NbExtraChars = (size_t) sprintf(l_szTitle+i, "<\x1b[1m%s %04d\x1b[0;%sm>",
                                          par_psMessageBox->sMessage.aszLogin, par_psMessageBox->sMessage.usPromo,
                                          AUTHORIZED_COLORS[par_psMessageBox->uiColor]);
        i += l_NbExtraChars;

        l_NbExtraChars -= 1 + strlen(par_psMessageBox->sMessage.aszLogin) + 1 + 4 + 1;
        
        for (; i < l_sizeOfMsgBox + l_NbExtraChars - 1; i++)
        {
                l_szTitle[i] = '=';
        }
        l_szTitle[i]   = '#';
        l_szTitle[i+1] = '\0';


        /* Ligne 1 */
        l_szLine1[0] = '|';
        for (i = 1; i < l_sizeOfMsgBox-1; i++)
        {
                l_szLine1[i] = ' ';
        }
        l_szLine1[i]   = '|';
        l_szLine1[i+1] = '\0';

        /* Ligne 2 */
        if (strlen(par_psMessageBox->sMessage.aszLigne2))
        {
                l_szLine2[0] = '|';
                for (i = 1; i < l_sizeOfMsgBox-1; i++)
                {
                        l_szLine2[i] = ' ';
                }
                l_szLine2[i]   = '|';
                l_szLine2[i+1] = '\0';
        }
        else
        {
                l_szLine2[0]        = '\0';
                l_szCommandLine2[0] = '\0';
        }

        /* Ligne 3 */
        if (strlen(par_psMessageBox->sMessage.aszLigne3))
        {
                l_szLine3[0] = '|';
                for (i = 1; i < l_sizeOfMsgBox-1; i++)
                {
                        l_szLine3[i] = ' ';
                }
                l_szLine3[i]   = '|';
                l_szLine3[i+1] = '\0';
        }
        else
        {
                l_szLine3[0]        = '\0';
                l_szCommandLine3[0] = '\0';
        }

        /* Bottom */
        for (i = 0; i < l_sizeOfMsgBox; i++)
        {
                l_szBottom[i] = '#';
        }
        l_szBottom[i] = '\0';


        fprintf(FileOutput,
                "%s%s"
                "%s%s"
                "%s%s"
                "%s%s"
                "%s%s",
                l_szCommandTitle, l_szTitle,
                l_szCommandLine1, l_szLine1,
                l_szCommandLine2, l_szLine2,
                l_szCommandLine3, l_szLine3,
                l_szCommandBottom, l_szBottom);


        /* Writing first line */
        l_paddingLeftLine1 = (l_sizeOfMsgBox - 2 - strlen(par_psMessageBox->sMessage.aszLigne1)) / 2;

        getNewControlCommand(l_szCommandMsgLine1,
                             par_psMessageBox->uiPosX+1+(unsigned int)l_paddingLeftLine1, par_psMessageBox->uiPosY+1,
                             par_psMessageBox->uiColor, 1);

        fprintf(FileOutput, "%s",l_szCommandMsgLine1);
        fflush(FileOutput);

        for (i = 0; par_psMessageBox->sMessage.aszLigne1[i]; i++)
        {
                fprintf(FileOutput, "%c",par_psMessageBox->sMessage.aszLigne1[i]);
                fflush(FileOutput);
                nanosleep(&l_sSleepInterval, NULL);
        }

        /* Writing second line */
        l_paddingLeftLine2 = (l_sizeOfMsgBox - 2 - strlen(par_psMessageBox->sMessage.aszLigne2)) / 2;

        getNewControlCommand(l_szCommandMsgLine2,
                             par_psMessageBox->uiPosX+1+(unsigned int)l_paddingLeftLine2, par_psMessageBox->uiPosY+2,
                             par_psMessageBox->uiColor, 1);

        fprintf(FileOutput, "%s",l_szCommandMsgLine2);
        fflush(FileOutput);

        for (i = 0; par_psMessageBox->sMessage.aszLigne2[i]; i++)
        {
                fprintf(FileOutput, "%c", par_psMessageBox->sMessage.aszLigne2[i]);
                fflush(FileOutput);
                nanosleep(&l_sSleepInterval, NULL);
        }

        /* Writing third line */
        l_paddingLeftLine3 = (l_sizeOfMsgBox - 2 - strlen(par_psMessageBox->sMessage.aszLigne3)) / 2;

        getNewControlCommand(l_szCommandMsgLine3,
                             par_psMessageBox->uiPosX+1+(unsigned int)l_paddingLeftLine3, par_psMessageBox->uiPosY+3,
                             par_psMessageBox->uiColor, 1);

        fprintf(FileOutput, "%s",l_szCommandMsgLine3);
        fflush(FileOutput);

        for (i = 0; par_psMessageBox->sMessage.aszLigne3[i]; i++)
        {
                fprintf(FileOutput, "%c", par_psMessageBox->sMessage.aszLigne3[i]);
                fflush(FileOutput);
                nanosleep(&l_sSleepInterval, NULL);
        }

        /* Resetting */
        fprintf(FileOutput, "\x1b[1;1f");
        resetColor();
        fflush(FileOutput);

        return SHOW_OK;
}


/******************************************************************************/
/*                          Position Management                               */
/******************************************************************************/
#define POSITION_OK                      CREATION_OK
#define POSITION_ERR_NOT_ENOUGH_ROOM     CREATION_ERR_NOT_ENOUGH_ROOM
#define POSITION_INTERNAL_ERROR          CREATION_ERR_INTERNALERROR
static int findNewPosition(MessageBox_T *par_sMessageBox)
{
        /* Local Variables */
        unsigned int l_uiSizeX = 0;
        unsigned int l_uiSizeY = 0;

        unsigned int l_uiNbLinesOnTop     = 0;
        unsigned int l_uiNbLinesOnMiddle  = 0;
        unsigned int l_uiNbLinesOnBottom  = 0;


        unsigned int l_uiNbColumnsOnLeft  = 0;
        unsigned int l_uiNbColumnsOnRight = 0;

        unsigned int l_uiRandomNumberX   = 0;
        unsigned int l_uiRandomNumberY   = 0;

        l_uiSizeX = MAX(MSGBOX_MIN_X_SIZE,
                        par_sMessageBox->sMessage.usLargeurMessage+2);
        if (strlen(par_sMessageBox->sMessage.aszLigne2))
        {
                if (strlen(par_sMessageBox->sMessage.aszLigne3))
                {
                        l_uiSizeY = 5;
                }
                else
                {
                        l_uiSizeY = 4;
                }
        }
        else
        {
                l_uiSizeY = 3;
        }


        if (l_uiSizeX > g_uiScreenWidth || l_uiSizeY > g_uiScreenHeight)
        {
                if (FileError) fprintf(FileError, "Error in function [%s] -- "
                        "Not enough room\n"
                        "BoxSize    : [%3d;%3d]\n"
                        "ScreenSize : [%3d;%3d]\n",
                        __FUNCTION__,
                        l_uiSizeX, l_uiSizeY,
                        g_uiScreenWidth, g_uiScreenHeight);
                return POSITION_ERR_NOT_ENOUGH_ROOM;
        }

        /*          LastMsgBoxPosX       LastMsgBoxSizeX                    SizeX    */
        /*        <------------------><------------------>              <----------> */
        /*       ******************************************************************* */
        /*      ^*                                                      ###########* */
        /* Last |*                                                      ###########* */
        /* PosY |*           #############################^             ###########* */
        /*      v*           #############################|SizeY        ###########* */
        /*      ^*           #########+------------------+v             ###########* */
        /* Last |*           #########|  OLD MESSAGEBOX  |              ###########* */
        /* SizeY|*           #########|                  |              ###########* */
        /*      v*           #########+------------------+              ###########* */
        /*       *           <-------->                                 ###########* */
        /*       *             SizeX                                    ###########* */
        /*       *                                                      ###########* */
        /*       *                                                      ###########* */
        /*      ^*#################################################################* */
        /* SizeY|*#################################################################* */
        /*      v******************************************************************* */

        /* Checking number of available lines */

        /* In TOP */
        if (g_uiLastMsgBoxPosY >= l_uiSizeY)
        {
                l_uiNbLinesOnTop = g_uiLastMsgBoxPosY - (l_uiSizeY-1);

                /* Removing lines if last mesgbox is too low */
                if (g_uiLastMsgBoxPosY > g_uiScreenHeight - (l_uiSizeY-1))
                {
                        l_uiNbLinesOnTop -= g_uiLastMsgBoxPosY -
                                            (g_uiScreenHeight - (l_uiSizeY-1));
                }
        }


        /* In MIDDLE */
        /* Checking number of available columns on middle */
        if (g_uiLastMsgBoxPosX >= l_uiSizeX)
        {
                l_uiNbColumnsOnLeft = g_uiLastMsgBoxPosX - (l_uiSizeX-1);
        }

        if (g_uiScreenWidth >= g_uiLastMsgBoxPosX + g_uiLastMsgBoxSizeX + l_uiSizeX)
        {
                l_uiNbColumnsOnRight = g_uiScreenWidth - (g_uiLastMsgBoxPosX + g_uiLastMsgBoxSizeX + (l_uiSizeX-1));
        }

        /* Deducting the number of lines available in MIDDLE */
        if (l_uiNbColumnsOnLeft + l_uiNbColumnsOnRight)
        {
                l_uiNbLinesOnMiddle = g_uiLastMsgBoxSizeY + (l_uiSizeY-1);

                /* Removing lines if last mesgbox is too high */
                if (g_uiLastMsgBoxPosY < (l_uiSizeY-1))
                {
                        l_uiNbLinesOnMiddle -= (l_uiSizeY-1) - g_uiLastMsgBoxPosY;
                }

                /* Removing lines if last mesgbox is too low */
                if (g_uiLastMsgBoxPosY + g_uiLastMsgBoxSizeY > g_uiScreenHeight - (l_uiSizeY-1))
                {
                        l_uiNbLinesOnMiddle -= g_uiLastMsgBoxPosY + g_uiLastMsgBoxSizeY + (l_uiSizeY-1) -g_uiScreenHeight;
                }
        }

        /* In BOTTOM */
        if (g_uiScreenHeight >= g_uiLastMsgBoxPosY + g_uiLastMsgBoxSizeY + l_uiSizeY)
        {
                l_uiNbLinesOnBottom = g_uiScreenHeight - (g_uiLastMsgBoxPosY + g_uiLastMsgBoxSizeY + (l_uiSizeY-1));
        }



        /* Checking if there is room */
        if (!(l_uiNbLinesOnTop + l_uiNbLinesOnMiddle + l_uiNbLinesOnBottom))
        {
                if (FileError) fprintf(FileError, "Error in function [%s] -- "
                        "Not enough room\n"
                        "Top = %3d; Middle = %3d; Bottom = %3d\n",
                        __FUNCTION__,
                        l_uiNbLinesOnTop,
                        l_uiNbLinesOnMiddle,
                        l_uiNbLinesOnBottom);
                return POSITION_ERR_NOT_ENOUGH_ROOM;
        }


        /* Choosing a random Y position */
        l_uiRandomNumberY  = (unsigned int) rand();
        l_uiRandomNumberY %= l_uiNbLinesOnTop + l_uiNbLinesOnMiddle + l_uiNbLinesOnBottom;

        if ((l_uiNbLinesOnMiddle == 0) && (l_uiRandomNumberY >= l_uiNbLinesOnTop))
        {
                par_sMessageBox->uiPosY = l_uiRandomNumberY - l_uiNbLinesOnTop +
                g_uiLastMsgBoxPosY + g_uiLastMsgBoxSizeY;
        }
        else
        {
                par_sMessageBox->uiPosY = l_uiRandomNumberY;
        }


        /* Checking the available columns */
        if ((par_sMessageBox->uiPosY <  l_uiNbLinesOnTop) ||
            (par_sMessageBox->uiPosY >= l_uiNbLinesOnMiddle + l_uiNbLinesOnTop))
        {
                /* We are in TOP or BOTTOM, all columns are available */
                par_sMessageBox->uiPosX = (unsigned int)
                    (rand() %  (g_uiScreenWidth-(l_uiSizeX-1)));
        }
        else
        {
                /* We are in MIDDLE, some columns are not available */
                if (!(l_uiNbColumnsOnLeft + l_uiNbColumnsOnRight))
                {
                        if (FileError)
                        {
                                fprintf(FileError,
                                        "Internal error in [%s] -- "
                                        "We selected MIDDLE but no columns are"
                                        "available ! LEFT : %u RIGHT : %u",
                                        __FUNCTION__,
                                        l_uiNbColumnsOnLeft,
                                        l_uiNbColumnsOnRight);
                        }
                        return POSITION_INTERNAL_ERROR;
                }

                l_uiRandomNumberX = (unsigned int) rand();
                l_uiRandomNumberX %= l_uiNbColumnsOnLeft + l_uiNbColumnsOnRight;

                if (l_uiRandomNumberX < l_uiNbColumnsOnLeft)
                {
                        par_sMessageBox->uiPosX = l_uiRandomNumberX;
                }
                else
                {
                        if (g_uiLastMsgBoxPosX < (l_uiSizeX-1))
                        {
                                par_sMessageBox->uiPosX = l_uiRandomNumberX +
                                                          g_uiLastMsgBoxSizeX +
                                                          g_uiLastMsgBoxPosX;
                        }
                        else
                        {
                                par_sMessageBox->uiPosX = l_uiRandomNumberX +
                                                          g_uiLastMsgBoxSizeX +
                                                          (l_uiSizeX-1);
                        }
                }
        }

        g_uiLastMsgBoxPosX = par_sMessageBox->uiPosX;
        g_uiLastMsgBoxPosY = par_sMessageBox->uiPosY;
        g_uiLastMsgBoxSizeX = l_uiSizeX;
        g_uiLastMsgBoxSizeY = l_uiSizeY;

        return POSITION_OK;
}

#define INITIALIZATION_ERR_INTERNALERROR CREATION_ERR_INTERNALERROR
#define INITIALIZATION_OK                CREATION_OK
static int initMessageBox(const char * const par_pszMessage, /* IN  */
                          MessageBox_T * par_psMessageBox)   /* OUT */
{
        /* Local Variables */
        int          l_iStatus    = INITIALIZATION_OK;
        unsigned int l_uiRandomNb = 0;

        /* Analysis of the Message */
        l_iStatus = AnalyseString(par_pszMessage, &(par_psMessageBox->sMessage));
        CheckReturnedStatus(__FUNCTION__, "Message Analysis", l_iStatus);
        if (l_iStatus < ANALYSESTRING_OK)
        {
                return l_iStatus;
        }

        /* Choosing the color */
        l_uiRandomNb = (unsigned int) rand();
        par_psMessageBox->uiColor = l_uiRandomNb % (sizeof(AUTHORIZED_COLORS)/sizeof(AUTHORIZED_COLORS[0]));

        /* Setting a good position */
        l_iStatus = findNewPosition(par_psMessageBox);
        CheckReturnedStatus(__FUNCTION__, "Finding new position", l_iStatus);
        if (l_iStatus < POSITION_OK)
        {
                return l_iStatus;
        }

        return l_iStatus;
}


int CreateMessageBox(const char * const par_pszMessage) /* IN */
{
        /* Local Variables */
        MessageBox_T l_sMessageBox   = {0, 0, 0, {"", 0, 0, "", "", ""}};
        int          l_iStatus       = CREATION_OK;

        /* Initialisation */
        l_iStatus = initMessageBox(par_pszMessage, &l_sMessageBox);
        CheckReturnedStatus(__FUNCTION__, "Initialisation", l_iStatus);
        if (l_iStatus < INITIALIZATION_OK)
        {
                return l_iStatus;
        }

        /* Printing */
        l_iStatus = showMessageBox(&l_sMessageBox);
        CheckReturnedStatus(__FUNCTION__, "Printing MessageBox", l_iStatus);
        if (l_iStatus < SHOW_OK)
        {
                return l_iStatus;
        }

        return l_iStatus;
}

