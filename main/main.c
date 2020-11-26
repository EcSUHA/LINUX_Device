
#include "ProjectConfig.h"

#include <stddef.h>
#include <stdint.h>
#include <time.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#include "SCDE_s.h"

#include "SCDE.h"



//LOG_TAG("SCDE_Main");








































/*
 * --------------------------------------------------------------------------------------------------
 *  FName: Platform_SelectQueryTask
 *  Desc: Task that selects the FD (read,write,exception) and creates read + write + exept. callbacks
 *  Para: void *pvParameters -> ?
 *  Rets: -/-
 * --------------------------------------------------------------------------------------------------
 */
static void
SelectQueryTask(void *pvParameters)
{
  Entry_Definition_t* p_entry_definition;
  int sd;

  // helper to finding highest file descriptor
  int max_fdp;

  // read socket descriptors set
  fd_set readfds;

  // write socket descriptors set
  fd_set writefds;

  // exception socket descriptors set
  fd_set exceptfds;

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  printf("debug1:%d  2:%d.\n",
	FD_SETSIZE,
	sizeof(readfds) );

  struct timeval timeout;
  // Initialize the timeout data structure
    timeout.tv_sec = 0;		// seconds;
    timeout.tv_usec = 100000;	// microseconds

  // ???
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!1
//  SCDEMux = xSemaphoreCreateRecursiveMutex();

  // ??
  int ret;
	
  printf("SCDE: startig global select loop.\n");

  // endless loop - blocked by select (depends on sets)
  while(1) {
/*
	// list currently stored modules
	printf("Selectloop: heap:%d\n",heap_caps_get_free_size(MALLOC_CAP_8BIT));

	Entry_Definition_t *Common_Definition;
	STAILQ_FOREACH(Common_Definition, &SCDERoot.HeadCommon_Definitions, entries) {
		printf("L Module:%.*s Name:%.*s FD:%d\n",
			Common_Definition->module->provided->typeNameLen,
			Common_Definition->module->provided->typeName,
			Common_Definition->nameLen,
			Common_Definition->name,
			Common_Definition->fd);
	}
*/
	// clear highest file descriptor
	max_fdp = 0;

	// clear the read socket set
	FD_ZERO (&readfds);

	// clear the write socket set
	FD_ZERO (&writefds);

	// clear the exception socket set
	FD_ZERO (&exceptfds);

	// loop through definitions with a valid socket descriptor to add to FD to sets
	STAILQ_FOREACH (p_entry_definition, &SCDERoot.HeadCommon_Definitions, entries) {

		// file descriptor / socket descriptor of this definition
		sd = p_entry_definition->fd;

		// if valid socket descriptor then may be we need to add to sets
		if ( sd != -1 ) {

/*			 os_printf("SCDE: adding %.*s fd:%d\n",
				p_entry_definition->nameLen,
				p_entry_definition->name,
				p_entry_definition->fd);
*/
			// select for reading, but only if an Fn is installed
			if (p_entry_definition->module->provided->DirectReadFn) {

				FD_SET (sd , &readfds);
			}

			// select for writing (if wants to write), but only if an Fn is installed
			if  ( (p_entry_definition->Common_CtrlRegA & F_WANTS_WRITE)
				&& (p_entry_definition->module->provided->DirectWriteFn) ) {

				FD_SET (sd , &writefds);
			}

			// select for exceptions, but only if an Fn is installed
			if (p_entry_definition->module->provided->ExceptFn) {

				FD_SET (sd , &exceptfds);
			}

			// find highest file descriptor number, need it for the select function
			// calculation after each FD_SET?
			if (sd > max_fdp) max_fdp = sd;
		}
	}

	// execute the selection
	ret = select (max_fdp + 1, &readfds, &writefds, &exceptfds, &timeout);

//	 os_printf("|SCDE: passed global select>");

	// did we get any?
	if ( ret > 0 ) {

//		 os_printf("SCDE: got any fd from select\n");
/*problem mit gelöschter definition?
		// loop through definitions with a valid socket descriptor to check and execute callbacks
		STAILQ_FOREACH(p_entry_definition, &SCDERoot.HeadCommon_Definitions, entries) {

			 os_printf("SCDE: ok fnd select\n");

			// file descriptor / socket descriptor of this definition
			sd = p_entry_definition->fd;

			// if valid socket descriptor then check and execute callbacks
			if (sd != -1) {

				// first check for write availability, but only if 'F_WANTS_WRITE' and 'DirectWriteFn' is installed
				if ( ( p_entry_definition->Common_CtrlRegA & F_WANTS_WRITE ) &&
					( p_entry_definition->module->provided->DirectWriteFn ) &&
					( FD_ISSET(sd , &writefds) ) ) {

					// clear Flag F_WANTS_WRITE, should be set again when more data should be sended
					p_entry_definition->Common_CtrlRegA &= ~F_WANTS_WRITE;

					// execute the write function
					p_entry_definition->module->provided->DirectWriteFn(p_entry_definition);
				}

				// second check for read availability. But only if a function is installed
				if ( (p_entry_definition->module->provided->DirectReadFn) &&
					(FD_ISSET(sd , &readfds)) ) {

					// execute the read function
					p_entry_definition->module->provided->DirectReadFn(p_entry_definition);
				}

				// third check for exceptions. But only if a function is installed
				if ( (p_entry_definition->module->provided->ExceptFn) &&
					(FD_ISSET(sd , &exceptfds)) ) {

					// execute the exception function
					p_entry_definition->module->provided->ExceptFn(p_entry_definition);
				}
			}
		}
*/
// --------------------------------------------------------------------------------------------------

		Entry_Definition_t* p_next_entry_definition;

		// first the write availability
//		os_printf("SCDE: check: in writeset?\n");

		p_entry_definition = STAILQ_FIRST(&SCDERoot.head_definition);

		while (p_entry_definition != NULL) {

			// load next, because current may be deleted during call
			p_next_entry_definition = STAILQ_NEXT(p_entry_definition, entries);

			// file descriptor / socket descriptor of this definition
			sd = p_entry_definition->fd;

			// if valid socket descriptor then check and execute callbacks
			if (sd != -1) {

				// first check for write availability, but only if 'F_WANTS_WRITE' and 'DirectWriteFn' is installed
				if ( ( p_entry_definition->Common_CtrlRegA & F_WANTS_WRITE ) &&
					( p_entry_definition->module->provided->DirectWriteFn ) &&
					( FD_ISSET(sd , &writefds) ) ) {

					// clear Flag F_WANTS_WRITE, should be set again when more data should be sended
					p_entry_definition->Common_CtrlRegA &= ~F_WANTS_WRITE;

					// execute the write function
					p_entry_definition->module->provided->DirectWriteFn(p_entry_definition);
				}
			}
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!1
//			esp_task_wdt_reset();

			// nexi is already buffered
			p_entry_definition = p_next_entry_definition;
		}


		// second the read availability
//		os_printf("SCDE: check: in readset?\n");

		p_entry_definition = STAILQ_FIRST(&SCDERoot.head_definition);

		while (p_entry_definition != NULL) {

			// load next, because current may be deleted during call
			p_next_entry_definition = STAILQ_NEXT(p_entry_definition, entries);

			// file descriptor / socket descriptor of this definition
			sd = p_entry_definition->fd;

			// if valid socket descriptor then check and execute callbacks
			if (sd != -1) {

				// second check for read availability. But only if a function is installed
				if ( (p_entry_definition->module->provided->DirectReadFn) &&
					(FD_ISSET(sd , &readfds)) ) {

					// execute the read function
					p_entry_definition->module->provided->DirectReadFn(p_entry_definition);
				}
			}

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//			esp_task_wdt_reset();

			// nexi is already buffered
			p_entry_definition = p_next_entry_definition;
		}

		// third the write availability
//		os_printf("SCDE: check: in exeptset?\n");

		p_entry_definition = STAILQ_FIRST(&SCDERoot.head_definition);

		while (p_entry_definition != NULL) {

			// load next, because current may be deleted during call
			p_next_entry_definition = STAILQ_NEXT(p_entry_definition, entries);

			// file descriptor / socket descriptor of this definition
			sd = p_entry_definition->fd;

			// if valid socket descriptor then check and execute callbacks
			if (sd != -1) {

				// third check for exceptions. But only if a function is installed
				if ( (p_entry_definition->module->provided->ExceptFn) &&

					(FD_ISSET(sd , &exceptfds)) ) {

					// execute the exception function
					p_entry_definition->module->provided->ExceptFn(p_entry_definition);
				}
			}

			// nexi is already buffered
			p_entry_definition = p_next_entry_definition;
		}

// --------------------------------------------------------------------------------------------------

	}


sleep(1);

//temp here 1sek cycle
	printf("\nNow IdleCbs>");

	// loop through definitions
	STAILQ_FOREACH(p_entry_definition, &SCDERoot.HeadCommon_Definitions, entries) {

		// check if F_WANTS_IDLE_TASK is set and IdleCbFn Fn is installed
		if ( (p_entry_definition->module->provided->IdleCbFn)

			&& (p_entry_definition->Common_CtrlRegA & F_WANTS_IDLE_TASK) ) {

			 printf("SCDE: Exec IdleCbFn!! Type-Name:%.*s Def-Name:%.*s FD:%d\n",
				p_entry_definition->module->provided->typeNameLen,
				p_entry_definition->module->provided->typeName,
				p_entry_definition->nameLen,
				p_entry_definition->name,
				p_entry_definition->fd);

			// clear Flag F_WANTS_WRITE, should be set again when more data should be sended
			p_entry_definition->Common_CtrlRegA &= ~F_WANTS_IDLE_TASK;

			// execute the idle callback function
			p_entry_definition->module->provided->IdleCbFn(p_entry_definition);
		}
	}
//temp here

  }
}
































/*
 *  Main entry point into the application.
 */
int main()
{



//--------------------------------------------------------------------------------------------------

  // init the SCDE-Engine (ROOT data)
  InitSCDERoot();

  // Now we can use the LogFn, say hello
  Log3( (const uint8_t*)"test!!!"
  		,4 //chck
		,1
		,"Test:%.*s\n"
		,3
//		,TAG);
		,"SCDE_Main");

//--------------------------------------------------------------------------------------------------










//--------------------------------------------------------------------------------------------------
// embedd commands A-Z

  // Activate Attr as SCDE built-in Command
  extern ProvidedByCommand_t Attr_ProvidedByCommand;
  CommandActivateCommand(&Attr_ProvidedByCommand);

  // Activate Define as SCDE built-in Command
  extern ProvidedByCommand_t Define_ProvidedByCommand;
  CommandActivateCommand(&Define_ProvidedByCommand);
  
  // Activate Deleteattr as SCDE built-in Command
  extern ProvidedByCommand_t Deleteattr_ProvidedByCommand;
  CommandActivateCommand(&Deleteattr_ProvidedByCommand);

  // Activate Delete as SCDE built-in Command
  extern ProvidedByCommand_t Delete_ProvidedByCommand;
  CommandActivateCommand(&Delete_ProvidedByCommand);
  
  // Activate Get as SCDE built-in Command
  extern ProvidedByCommand_t Get_ProvidedByCommand;
  CommandActivateCommand(&Get_ProvidedByCommand);

  // Activate Help as SCDE built-in Command
  extern ProvidedByCommand_t Help_ProvidedByCommand;
  CommandActivateCommand(&Help_ProvidedByCommand);

  // Activate Include as SCDE built-in Command
  extern ProvidedByCommand_t Include_ProvidedByCommand;
  CommandActivateCommand(&Include_ProvidedByCommand);
  
  // Activate IOWrite as SCDE built-in Command
  extern ProvidedByCommand_t IOWrite_ProvidedByCommand;
  CommandActivateCommand(&IOWrite_ProvidedByCommand);

  // Activate List as SCDE built-in Command
  extern ProvidedByCommand_t List_ProvidedByCommand;
  CommandActivateCommand(&List_ProvidedByCommand);
  
  // Activate Rename as SCDE built-in Command
  extern ProvidedByCommand_t Rename_ProvidedByCommand;
  CommandActivateCommand(&Rename_ProvidedByCommand);
  
  // Activate Rereadcfg as SCDE built-in Command
  extern ProvidedByCommand_t Rereadcfg_ProvidedByCommand;
  CommandActivateCommand(&Rereadcfg_ProvidedByCommand);
  
  // Activate Save as SCDE built-in Command
  extern ProvidedByCommand_t Save_ProvidedByCommand;
  CommandActivateCommand(&Save_ProvidedByCommand);

  // Activate Set as SCDE built-in Command
  extern ProvidedByCommand_t Set_ProvidedByCommand;
  CommandActivateCommand(&Set_ProvidedByCommand);
  
  // Activate Setstate as SCDE built-in Command
  extern ProvidedByCommand_t Setstate_ProvidedByCommand;
  CommandActivateCommand(&Setstate_ProvidedByCommand);

  // Activate Shutdown as SCDE built-in Command
  extern ProvidedByCommand_t Shutdown_ProvidedByCommand;
  CommandActivateCommand(&Shutdown_ProvidedByCommand);

// -------------------------------------------------------------------------------------------------
// embedd modules A-Z (we have no file system to load)

  // --- the required module

  // Activate Global as SCDE built-in Module
  extern ProvidedByModule_t Global_ProvidedByModule;
  CommandActivateModule(&Global_ProvidedByModule);


 // --- now the optional commands

  // Activate Reloadcommand as SCDE built-in Command
  extern ProvidedByCommand_t Reloadcommand_ProvidedByCommand;
  CommandActivateCommand(&Reloadcommand_ProvidedByCommand);

//  // Activate Reloadmodule as SCDE built-in Command
//  extern ProvidedByCommand_t Reloadmodule_ProvidedByCommand;
//  CommandActivateCommand(&Reloadmodule_ProvidedByCommand);

  // --- now the optional modules
  
  // Activate Telnet as SCDE built-in Module
//  extern ProvidedByModule_t Telnet_ProvidedByModule;
//  CommandActivateModule(&Telnet_ProvidedByModule);


// -------------------------------------------------------------------------------------------------
// initialize the global definition

  // for response msg from AnalyzeCommand
 // struct headRetMsgMultiple_s headRetMsgMultiple;

  #define CMD_4_GLOBAL "define Global Global"
//  headRetMsgMultiple = AnalyzeCommand((const uint8_t*) CMD_4_GLOBAL, 20);//sizeof(CMD_4_TELNET));

  String_t x = { "define Global Global", 20};
  struct Head_String_s head_ret_msg = AnalyzeCommand(x);
  
  
  
  // temp conversion start
  struct headRetMsgMultiple_s headRetMsgMultiple;
  STAILQ_INIT(&headRetMsgMultiple);
  headRetMsgMultiple.stqh_first =  head_ret_msg.stqh_first;
  headRetMsgMultiple.stqh_last =  head_ret_msg.stqh_last;
// temp conversion end


  // retMsgMultiple stailq filled ?
  if (!STAILQ_EMPTY(&headRetMsgMultiple)) {

	// for the retMsg elements
	strTextMultiple_t *retMsg;

	// get the entries till empty
	while (!STAILQ_EMPTY(&headRetMsgMultiple)) {

		retMsg = STAILQ_FIRST(&headRetMsgMultiple);

		Log3( (const uint8_t*)"xxx"
			 ,sizeof("xxx")
			 ,1
			 ,"result is:%.*s\n"
			 ,retMsg->strTextLen
			,retMsg->strText);

		// done, remove this entry
		STAILQ_REMOVE(&headRetMsgMultiple, retMsg, strTextMultiple_s, entries);

		// free the msg-string
		free(retMsg->strText);

		// and the strTextMultiple_t
		free(retMsg);
	}
  }

//--------------------------------------------------------------------------------------------------

  // vTaskDelay(1000 / portTICK_PERIOD_MS);
  Log("Plat_ESP32",10,"MainFn inits SCDE.");

  // Init / start SCDE Core ...
  InitA();

//  vTaskDelay(1000 / portTICK_PERIOD_MS);

//--------------------------------------------------------------------------------------------------



	puts("Hello World!");
	
	
	SelectQueryTask(NULL);
	
	

	return 0;
}
