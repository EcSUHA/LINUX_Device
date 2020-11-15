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
 *  FName: GoodReadingName
 *  Desc: Checks the given Reading Name if it is in compliance with the reading name rules
 *        - normal letters, without special signs (A-Z, a-z)
 *        - numbers (0-9), point (.), hyphen (-), slash (/) and underscore (_)
 *  Para: const String_s nameString -> the Reading Name string that should be checked
 *  Rets: bool false -> not good ; bool true -> good reading name
 * -------------------------------------------------------------------------------------------------
 */
bool
GoodReadingName(const String_t nameString)
{

// CODE HERE IS NOT COMPLETE - ONLY FOR DEBUGGING

  return true;
}
