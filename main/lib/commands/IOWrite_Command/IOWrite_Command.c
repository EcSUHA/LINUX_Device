/* #################################################################################################
 *
 *  Function: IOWrite Command for SCDE (Smart Connected Device Engine)
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

#include "include/IOWrite_Command.h"



// set default build verbose - if no external override
#ifndef IOWrite_Command_DBG  
#define IOWrite_Command_DBG  5	// 5 is default
#endif



// -------------------------------------------------------------------------------------------------

// make data root locally available
static SCDERoot_t* SCDERoot;

// make locally available from data-root: the SCDEFn (Functions / callbacks) for faster operation
static SCDEFn_t* SCDEFn;

// -------------------------------------------------------------------------------------------------



/* --------------------------------------------------------------------------------------------------
 *  DName: IOWrite_ProvidedByCommand
 *  Desc: Data 'Provided By Command' for this Command (functions + infos this command provides
 *        to SCDE)
 *  Data: 
 * --------------------------------------------------------------------------------------------------
 */

// Command Help
const uint8_t IOWrite_helpText[] = 
  "Usage: iowrite <DevSpec> <data in hex[0-9|A-F]>, to write data to the IOWriteFn of definitions matching the DevSpec.";
// CommandHelp (detailed)
const uint8_t IOWrite_helpDetailText[] = 
  "Usagebwrebwerb: iowrite <DevSpec> <data in hex[0-9|A-F]>, to write data to the IOWriteFn of definitions matching the DevSpec.";

ProvidedByCommand_t IOWrite_ProvidedByCommand = {
  "IOWrite",					// Command-Name of command -> libfilename.so !
  7,						// length of cmd
  IOWrite_InitializeCommandFn,			// Initialize Fn
  IOWrite_CommandFn,				// the Fn code
  { &IOWrite_helpText, sizeof(IOWrite_helpText) },
  { &IOWrite_helpDetailText, sizeof(IOWrite_helpDetailText) }
};



/* --------------------------------------------------------------------------------------------------
 *  FName: IOWrite_InitializeCommandFn
 *  Desc: Initializion of SCDE Function Callback of an new loaded command
 *  Info: Stores Command-Information (Function Callbacks) to SCDE-Root
 *  Para: SCDERoot_t* SCDERootptr -> ptr to SCDE Data Root
 *  Rets: ? unused
 *--------------------------------------------------------------------------------------------------
 */
int 
IOWrite_InitializeCommandFn(SCDERoot_t* SCDERootptr)
{

  // make data root locally available
  SCDERoot = SCDERootptr;

  // make locally available from data-root: SCDEFn (Functions / callbacks) for faster operation
  SCDEFn = SCDERootptr->SCDEFn;

// --------------------------------------------------------------------------------------------------

  #if IOWrite_Command_DBG >= 3
  SCDEFn->Log3Fn(IOWrite_ProvidedByCommand.commandNameText
	,IOWrite_ProvidedByCommand.commandNameTextLen
	,3
	,"InitializeFn called. Now useable.");
  #endif

// --------------------------------------------------------------------------------------------------

  return 0;
}





/* --------------------------------------------------------------------------------------------------
 *  FName: IOWrite_Command
 *  Desc: Writes <data> to <DevSpec> matching Definitions. First the HEX data is converted and stored
 *        in allocated memory. This pointer to the data in allocated memory is send to the IOWriteFn. 
 *  Note: IOWriteFn has to free the allocated memory. 
 *  Info: <DevSpec>  [azAZ09._], uint8_t[32]
 *        <data in hex[0-9|A-F]> is the data that should be written. (hex-dump)
 *  Para: const xString_t argsString -> IOWrite arguments string  
 *  Rets: struct Head_String_s -> singly linked tail queue head to store multiple 
 *                                            return message strings. Loop entrys till STAILQ_EMPTY!
 * -------------------------------------------------------------------------------------------------
 */
struct headRetMsgMultiple_s  //new Head_String_s
IOWrite_CommandFn (const uint8_t *args
		,const size_t argsLen) //new const String_t argsString)
{
  #if IOWrite_Command_DBG >= 7
  SCDEFn->Log3Fn(IOWrite_ProvidedByCommand.commandNameText
	,IOWrite_ProvidedByCommand.commandNameTextLen
	,7
	,"CommandFn called with args '%.*s'"
	,argsLen
	,args);
  #endif

// --------------------------------------------------------------------------------------------------

// temporary conversion to make ready -> const xString_s argsString
  String_t argsString;
  argsString.p_char = args;
  argsString.len = argsLen;

// -------------------------------------------------------------------------------------------------

  // first seek-counter
  int i = 0;

  // stores extracted Devspec-String
  String_t devspecString;

  // start seeking at Arguments-String *
  devspecString.p_char = 
	argsString.p_char;

  // seek * to start of Devspec-String (after space  '\32')
  while( (i < argsString.len) && (*devspecString.p_char == ' ') )
	{ i++ ; devspecString.p_char++ ; }

  // stores extracted HEX-String
  String_t hexString;

  // start seeking at beginning of Devspec-String *
  hexString.p_char = 
	devspecString.p_char;

  // second seek-counter
  int j = 0;

  // seek * to the end of Devspec-String (before space '\32' !)
  while( (i < argsString.len) && (*hexString.p_char != ' ') )
	{ i++ ; j++ ; hexString.p_char++ ; }

  // length of Devspec-String determined
  devspecString.len = j;

  // seek * to start of HEX-String (after space  '\32')
  while( (i < argsString.len) && (*hexString.p_char == ' ') )
	{ i++ ; hexString.p_char++ ; }

  // length of HEX-String determined (assuming: rest of the args)
  hexString.len = argsString.len - i;

// -------------------------------------------------------------------------------------------------

  // prepare singly linked tail queue head for multiple Return-Message storage
  struct Head_String_s retMsg_Head_String;

  // Initialize the SLTQ
  STAILQ_INIT(&retMsg_Head_String);

// -------------------------------------------------------------------------------------------------

  // check usage by verifying lengths
  if ( ( devspecString.len == 0 ) || ( hexString.len == 0 ) ) {

	// alloc mem for retMsg
	Entry_String_t *retMsg_Entry_String =
	malloc(sizeof(Entry_String_t));

	// response with error text
	retMsg_Entry_String->string.len = 
		asprintf(&retMsg_Entry_String->string.p_char
			,"Command IOWrite could not interpret args '%.*s'!"
			 " Usage: IOWrite <DevSpec> <Data>"
			,argsString.len
			,argsString.p_char);

	// insert retMsg in stail-queue
	STAILQ_INSERT_TAIL(&retMsg_Head_String, retMsg_Entry_String, entries);

  	// return singly linked tail queue head, which holds multiple linked retMsg strings

//-------
struct headRetMsgMultiple_s x;
STAILQ_INIT(&x);
x.stqh_first = retMsg_Head_String.stqh_first;
x.stqh_last = retMsg_Head_String.stqh_last;

  	return x; // retMsg_Head_String;   
  }

// -------------------------------------------------------------------------------------------------

  // get all Definitions which meet devspec in an SLTQ
  struct Head_String_s definition_Head_String =
  	SCDEFn->Devspec2ArrayFn(devspecString);

// -------------------------------------------------------------------------------------------------

  // start processing the definition queue elements
  Entry_String_t *definition_Entry_String = 
	STAILQ_FIRST(&definition_Head_String);

  // loop through the found Definitions - till the queue is empty
  while ( definition_Entry_String != NULL ) {

  	// search the definition for the requested Definition-Name
  	Common_Definition_t *Common_Definition = 
		STAILQ_FIRST(&SCDERoot->HeadCommon_Definitions);

	// loop through the definitions
 	while ( Common_Definition != NULL ) {

		if ( ( Common_Definition->nameLen == definition_Entry_String->string.len )
			&& ( !strncasecmp((const char*) Common_Definition->name
				,(const char*) definition_Entry_String->string.p_char
				, definition_Entry_String->string.len) ) ) {

			// found matching Definition, break and keep current
			break;
		}

		// get next Common_Definition to process it
		Common_Definition = STAILQ_NEXT(Common_Definition, entries);
 	 }

// -------------------------------------------------------------------------------------------------

	// Definition for the given Definition-Name NOT found
  	if ( Common_Definition == NULL ) {

		// alloc mem for retMsg
		Entry_String_t *retMsg_Entry_String =
			malloc(sizeof(Entry_String_t));

		// response with error retMsg
		retMsg_Entry_String->string.len = 
			asprintf(&retMsg_Entry_String->string.p_char
				,"Please define <defName>: %.*s first!"
				,definition_Entry_String->string.len
				,definition_Entry_String->string.p_char);

		// insert retMsg in stail-queue
		STAILQ_INSERT_TAIL(&retMsg_Head_String, retMsg_Entry_String, entries);

		// -> continue processing the 'definitions that meet devspec' loop
  	}

// -------------------------------------------------------------------------------------------------

 	// Definition for the given Definition-Name found
  	else {


/*
	size_t hexs2bin(const char *hex, size_t len , unsigned char **out)

	const char    *a = "Test 123! - jklmn";
	char          *hex;
	unsigned char *bin;
	size_t         binlen;

	hex = bin2hex((unsigned char *)a, strlen(a));
	printf("%sn", hex);

	binlen = hexs2bin(hex, &bin);
	printf("%.*sn", (int)binlen, (char *)bin);

	free(bin);
	free(hex);
	return 0;
*/



		String_t hexData;
		hexData.p_char = NULL;//(uint8_t*) "Maiktest";
		hexData.len = 8;



		// if provided by Type -> call IOWrite Fn to send the pointer
		if (Common_Definition->module->provided->WriteFn) {

			#if IOWrite_Command_DBG >= 7
			SCDEFn->Log3Fn(IOWrite_ProvidedByCommand.commandNameText
		  		,IOWrite_ProvidedByCommand.commandNameTextLen
				,7
				,"Calling IOWriteFn of Module '%.*s' for Definition '%.*s'. "
				 "Writing  %d bytes."
				,Common_Definition->module->provided->typeNameLen
				,Common_Definition->module->provided->typeName
				,Common_Definition->nameLen
				,Common_Definition->name
				,hexData.len);
			#endif

			// call Modules IOWriteFn. Interpret retMsg_Entry_String != NULL as veto !
			Entry_String_t *retMsg_Entry_String =
				Common_Definition->module->provided->WriteFn(Common_Definition
					,hexData);

			// singly linked tail queue element holding an return message? Insert it in queue.
			if (retMsg_Entry_String) 
				STAILQ_INSERT_TAIL(&retMsg_Head_String
					,retMsg_Entry_String, entries);

		// no veto implemented here
		}
	}

// -------------------------------------------------------------------------------------------------

	// backup queue element - to free mem later
	Entry_String_t *oldMultipleStringSLTQE =
		definition_Entry_String;

	// get next definition from definition(s) STAIL Queue
	definition_Entry_String = 
		STAILQ_NEXT(definition_Entry_String, entries);

	// free charancters in xString_t
	free(oldMultipleStringSLTQE->string.p_char);

	// free STAIL Queue element Entry_String_t
	free(oldMultipleStringSLTQE);
  }

// -------------------------------------------------------------------------------------------------

//-------
struct headRetMsgMultiple_s x;
STAILQ_INIT(&x);
x.stqh_first = retMsg_Head_String.stqh_first;
x.stqh_last = retMsg_Head_String.stqh_last;

  	// return STAILQ head, stores multiple retMsg, if NULL -> no retMsg-entries
  	return x; // retMsg_Head_String;   
}





































/*

//https://nachtimwald.com/2017/09/24/hex-encode-and-decode-in-c/
int main(int argc, char **argv)
{
	const char    *a = "Test 123! - jklmn";
	char          *hex;
	unsigned char *bin;
	size_t         binlen;

	hex = bin2hex((unsigned char *)a, strlen(a));
	printf("%sn", hex);

	binlen = hexs2bin(hex, &bin);
	printf("%.*sn", (int)binlen, (char *)bin);

	free(bin);
	free(hex);
	return 0;
}






int hexchr2bin(const char hex, char *out)
{
  if (out == NULL)
	return 0;

  if (hex >= '0' && hex <= '9') {

	*out = hex - '0';
  } else if (hex >= 'A' && hex <= 'F') {

	*out = hex - 'A' + 10;
  } else if (hex >= 'a' && hex <= 'f') {

	*out = hex - 'a' + 10;
  } else {
	return 0;
  }
  return 1;
}



size_t hexs2bin(const char *hex, size_t len , unsigned char **out)
{
  char   b1;
  char   b2;
  size_t i;

  if (hex == NULL || *hex == '\0' || out == NULL)
	return 0;

  if (len % 2 != 0) 
	return 0;

  len /= 2;

  *out = malloc(len);

  memset(*out, 'A', len);

  for ( i=0 ; i<len ; i++ ) {

	if (!hexchr2bin(hex[i*2], &b1) || !hexchr2bin(hex[i*2+1], &b2)) {

		return 0;
	}

	(*out)[i] = (b1 << 4) | b2;
  }
  return len;
}

*/













