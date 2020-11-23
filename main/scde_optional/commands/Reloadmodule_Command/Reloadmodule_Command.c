/* #################################################################################################
 *
 *  Function: Reloadmodule Command for SCDE (Smart Connected Device Engine)
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



#include <ProjectConfig.h>

#if defined(ESP_PLATFORM)
#include <esp8266.h>
#include <Platform.h>
#endif // ESP_PLATFORM

//#if defined(LINUX_PLATFORM)
#include <dlfcn.h>
//#endif

#define _GNU_SOURCE         /* See feature_test_macros(7) */
#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <time.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include <SCDE_s.h>

#include "Reloadmodule_Command.h"


// -------------------------------------------------------------------------------------------------

// set default build verbose - if no external override
#ifndef RELOADMODULE_COMMAND_DBG  
#define RELOADMODULE_COMMAND_DBG  5	// 5 is default
#endif

// -------------------------------------------------------------------------------------------------

// make data root locally available
static SCDERoot_t* SCDERoot;

// make locally available from data-root: the SCDEFn (Functions / callbacks) for operation
static SCDEFn_t* SCDEFn;

// -------------------------------------------------------------------------------------------------



/*
 * -------------------------------------------------------------------------------------------------
 *  DName: Reloadmodule_Command
 *  Desc: Data 'Provided By Command' for the Reloadmodule Command (functions + infos this command provides
 *        to SCDE)
 *  Data: 
 * -------------------------------------------------------------------------------------------------
 */

// Command Help
const uint8_t Reloadmodule_helpText[] =
  "Usage: Reloadmodule <type-name>, to load the moduleread the commands from <filenname>";
// CommandHelp (detailed)
const uint8_t Reloadmodule_helpDetailText[] =
  "Usagebwrebwerb: Reloadmodule <filename>, to read the commands from <filenname>";


ProvidedByCommand_t Reloadmodule_ProvidedByCommand = {
  "Reloadmodule",					// command-name text -> libfilename.so !
  12,								// command-name text length
  Reloadmodule_InitializeCommandFn,	// Initialize Fn
  Reloadmodule_CommandFn,			// the Fn code
  { &Reloadmodule_helpText, sizeof(Reloadmodule_helpText) },
  { &Reloadmodule_helpDetailText, sizeof(Reloadmodule_helpDetailText) }
};



/* --------------------------------------------------------------------------------------------------
 *  FName: Reloadmodule_Initialize
 *  Desc: Initializion of SCDE Function Callback of an new loaded command
 *  Info: Stores Command-Information (Function Callbacks) to SCDE-Root
 *  Para: SCDERoot_t* SCDERootptr -> ptr to SCDE Data Root
 *  Rets: ? unused
 *--------------------------------------------------------------------------------------------------
 */
int 
Reloadmodule_InitializeCommandFn(SCDERoot_t* SCDERootptr)
{
  // make data root locally available
  SCDERoot = SCDERootptr;

  // make locally available from data-root: SCDEFn (Functions / callbacks) for faster operation
  SCDEFn = SCDERootptr->SCDEFn;

// --------------------------------------------------------------------------------------------------

  #if RELOADMODULE_COMMAND_DBG >= 3
  SCDEFn->Log3Fn(Reloadmodule_ProvidedByCommand.commandNameText
	,Reloadmodule_ProvidedByCommand.commandNameTextLen
	,3
	,"InitializeFn called. Now useable.");
  #endif

// --------------------------------------------------------------------------------------------------

  return 0;
}




// conversion to V2
struct headRetMsgMultiple_s
Reloadmodule_CommandFn (const uint8_t* p_args
		,const size_t args_len)
{
  // temporary conversion to make ready -> const String_t args
  String_t args;
  args.p_char = p_args;
  args.len = args_len;


// temporary conversion to make ready ->  head_ret_msg
  struct Head_String_s head_ret_msg
  	 = Reloadmodule_Command2Fn(args);


  struct headRetMsgMultiple_s x;
  STAILQ_INIT(&x);
  x.stqh_first =  head_ret_msg.stqh_first;
  x.stqh_last =  head_ret_msg.stqh_last;
  return x; 
}







/* -------------------------------------------------------------------------------------------------
 *  FName: Reloadmodule_CommandFn
 *  Desc: (Re-)Loads the Module. Given name may by an Command (Command-name) or Type (Type-name)
 *  Info: 
 *  Para: const uint8_t *argsText  -> prt to Reloadmodule command args text "module_name"
 *        const size_t argsTextLen -> attr command args text length
 *  Rets: struct headRetMsgMultiple_s -> STAILQ head of multiple retMsg, if NULL -> NO entry
 * -------------------------------------------------------------------------------------------------
 */
struct Head_String_s
Reloadmodule_Command2Fn (const String_t args)
{
  #if RELOADMODULE_COMMAND_DBG >= 7
  SCDEFn->Log3Fn(Reloadmodule_ProvidedByCommand.commandNameText
	,Reloadmodule_ProvidedByCommand.commandNameTextLen
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
  String_t module_name;

  // set * to start of possible def-spec text (seek-start-pos)
  module_name.p_char = args.p_char;

  // the total seek-counter
  int i = 0;
  
  // an element seek-counter
  int j = 0;

  // seek * to start of  def-spec text ('\32' after space)
  while( ( i < args.len ) && ( *module_name.p_char == ' ' ) ) { i++ ; module_name.p_char++ ; }

  // 1 @ finnished

  // no further arguments expected - searching the end of text
  String_t end_of_text;
	
  // set start * of possible 'end of text' seek-start-pos
  end_of_text.p_char = module_name.p_char;
	
  // clear element seek-counter
  j = 0;

  // seek to next space '\32'
  while( ( i < args.len ) && ( *end_of_text.p_char != ' ' ) ) { i++ , j++ ; end_of_text.p_char++ ; }

  // length of attr-Val text determined
  module_name.len = j;

  // @ 'p_end_of_text' ! No further processing ...

// -------------------------------------------------------------------------------------------------

  // verify lengths > 0, ATTR_VAL_LEN = 0 ALLOWED!
  if ( module_name.len == 0 ) {

	// alloc mem for retMsg
	Entry_String_t* p_entry_ret_msg =
		 malloc(sizeof(Entry_String_t));

	// response with error text
	p_entry_ret_msg->string.len = asprintf(&p_entry_ret_msg->string.p_char,
		"Error! Could not interpret '%.*s'! Usage: Reloadmodule <module-name>",
		(int) args.len,
		args.p_char);

	// insert ret_msg as entry in stail-queue
	STAILQ_INSERT_TAIL(&head_ret_msg, p_entry_ret_msg, entries);

  	// return head of singly linked tail queue, which holds 'ret_msg' elements
    return 	head_ret_msg;
  }

// -------------------------------------------------------------------------------------------------

#if defined(ESP_PLATFORM)

	// alloc mem for retMsg
	Entry_String_t* p_entry_ret_msg =
		 malloc(sizeof(Entry_String_t));

	// response with error text
	p_entry_ret_msg->string.len = asprintf(&p_entry_ret_msg->string.p_char,
		""Error!, platform has no support for loading Type-Name '%.*s'. It should be linked into the image & activated, by the maker.",
 		,module_name.len
		,module_name.p_char));

	// insert ret_msg as entry in stail-queue
	STAILQ_INSERT_TAIL(&head_ret_msg, p_entry_ret_msg, entries);
	
//#endif
   
//#if defined(LINUX_PLATFORM)// LINUX_PLATFORM
#else

  // build lib.so filename
  char *file_name;
  asprintf(&file_name
		,"/home/maikschulze/LINUX/LINUX_Device/build/release/main/lib/modules/Telnet_Module/lib%.*s_Module.so"
		,module_name.len
		,module_name.p_char);
		
  #if RELOADMODULE_COMMAND_DBG >= 7
  SCDEFn->Log3Fn(Reloadmodule_ProvidedByCommand.commandNameText
    ,Reloadmodule_ProvidedByCommand.commandNameTextLen
	,7
	,"(Re-)Loading Module with Type-Name '%.*s'."
  	,module_name.len
	,module_name.p_char);
  #endif
		
  // handle for the lib
  void *lib_handle;

  // open the module (lib) 
  lib_handle = dlopen(file_name, RTLD_NOW);
  
  // free the constructed file-name
  free(file_name);

  // open failed ?
  if (lib_handle == NULL) {
  
	// alloc mem for retMsg
	Entry_String_t* p_entry_ret_msg =
		 malloc(sizeof(Entry_String_t));

	// response with error text
	p_entry_ret_msg->string.len = asprintf(&p_entry_ret_msg->string.p_char
		,"Error! Loading Type-Name '%.*s' failed, %s."
		,module_name.len
		,module_name.p_char
		,dlerror() );

	// insert ret_msg as entry in stail-queue
	STAILQ_INSERT_TAIL(&head_ret_msg, p_entry_ret_msg, entries);

  	// return head of singly linked tail queue, which holds 'ret_msg' elements
    return 	head_ret_msg;
  }
   
// ---
  
  // build ProvidedByModule symbol-filename
  char *symbol_name;
  asprintf(&symbol_name
		,"%.*s_ProvidedByModule"
		,module_name.len
		,module_name.p_char);
  
  // Get ptr to ProvidedByModule_t. We need the function callbacks for module operation
  ProvidedByModule_t* provided_by_loaded_module;
	
  provided_by_loaded_module = dlsym(lib_handle, symbol_name);

  // free the constructed symbol-name
  free(symbol_name);

  // failed to get ?
  if (!provided_by_loaded_module ) {
  
    #if RELOADMODULE_COMMAND_DBG >= 1
    SCDEFn->Log3Fn(Reloadmodule_ProvidedByCommand.commandNameText
      ,Reloadmodule_ProvidedByCommand.commandNameTextLen
	  ,1
	  ,"CommandReloadModule",1,"Error!, accessing Type-Name '%.*s' failed, %s."
      ,module_name.len
	  ,module_name.p_char
	  ,dlerror() );
    #endif

    // unload lib to clean up
    if (dlclose(lib_handle)) {
   
      #if RELOADMODULE_COMMAND_DBG >= 1
       SCDEFn->Log3Fn(Reloadmodule_ProvidedByCommand.commandNameText
      ,Reloadmodule_ProvidedByCommand.commandNameTextLen
	  ,1
	  ,"CommandReloadModule",1,"Error!, closing Type-Name '%.*s' failed, %s."
 	  ,module_name.len
	  ,module_name.p_char
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
  provided_by_loaded_module->InitializeFn(SCDERoot);

  // store the Module (provided Fn's, and the lib handle for possible clean up)
  Module_t* new_module;
  
  new_module = (Module_t*) malloc(sizeof(Module_t));
  new_module->provided = provided_by_loaded_module;
  new_module->lib_handle = lib_handle;
  
  STAILQ_INSERT_HEAD(&SCDERoot->HeadModules, new_module, entries);

  #if RELOADMODULE_COMMAND_DBG >= 5
   SCDEFn->Log3Fn(Reloadmodule_ProvidedByCommand.commandNameText
    ,Reloadmodule_ProvidedByCommand.commandNameTextLen
    ,5
	,"Loaded Module, Type-Name '%.*s', is now useable."
  	,new_module->provided->typeNameLen
	,new_module->provided->typeName);
  #endif

#endif // LINUX_PLATFORM

 // -------------------------------------------------------------------------------------------------

  // return head of singly linked tail queue, which holds 'ret_msg' elements
  return head_ret_msg;
}



