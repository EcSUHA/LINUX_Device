/* #################################################################################################
 *
 *  Function: Reloadcommand Command for SCDE (Smart Connected Device Engine)
 *
 *  ESP 8266EX & ESP32 SOC Activities ...
 *  Copyright by EcSUHA
 *
 *  Created by Maik Schulze, Sandfuhren 4, 38448 Wolfsburg, Germany for EcSUHA.de 
 *
 *  MSchulze780@GMAIL.COM
 *  EcSUHA - ECONOMIC SURVEILLANCE AND HOME AUTOMATION - WWW.EcSUHA.DE
 * #################################################################################################
 */

// PLATFORM LINUX ONLY Command!

#include <ProjectConfig.h>

#define _GNU_SOURCE         /* See feature_test_macros(7) */
#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <time.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>

#include <SCDE_s.h>

#include "Reloadcommand_Command.h"


// -------------------------------------------------------------------------------------------------

// set default build verbose - if no external override
#ifndef RELOADCOMMAND_COMMAND_DBG  
#define RELOADCOMMAND_COMMAND_DBG  5	// 5 is default
#endif

// -------------------------------------------------------------------------------------------------

// make data root locally available
static SCDERoot_t* SCDERoot;

// make locally available from data-root: the SCDEFn (Functions / callbacks) for operation
static SCDEFn_t* SCDEFn;

// -------------------------------------------------------------------------------------------------



/*
 * -------------------------------------------------------------------------------------------------
 *  DName: Reloadcommand_Command
 *  Desc: Data 'Provided By Command' for the Reloadcommand Command (functions + infos this command provides
 *        to SCDE)
 *  Data: 
 * -------------------------------------------------------------------------------------------------
 */

// Command Help
const uint8_t Reloadcommand_helpText[] =
  "Usage: Reloadcommand <command-name>, to load the moduleread the commands from <filenname>";
// CommandHelp (detailed)
const uint8_t Reloadcommand_helpDetailText[] =
  "Usagebwrebwerb: Reloadcommand <command-name>, to read the commands from <filenname>";


ProvidedByCommand_t Reloadcommand_ProvidedByCommand = {
  "Reloadcommand",					// command-name text -> libfilename.so !
  13,								// command-name text length
  Reloadcommand_InitializeCommandFn,	// Initialize Fn
  Reloadcommand_CommandFn,			// the Fn code
  { &Reloadcommand_helpText, sizeof(Reloadcommand_helpText) },
  { &Reloadcommand_helpDetailText, sizeof(Reloadcommand_helpDetailText) }
};



/* --------------------------------------------------------------------------------------------------
 *  FName: Reloadcommand_Initialize
 *  Desc: Initializion of SCDE Function Callback of an new loaded command
 *  Info: Stores Command-Information (Function Callbacks) to SCDE-Root
 *  Para: SCDERoot_t* SCDERootptr -> ptr to SCDE Data Root
 *  Rets: ? unused
 *--------------------------------------------------------------------------------------------------
 */
int 
Reloadcommand_InitializeCommandFn(SCDERoot_t* SCDERootptr)
{
  // make data root locally available
  SCDERoot = SCDERootptr;

  // make locally available from data-root: SCDEFn (Functions / callbacks) for faster operation
  SCDEFn = SCDERootptr->SCDEFn;

// --------------------------------------------------------------------------------------------------

  #if RELOADCOMMAND_COMMAND_DBG >= 3
  SCDEFn->Log3Fn(Reloadcommand_ProvidedByCommand.commandNameText
	,Reloadcommand_ProvidedByCommand.commandNameTextLen
	,3
	,"InitializeFn called. Now useable.");
  #endif

// --------------------------------------------------------------------------------------------------

  return 0;
}




// conversion to V2
struct headRetMsgMultiple_s
Reloadcommand_CommandFn (const uint8_t* p_args
		,const size_t args_len)
{
  // temporary conversion to make ready -> const String_t args
  String_t args;
  args.p_char = p_args;
  args.len = args_len;


// temporary conversion to make ready ->  head_ret_msg
  struct Head_String_s head_ret_msg
  	 = Reloadcommand_Command2Fn(args);


  struct headRetMsgMultiple_s x;
  STAILQ_INIT(&x);
  x.stqh_first =  head_ret_msg.stqh_first;
  x.stqh_last =  head_ret_msg.stqh_last;
  return x; 
}







/* -------------------------------------------------------------------------------------------------
 *  FName: Reloadcommand_CommandFn
 *  Desc: (Re-)Loads the Module. Given name may by an Command (Command-name) or Type (Type-name)
 *  Info: 
 *  Para: const uint8_t *argsText  -> prt to Reloadcommand command args text "command_name"
 *        const size_t argsTextLen -> attr command args text length
 *  Rets: struct headRetMsgMultiple_s -> STAILQ head of multiple retMsg, if NULL -> NO entry
 * -------------------------------------------------------------------------------------------------
 */
struct Head_String_s
Reloadcommand_Command2Fn (const String_t args)
{
  #if RELOADCOMMAND_COMMAND_DBG >= 7
  SCDEFn->Log3Fn(Reloadcommand_ProvidedByCommand.commandNameText
	,Reloadcommand_ProvidedByCommand.commandNameTextLen
	,7
	,"CommandFn called with args '%.*s'"
	,args.len
	,args.p_char);
  #endif

// --------------------------------------------------------------------------------------------------

  // prepare STAILQ head to store multiple 'ret_msg' elements
  struct Head_String_s head_ret_msg;

  // Initialize the queue
  STAILQ_INIT(&head_ret_msg);

// --------------------------------------------------------------------------------------------------

  // expected argument #1
  String_t command_name;

  // set * to start of possible def-spec text (seek-start-pos)
  command_name.p_char = args.p_char;

  // the total seek-counter
  int i = 0;
  
  // an element seek-counter
  int j = 0;

  // seek * to start of  def-spec text ('\32' after space)
  while( ( i < args.len ) && ( *command_name.p_char == ' ' ) ) { i++ ; command_name.p_char++ ; }

  // 1 @ finnished

  // no further arguments expected - searching the end of text
  String_t end_of_text;
	
  // set start * of possible 'end of text' seek-start-pos
  end_of_text.p_char = command_name.p_char;
	
  // clear element seek-counter
  j = 0;

  // seek to next space '\32'
  while( ( i < args.len ) && ( *end_of_text.p_char != ' ' ) ) { i++ , j++ ; end_of_text.p_char++ ; }

  // length of attr-Val text determined
  command_name.len = j;

  // @ 'p_end_of_text' ! No further processing ...

// -------------------------------------------------------------------------------------------------

  // verify lengths > 0, ATTR_VAL_LEN = 0 ALLOWED!
  if ( command_name.len == 0 ) {

	// alloc mem for retMsg
	Entry_String_t* p_entry_ret_msg =
		 malloc(sizeof(Entry_String_t));

	// response with error text
	p_entry_ret_msg->string.len = asprintf(&p_entry_ret_msg->string.p_char,
		"Error! Could not interpret '%.*s'! Usage: Reloadcommand <command-name>",
		(int) args.len,
		args.p_char);

	// insert ret_msg as entry in stail-queue
	STAILQ_INSERT_TAIL(&head_ret_msg, p_entry_ret_msg, entries);

  	// return head of singly linked tail queue, which holds 'ret_msg' elements
    return 	head_ret_msg;
  }

// -------------------------------------------------------------------------------------------------

  // build lib.so filename
  char *file_name;
  asprintf(&file_name
		,"/home/maikschulze/LINUX/LINUX_Device/build/release/main/scde_optional/commands/Reloadmodule_Command/lib%.*s_Command.so"
		,command_name.len
		,command_name.p_char);
		
  #if RELOADCOMMAND_COMMAND_DBG >= 7
  SCDEFn->Log3Fn(Reloadcommand_ProvidedByCommand.commandNameText
    ,Reloadcommand_ProvidedByCommand.commandNameTextLen
	,7
	,"(Re-)Loading Commmand '%.*s'."
  	,command_name.len
	,command_name.p_char);
  #endif
		
  // handle for the lib
  void *lib_handle;

  // open the module (lib) 
  lib_handle = dlopen(file_name, RTLD_NOW);
  
  // free the constructed file-name
  free(file_name);

  // open failed ?
  if (!lib_handle) {
  
	// alloc mem for retMsg
	Entry_String_t* p_entry_ret_msg =
		 malloc(sizeof(Entry_String_t));

	// response with error text
	p_entry_ret_msg->string.len = asprintf(&p_entry_ret_msg->string.p_char
		,"Error! Loading Command '%.*s' failed, %s."
		,command_name.len
		,command_name.p_char
		,dlerror() );

	// insert ret_msg as entry in stail-queue
	STAILQ_INSERT_TAIL(&head_ret_msg, p_entry_ret_msg, entries);

  	// return head of singly linked tail queue, which holds 'ret_msg' elements
    return 	head_ret_msg;
  }
   
// ---
  
  // build ProvidedByCommand symbol-filename
  char *symbol_name;
  asprintf(&symbol_name
		,"%.*s_ProvidedByCommand"
		,command_name.len
		,command_name.p_char);
  
  // Get ptr to ProvidedByCommand_t. We need the function callbacks for module operation
  ProvidedByCommand_t* provided_by_loaded_command;
	
  provided_by_loaded_command = dlsym(lib_handle, symbol_name);

  // free the constructed symbol-name
  free(symbol_name);

  // failed to get ?
  if (!provided_by_loaded_command ) {
  
    #if RELOADCOMMAND_COMMAND_DBG >= 1
    SCDEFn->Log3Fn(Reloadcommand_ProvidedByCommand.commandNameText
      ,Reloadcommand_ProvidedByCommand.commandNameTextLen
	  ,1
	  ,"CommandReloadcommand",1,"Error!, accessing Type-Name '%.*s' failed, %s."
      ,command_name.len
	  ,command_name.p_char
	  ,dlerror() );
    #endif

    // unload lib to clean up
    if (dlclose(lib_handle)) {
   
      #if RELOADCOMMAND_COMMAND_DBG >= 1
       SCDEFn->Log3Fn(Reloadcommand_ProvidedByCommand.commandNameText
      ,Reloadcommand_ProvidedByCommand.commandNameTextLen
	  ,1
	  ,"CommandReloadcommand",1,"Error!, closing Type-Name '%.*s' failed, %s."
 	  ,command_name.len
	  ,command_name.p_char
      ,dlerror() );
      #endif
    }
     
  	// return with result: not found

  // generae msg?

  // return head of singly linked tail queue, which holds 'ret_msg' elements
  return head_ret_msg;
  }

// ---

  // call loaded modules initialize function
  provided_by_loaded_command->InitializeCommandFn(SCDERoot);

  // store the Module (provided Fn's, and the lib handle for possible clean up)
  Entry_Command_t* new_command;
  
  new_command = (Entry_Command_t*) malloc(sizeof(Entry_Command_t));
  new_command->provided = provided_by_loaded_command;
  new_command->lib_handle = lib_handle;
  
  STAILQ_INSERT_HEAD(&SCDERoot->head_command, new_command, entries);

  #if RELOADCOMMAND_COMMAND_DBG >= 5
   SCDEFn->Log3Fn(Reloadcommand_ProvidedByCommand.commandNameText
    ,Reloadcommand_ProvidedByCommand.commandNameTextLen
    ,5
	,"Loaded Module, Type-Name '%.*s', is now useable."
  	,new_command->provided->commandNameTextLen
	,new_command->provided->commandNameText);
  #endif

 // -------------------------------------------------------------------------------------------------

  // return head of singly linked tail queue, which holds 'ret_msg' elements
  return head_ret_msg;
}



