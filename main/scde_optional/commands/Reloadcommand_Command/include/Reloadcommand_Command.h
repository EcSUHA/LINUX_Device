// SCDE-Command Reloadcommand

#ifndef RELOADCOMMAND_COMMAND_H
#define RELOADCOMMAND_COMMAND_H



#include "SCDE_s.h"



int Reloadcommand_InitializeCommandFn(SCDERoot_t* SCDERootptr);

struct headRetMsgMultiple_s Reloadcommand_CommandFn (const uint8_t* p_args ,const size_t args_len);
struct Head_String_s Reloadcommand_Command2Fn(const String_t args);


#endif /*RELOADCOMMAND_COMMAND_H*/
