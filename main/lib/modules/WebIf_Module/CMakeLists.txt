
cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
cmake_policy(VERSION 2.6)

project(WebIf_Module)

add_library(WebIf_Module
	"WebIf_Module.c"
	"Base64.c"
	"CGI_NoAuthErr.c"
	"CGI_NotFoundErr.c"
	"CGI_Redirect.c"
	"CGI_Web.c"
	"Device_WebIfTokens.c"
	"sha1.c"
	"WebIf_ActiveDirTokens.c"
	"WebIf_EspFSAdvFileTX.c"
	"WebIf_EspFSStdFileTX.c"
	"WebIf_Module_Mimes.c"
	"WebIf_Module_Schemes.c"
)

target_include_directories(WebIf_Module PRIVATE 
	"include"
	"../../../include"
	
	"ESP32_DeVICE_Moduleinclude"
)

