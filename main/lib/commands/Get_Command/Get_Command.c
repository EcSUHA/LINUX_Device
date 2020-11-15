/* #################################################################################################
 *
 *  Function: Get Command for SCDE (Smart Connected Device Engine)
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

#define _GNU_SOURCE         /* See feature_test_macros(7) */
#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <time.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include <SCDE_s.h>

#include "include/Get_Command.h"



// set default build verbose - if no external override
#ifndef Get_Command_DBG  
#define Get_Command_DBG  5	// 5 is default
#endif



// -------------------------------------------------------------------------------------------------

// make data root locally available
static SCDERoot_t* SCDERoot;

// make locally available from data-root: the SCDEFn (Functions / callbacks) for faster operation
static SCDEFn_t* SCDEFn;

// -------------------------------------------------------------------------------------------------



/* --------------------------------------------------------------------------------------------------
 *  DName: Get_ProvidedByCommand
 *  Desc: Data 'Provided By Command' for this Command (functions + infos this command provides
 *        to SCDE)
 *  Data: 
 * --------------------------------------------------------------------------------------------------
 */

// Command Help
const uint8_t Get_helpText[] = 
  "Usage: Get <Def-Name> <Type dependent arguments or ?>";
// CommandHelp (detailed)
const uint8_t Get_helpDetailText[] = 
  "Usagebwrebwerb: define <name> <type> <options>, to define a device";

ProvidedByCommand_t Get_ProvidedByCommand = {
  "Get",					// Command-Name of command -> libfilename.so !
  3,						// length of cmd
  Get_InitializeCommandFn,			// Initialize Fn
  Get_CommandFn,				// the Fn code
  { &Get_helpText, sizeof(Get_helpText) },
  { &Get_helpDetailText, sizeof(Get_helpDetailText) }
};



/* --------------------------------------------------------------------------------------------------
 *  FName: Get_InitializeCommandFn
 *  Desc: Initializion of SCDE Function Callback of an new loaded command
 *  Info: Stores Command-Information (Function Callbacks) to SCDE-Root
 *  Para: SCDERoot_t* SCDERootptr -> ptr to SCDE Data Root
 *  Rets: ? unused
 *--------------------------------------------------------------------------------------------------
 */
int 
Get_InitializeCommandFn(SCDERoot_t* SCDERootptr)
{
  // make data root locally available
  SCDERoot = SCDERootptr;

  // make locally available from data-root: SCDEFn (Functions / callbacks) for faster operation
  SCDEFn = SCDERootptr->SCDEFn;

// --------------------------------------------------------------------------------------------------

  #if Get_Command_DBG >= 3
  SCDEFn->Log3Fn(Get_ProvidedByCommand.commandNameText
	,Get_ProvidedByCommand.commandNameTextLen
	,3
	,"InitializeFn called. Now useable.");
  #endif

// --------------------------------------------------------------------------------------------------

  return 0;
}



/**
  * --------------------------------------------------------------------------------------------------
  * FName: CommandGet -> gets arguments from Get 'Name Args'
  * Desc: Passes args "Args" to the GetFn of the define with name "Name"
  *
  * , and Module "TypeName" and calls Modules GetFn with
  *       args "Args"
  * Info: Get 'Name' 'Args'
  *       'Name' is a valid definition name [azAZ09._] char[31]
  *       'Args' are passed to modules GetFn for processing
  * Para: char* Args -> prt to space seperated txt string "Name TypeName Definition..."
  * Rets: char* -> error-text-string in allocated mem, or NULL=OK
  * --------------------------------------------------------------------------------------------------
  **/

/**
 * -------------------------------------------------------------------------------------------------
 *  FName: Get_Command
 *  Desc: Creates a new common define with name "Name", and Module "Type-Name", prevills some common
 *        values and calls Modules DefFn with for further module-specific initialization.
 *  Info: 'Name' is custom definition name. Allowed: [azAZ09._], uint8_t[32]
 *        'Type-Name' is module name
 *        'Definition ...' is custom and passed to modules DefineFn, stored in Definition->Definition
 *  Para: const uint8_t *args  -> prt to space seperated define text string "Name Type-Name Definition ..."
 *        const size_t argsLen -> length of args
 *  Rets: struct headRetMsgMultiple_s -> STAILQ head of multiple retMsg, if NULL -> no retMsg-entry
 * -------------------------------------------------------------------------------------------------
 */
struct headRetMsgMultiple_s
Get_CommandFn (const uint8_t *args
		,const size_t argsLen)
{
  #if Get_Command_DBG >= 7
  SCDEFn->Log3Fn(Get_ProvidedByCommand.commandNameText
	,Get_ProvidedByCommand.commandNameTextLen
	,7
	,"CommandFn called with args '%.*s'"
	,argsLen
	,args);
  #endif

// --------------------------------------------------------------------------------------------------

  // prepare STAILQ head for multiple RetMsg storage
  struct headRetMsgMultiple_s headRetMsgMultiple;

  // Initialize the queue
  STAILQ_INIT(&headRetMsgMultiple);

  // set start of possible Name
  const uint8_t *name = args;

  // set start of possible Get-Arguments
  const uint8_t *getArgs = args;

  // a seek-counter
  int i = 0;

  // seek to next space !'\32'
  while( (i < argsLen) && (*getArgs != ' ') ) {i++;getArgs++;}

  // length of Name
  size_t nameLen = i;

  // seek to start position of Get-Arguments, after '\32'
  while( (i < argsLen) && (*getArgs == ' ') ) {i++;getArgs++;}

   // length of Get-Arguments
  size_t getArgsLen = argsLen - i;

  // veryfy lengths > 0
  if ( (nameLen) == 0 || (getArgsLen == 0) ) {

	// alloc mem for retMsg
	strTextMultiple_t *retMsg =
		 malloc(sizeof(strTextMultiple_t));

	// response with error text
	retMsg->strTextLen = asprintf(&retMsg->strText
		,"Error! Could not execute command GET! Arguments '%.*s' not sufficient! Usage: Get <Def-Name> <Type dependent arguments or ?>"
		,argsLen
		,args);

	// insert retMsg in stail-queue
	STAILQ_INSERT_TAIL(&headRetMsgMultiple, retMsg, entries);

	// return STAILQ head, stores multiple retMsg, if NULL -> no retMsg-entries
	return headRetMsgMultiple;

  }

  // get the Common_Definition by Name
  Common_Definition_t *Common_Definition;

  strTextMultiple_t *retMsg = NULL;

  STAILQ_FOREACH(Common_Definition, &SCDERoot->HeadCommon_Definitions, entries) {

	if ( (Common_Definition->nameLen == nameLen)
		&& (!strncasecmp((const char*) Common_Definition->name, (const char*) name, nameLen)) ) {

		if (Common_Definition->module->provided->GetFn) {

			retMsg = Common_Definition->module->provided->GetFn(Common_Definition, getArgs, getArgsLen);

			// got an error msg?
			if (retMsg) {

				// insert retMsg in stail-queue
				STAILQ_INSERT_TAIL(&headRetMsgMultiple, retMsg, entries);

			}

		}

		else	{

			// response with error -> Type doesnt support get Fn
			// alloc mem for retMsg
			strTextMultiple_t *retMsg =
				 malloc(sizeof(strTextMultiple_t));

			// response with error text
			retMsg->strTextLen = asprintf(&retMsg->strText
				,"Error! Could not execute command GET at Definition '%.*s' with arguments '%.*s'! Type '%.*s' does not support it!"
				,Common_Definition->nameLen
				,Common_Definition->name
				,getArgsLen
				,getArgs
			   	,Common_Definition->module->provided->typeNameLen
				,Common_Definition->module->provided->typeName);

			// insert retMsg in stail-queue
		STAILQ_INSERT_TAIL(&headRetMsgMultiple, retMsg, entries);

		}

	// return STAILQ head, stores multiple retMsg, if NULL -> no retMsg-entries
	return headRetMsgMultiple;

	}

  }

  // alloc mem for retMsg
  //strTextMultiple_t *retMsg =
  retMsg =
	 malloc(sizeof(strTextMultiple_t));

  // response with error text
  retMsg->strTextLen = asprintf(&retMsg->strText
	,"Error! Could not execute command GET! Definition '%.*s' not found!"
 	,nameLen
	,name);
	
  // insert retMsg in stail-queue
  STAILQ_INSERT_TAIL(&headRetMsgMultiple, retMsg, entries);

  // return STAILQ head, stores multiple retMsg, if NULL -> no retMsg-entries
  return headRetMsgMultiple;

  }



