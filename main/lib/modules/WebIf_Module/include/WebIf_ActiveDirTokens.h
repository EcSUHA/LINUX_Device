#ifndef WebIf_ActiveDirTokens
#define WebIf_ActiveDirTokens

// enum for active directory content (tokens)
enum ActiveDirContent {			// CMDs (reserved range 0xf0-0xff)
    DevName = 0xf0			// #0xf0 
  , FeatNr				// #0xf1 extract ADID of feature from number
  , ForceAFN				// #0xf2 no parse Fn, indicates in result (-1) that an alternative filename should be used
  , FeatureName				// #0xf3
  , LAST				// #0x??
};



// checks and parses WebIf Active-Drectory-Tokens to get an ADID (Active Directory ID) for feature adressing
bool WebIf_ActiveDirToken_Check(WebIf_HTTPDConnSlotData_t* conn, Common_Definition_t* Common_Definition, const char** SrcPtr, const char** UrlSeekPtr, int* tokenExecResult);

#endif //WebIf_ActiveDirTokens

