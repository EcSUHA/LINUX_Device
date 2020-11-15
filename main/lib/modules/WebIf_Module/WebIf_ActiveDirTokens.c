//##############################################################################
//###  Function: Project specific content for active directory processing
//###  ESP 8266EX SOC Activities ...
//###  (C) EcSUHA
//###  Maik Schulze, Sandfuhren 4, 38448 Wolfsburg, Germany 
//###  MSchulze780@GMAIL.COM
//###  EcSUHA - ECONOMIC SURVEILLANCE AND HOME AUTOMATION - WWW.EcSUHA.DE
//##############################################################################

#include <ProjectConfig.h>
#include <esp8266.h>
#include <Platform.h>

#include <SCDE_s.h>

#include "WebIf_Module.h"

#include "WebIf_ActiveDirTokens.h"


/*
 *--------------------------------------------------------------------------------------------------
 *FName: HTTPD_ADirDevName
 * Desc: check if the Devicename is requested (compare with SysCfgRamFlashMirrored->Ap_ssid)
 * Para: UrlProcHelper *MyUrlProcHelper -> helper for str ptr storage
 *       int NumFeat -> number of features embedded in this project
 * Rets: -1 = compared OK; -3 = not the same
 *--------------------------------------------------------------------------------------------------
 */
/*
int ICACHE_FLASH_ATTR
HTTPD_ADirDevName(UrlProcHelper *MyUrlProcHelper)
  {
  // get Devicename from current Service AP settings
  struct softap_config softap_config;
  wifi_softap_get_config (&softap_config);

  char *s = MyUrlProcHelper->pp_src;
  char *d = (char*) &softap_config.ssid;

  while ( (*s == *d) &&
		(*d != '\0') )
		{
		s++;
		d++;
		}

  MyUrlProcHelper->pp_src = s;

  if (*d == 0) return -1; // -1 = the same, indicates OK

  else return -3;	  // not the same = -3 -> error not found
  }
*/



/*
 *--------------------------------------------------------------------------------------------------
 *FName: WebIf_ActiveDirToken_FeatNr
 * Desc: Checks if an encoded number at pp_src matches the given 'Features definitons sequential modules
 *       number' (extracts the number + checks if this number matches the seq. no. of this definition from this module)
 * Para: Common_Definition_t* Common_Definition -> the definition that should be checked if it matches
 *       const char** pp_src -> ptr to requested path
 *       const char** pp_urlSeek -> ptr to implemented url-resource data
 *       int* p_result -> result data, 0-xx is the FeatNr, else -1 () ,-2  (),-3 (no match)
 * Rets: Common_Definition_t* Common_Definition -> ??? why ???
 *       updated src -> skips the number (if matched only)
 *       updated result -> result data, 0-xx is the FeatNr, else -1 () ,-2  (),-3 (no match)
 * result is the new ADID
 *--------------------------------------------------------------------------------------------------
 */
Common_Definition_t*			// e.g. http://192.168.0.56/1/SwITCH.htm (case sensitive!!)
WebIf_ActiveDirToken_FeatNr(Common_Definition_t* Common_Definition,
	const char** pp_src,
	const char** pp_urlSeek,
	int* p_result)
{

  // first extract the number at p_src to n
  char* p_s = *pp_src;

  int n = 0;

  // extract number
  while ( ( *p_s >= '0') && (*p_s <= '9') ) {

	n = n * 10 + *p_s - '0';

	p_s++;
  }

 // Fall keine Zahl ?

  #if SCDED_DBG >= 5
  printf(">Fn_ADT_FeatNr, parsed %d at pp_src,", n);
  #endif

  // 2nd check if this number matches current 'Features definitons sequential modules number'
  // start searching the current seq. number of this definition, initial cnt 0 => none defined 
  int cnt = 0;

  // loop through all definitions
  Common_Definition_t *analyzeCommon_Definition;

  STAILQ_FOREACH(analyzeCommon_Definition, &SCDERoot_at_WebIf_M->HeadCommon_Definitions, entries) {

	// cnt matching module types only ...
	if (analyzeCommon_Definition->module == Common_Definition->module) {

		// yes a match, cnt +1
		cnt++;

		// if the definition matches and its the requestet seq. number,  match !
		if ( (Common_Definition == analyzeCommon_Definition) &&
			(  n == cnt ) ) {

			#if SCDED_DBG >= 5
			printf("matches!");
			#endif	

			// correct ptr to verified position
			*pp_src = p_s;

			*p_result = cnt;	// who needs this?

			return Common_Definition;
		}
 	}
  }

  // no match -> none defined or requested numer to high !

  #if SCDED_DBG >= 5
  printf(">no match!");
  #endif	

  *p_result = -3;

  return NULL;
}



/*
 *--------------------------------------------------------------------------------------------------
 *FName: WebIf_ActiveDirToken_FeatName
 * Desc: Checks if an name at pp_src matches the given 'definiton name' of the Feature
 * Desc: check if the Featurename is requested (compare with Name[0-X])
 * Para: Common_Definition_t* Common_Definition -> the definition that should be checked if it matches
 *       const char** pp_src -> ptr to requested path
 *       const char** pp_urlSeek -> ptr to implemented url-resource data
 *       int* p_result -> result data, 0-xx is the FeatNr, else -1 () ,-2  (),-3 (no match)
 * Rets: Common_Definition_t* Common_Definition -> ??? why ???
 *       updated src -> skips the number (if matched only)
 *       updated result -> result data, 0-xx is the FeatNr, else -1 () ,-2  (),-3 (no match)
 * result is the new ADID
 *--------------------------------------------------------------------------------------------------
 */
Common_Definition_t*			//http://192.168.0.56/SSR.1.at.GPIO.13.htm
WebIf_ActiveDirToken_FeatName(Common_Definition_t* Common_Definition,
	const char** pp_src,
	const char** pp_urlSeek,
	int* p_result)
{
  char* p_s = *pp_src;
  char* p_d = (char *) Common_Definition->name;
  int dLen = Common_Definition->nameLen;

  #if SCDED_DBG >= 5
  printf(">Fn_ADT_FeatName, cmp:");
  #endif	

  while ( (*p_s == *p_d) &&
//	(  *p_d != '\0') ) {	we have a given length
	( dLen-- ) ) {

	#if SCDED_DBG >= 5
	printf("%c", *p_s);
	#endif	

	p_s++;
	p_d++;
  }

  // full length match ?
 // if (*p_d == 0) {		we have a given length
  if (!dLen) {

	#if SCDED_DBG >= 9
	printf(",match!");
	#endif	

	// correct ptr to verified position
	*pp_src = p_s;

	return Common_Definition;
  }

 // NO full length match !
  #if SCDED_DBG >= 5
  printf(",no full match!");
  #endif	

  *p_result = -3;

  return NULL;
}



/*
 *--------------------------------------------------------------------------------------------------
 *FName: WebIf_ActiveDirToken_Check
 * Desc: Checks for special character (token) at pp_urlSeek. Tokens are used to parse content from src.
 * Info: result->ActiveDirectoryID 
 *      -3 error not found,
 *      -2 empty and no error,
 *      -1 token 'ForceAFN' executed, no value -> -1 indicates to use an alternative filename !!!
 *       0-x value found 
 * Para: WebIf_HTTPDConnSlotData *conn -> ptr to current connection slot
 *       Common_Definition_t* Common_Definition -> Definition that should be checked for a match
 *       const char* pp_src -> ptr to the source -> requested url
 *       const char* pp_urlSeek -> ptr to implemented url-resource data
 *       int* p_result -> result data, 0-xx is the FeatNr, else -1 () ,-2  (),-3 (no match)
 * Rets: bool -> true will break Url match loop
 *       updated src -> skips the number (if matched only)
 *       updated result -> result data, 0-xx is the FeatNr, else -1 () ,-2  (),-3 (no match)
 *--------------------------------------------------------------------------------------------------
 */
bool
WebIf_ActiveDirToken_Check(WebIf_HTTPDConnSlotData_t* conn,
	Common_Definition_t* Common_Definition,
	const char** pp_src,
	const char** pp_urlSeek,
	int* p_result)
{
  // check if the current token at pp_urlSeek is in range of an 'Active Directory Content Token'
  if ( (**pp_urlSeek >= DevName) && (**pp_urlSeek < LAST) ) {

	// yes, load token as 'special function' and go to next char ptr++
	char SpecFunc = **pp_urlSeek;
	*pp_urlSeek = *pp_urlSeek+1;

	// Device Name ?
//	if      (SpecFunc == ADC_DevName) conn->ActiveDirID = 
//		HTTPD_ADirDevName(MyUrlProcHelper);		// -3 in case of ERROR, -1 OK


	// check: active directory content token -> Force alternative file name ?
	// results: p_result -1 if token forced an alternative file name
	if (SpecFunc == ForceAFN) {

		*p_result = -1;	// -1 -> force alternative filename

		#if SCDED_DBG >= 5
		printf(">Fn_ForceAFN");
		#endif
	}

	// check: active directory content token -> Feature Number ?
	// results: p_result -3 in case of ERROR
	if (SpecFunc == FeatNr) conn->p_entry_active_dir_matching_definition =
		WebIf_ActiveDirToken_FeatNr(Common_Definition, pp_src, pp_urlSeek, p_result);

	// check: active directory content token -> Feature Name ?
	// results: p_result -3 in case of ERROR
	if (SpecFunc == FeatureName) conn->p_entry_active_dir_matching_definition =
		WebIf_ActiveDirToken_FeatName(Common_Definition, pp_src, pp_urlSeek, p_result);

	// all others: active content xxxxxxxwith 'No. of Features Information'
	else {	

/*		// read: 'No. of Features Information'
		uint8_t HiByte = **pp_urlSeek;
		uint8_t LoByte = **pp_urlSeek;
		*pp_urlSeek = *pp_urlSeek+1;
		*pp_urlSeek = *pp_urlSeek+1
		int NumFeat = HiByte<<8 | LoByte;
*/


	}

//	if (conn->ActiveDirID >= 0) conn->ActiveDirID++; // Name 0 is 1 to the user    äNDERN?????????

	// succesfull, in range, false continues urlSeeking in httpd.c, (if ADID is not -3)
	return false;
  }

  // not successful, true breaks loop in httpd.c
  else return true;
}


