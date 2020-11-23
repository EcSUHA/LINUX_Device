// SCDE-Command Reloadmodule

#ifndef RELOADMODULE_COMMAND_H
#define RELOADMODULE_COMMAND_H



#include "SCDE_s.h"



int Reloadmodule_InitializeCommandFn(SCDERoot_t* SCDERootptr);

struct headRetMsgMultiple_s Reloadmodule_CommandFn (const uint8_t* p_args ,const size_t args_len);
struct Head_String_s Reloadmodule_Command2Fn(const String_t args);


#endif /*RELOADMODULE_COMMAND_H*/
