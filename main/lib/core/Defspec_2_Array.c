#include "ProjectConfig.h"

#if defined(ESP_PLATFORM)
#include <esp8266.h>
#endif

#include <stddef.h>
#include <stdint.h>
#include <time.h>
#include <stdbool.h>
#include <stdlib.h>

#include "SCDE_s.h"

#include "SCDE.h"



/** Category: SCDE Core Fn
 * -------------------------------------------------------------------------------------------------
 *  FName: Device-Specifications to Array
 *  Desc: Returns all (multiple) definitions (name) that match the given devicespecification (devspec)
 *  Info: devspec should contain data (check in advance!) ; 
 *  Para: const String_t devspecString -> the devicespecification (devspec) string
 *  Rets: Head_String_s -> singly linked tail queue head which stores multiple strings
 *        (definition-names) that match the requested devicespecification (devspec),
 *        loop string entrys till STAILQ_EMPTY
 * -------------------------------------------------------------------------------------------------
 */
struct Head_String_s
Devspec2Array(const String_t devspecString)
{
 // prepare STAILQ head for multiple definitions storage
  struct Head_String_s definition_Head_String;

  // Initialize the queue head
  STAILQ_INIT(&definition_Head_String);

//---------------------------------------------------------------------------------------------------

// CODE HERE IS NOT COMPLETE - ONLY FOR DEBUGGING

  // alloc an definitionMultipleString queue element
  Entry_String_t *definition_Entry_String =
	malloc(sizeof(Entry_String_t));

  // fill string in queue element 
  definition_Entry_String->string.len =
	asprintf(&definition_Entry_String->string.p_char
		,"%.*s"
		,devspecString.len
		,devspecString.p_char);

  // insert definitionMultipleString queue element in stail-queue
  STAILQ_INSERT_TAIL(&definition_Head_String, definition_Entry_String, entries);

//---------------------------------------------------------------------------------------------------

  // return STAILQ head, stores multiple (all) matching definitions, if STAILQ_EMPTY -> no matching definitions
  return definition_Head_String;
}
