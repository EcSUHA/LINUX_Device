cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
cmake_policy(VERSION 2.6)

project(core)

add_library(core	"SCDE.c"
					"ActivateCommand.c"
					"ActivateModule.c"
					"AnalyzeCommand.c"
					"AnalyzeCommandChain.c"
					"ArgParse.c"
 					"CallGetFnByDefName.c"
 					"CommandReloadModule.c"
  					"Defspec_2_Array.c"
	 				"doGlobalDef.c"
					"FmtDateTime.c"
					"FmtTime.c"
					"Get_All_Readings.c"
					"Get_Attr_Val_By_Def_Name_And_Attr_Name.c"
					"Get_Def_And_Attr.c"
					"GetDefinitionPtrByName.c"
					"Get_Definitions_That_Match_DefSpec_String.c"
					"GetLoadedModulePtrByName.c"
					"GetTiSt.c"
					"GetUniqueTiSt.c"
					"GoodDeviceName.c"
					"GoodReadingName.c"
					"HexDumpOut.c"
					"InitSCDERoot.c"
					"Log3.c"
					"MakeDeviceName.c"
					"MakeReadingName.c"
					"TimeNow.c"
					"WriteStateFile.c")

target_include_directories(core PRIVATE "include"
										"../../include")
