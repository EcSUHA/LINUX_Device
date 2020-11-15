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
 *  FName: MakeReadingName
 *  Desc: Corrects the given Reading Name.Then it is in compliance with the reading name rules:
 *        - normal letters, without special signs (A-Z, a-z)
 *        - numbers (0-9), point (.), hyphen (-), slash (/) and underscore (_)
 *  Info: Characters that are not in comliance with the rules will be replaced by and underscore (_)
 *  Para: const String_s nameString -> the Reading Name string that should be corrected
 *  Rets: -/-
 * -------------------------------------------------------------------------------------------------
 */
void
MakeReadingName(const String_t nameString)
{

// CODE HERE IS NOT COMPLETE - ONLY FOR DEBUGGING

  return;
}
