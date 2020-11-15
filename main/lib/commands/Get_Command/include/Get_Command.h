// Command Help for SCDE

#ifndef GET_COMMAND_H
#define GET_COMMAND_H


/*
 *  SCDE Typedefs and Structures
 */
#include "SCDE_s.h"



// the initialize Fn
int Get_InitializeCommandFn(SCDERoot_t* SCDERootptr);

// the main command Fn
struct headRetMsgMultiple_s Get_CommandFn (const uint8_t *args, const size_t argsLen);



#endif /*GET_COMMAND_H*/
