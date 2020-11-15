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



/**
 * -------------------------------------------------------------------------------------------------
 *  FName: InitSCDERoot
 *  Desc: Initializes the SCDERoot data structure
 *  Info: 
 *  Para: -/- 
 *  Rets: -/-
 * -------------------------------------------------------------------------------------------------
 */
void
InitSCDERoot(void) {
  SCDERoot.SCDEFn = &SCDEFn;

//  STAILQ_INIT(&SCDERoot.headAttributes);

//new:
  LIST_INIT(&SCDERoot.head_attr_name);

  STAILQ_INIT(&SCDERoot.headCommands);

  STAILQ_INIT(&SCDERoot.HeadCommon_Definitions);

  STAILQ_INIT(&SCDERoot.HeadModules);
}
