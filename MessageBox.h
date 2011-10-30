#ifndef _MESSAGEBOX_H_
#define _MESSAGEBOX_H_

#include <stdio.h>

#define MSGBOX_MIN_X_SIZE    ((size_t) 17) /* "#=LOGINXXX~YYYY=#" */
#define MSGBOX_MAX_X_SIZE    ((size_t) 78) /* "#============================LOGINXXX~YYYY===========================#" */
#define MSGBOX_MIN_Y_SIZE    ((size_t)  3)
#define MSGBOX_MAX_Y_SIZE    ((size_t)  5)

#define ONE_LINE_MSG_MAX_LENGTH      ((size_t) (MSGBOX_MAX_X_SIZE-2))
#define TWO_LINE_MSG_MAX_LENGTH      (2 * ONE_LINE_MSG_MAX_LENGTH)
#define THREE_LINE_MSG_MAX_LENGTH    (3 * ONE_LINE_MSG_MAX_LENGTH)
#define MSGBOX_MAX_MSG_LENGTH        ((size_t) 198)
#define MSGBOX_PADDING_SIZE          ((THREE_LINE_MSG_MAX_LENGTH - MSGBOX_MAX_MSG_LENGTH)/6)

#define LENGTHOFCONTROLCOMMAND 20

typedef char t_szOneLineArray[ONE_LINE_MSG_MAX_LENGTH + 1];
typedef char t_szMessageArray[MSGBOX_MAX_MSG_LENGTH   + 1];
typedef char t_szCommandArray[LENGTHOFCONTROLCOMMAND  + 1];

#define CREATION_OK                        0 /* Message correctly displayed */
#define CREATION_WAR_TRUNCATED             1 /* Message too long, truncated but displayed */
#define CREATION_WAR_MISWRAPPED            2 /* No solution found to wrap the message, truncated but displayed */
#define CREATION_ERR_NOT_ENOUGH_ROOM      -1 /* No room in window to display message (eg position incorrect) */
#define CREATION_ERR_WRONG_MESSAGE_FORMAT -2 /* The message format is is incorrect */
#define CREATION_ERR_INTERNALERROR        -3 /* An internal error occured */
#define CREATION_NO_OUTPUT                -4 /* No output is defined */
/* 
 * Prints a MessageBox in the current terminal
 * Returned values:
 * CREATION_OK: message correctly displayed.
 * CREATION_WAR_TRUNCATED: message too long, truncated but displayed.
 * CREATION_WAR_MISWRAPPED: no solution found to wrap the message, but displayed.
 * CREATION_ERR_NOT_ENOUGH_ROOM: no room in window to display message (eg position incorrect).
 * CREATION_ERR_WRONG_MESSAGE_FORMAT: The message format is is incorrect
 * CREATION_ERR_INTERNALERROR: An internal error occured.
 * CREATION_NO_OUTPUT: No output is defined.
 */
int CreateMessageBox(const char * const par_pszMessage);


#define MIN_LINESIZE            24
#define MIN_COLUMNSIZE          80


unsigned int setScreenWidth(unsigned int par_uiScreenWidth);
unsigned int setScreenHeight(unsigned int par_uiScreenHeight);
unsigned int getScreenWidth(void);
unsigned int getScreenHeight(void);

#endif
