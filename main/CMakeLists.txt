# CMakeLists.txt for this Project (needs to be customized)
# Platform: LINUX

project (LINUX_Device)

# Set values of hypothetical variables that control the build of `foo`
set(FOO_BUILD_STATIC OFF)
set(FOO_BUILD_TESTS OFF)    

# the project configuration
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# the SCDE public headers
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/scde_core/include)


add_subdirectory(scde_core)
add_subdirectory(lib) #scde_optional)






# Add this SCDE_Devices project executables
add_executable(${PROJECT_NAME} "${CMAKE_CURRENT_SOURCE_DIR}/main.c"
						#		"${CMAKE_CURRENT_SOURCE_DIR}/SCDE.c"
								"${CMAKE_CURRENT_SOURCE_DIR}/logging.c")
						
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)



# Link the librarys

 # The SCDE Default Librarys
 
  # the Core
  target_link_libraries(${PROJECT_NAME} PRIVATE SCDE_CORE  ${CMAKE_DL_LIBS})
  
  # the embedded Commands
  target_link_libraries(${PROJECT_NAME} PRIVATE Attr_Command)
  target_link_libraries(${PROJECT_NAME} PRIVATE Define_Command)
  target_link_libraries(${PROJECT_NAME} PRIVATE Delete_Command)
  target_link_libraries(${PROJECT_NAME} PRIVATE Deleteattr_Command)
  target_link_libraries(${PROJECT_NAME} PRIVATE Get_Command)
  target_link_libraries(${PROJECT_NAME} PRIVATE Help_Command)
  target_link_libraries(${PROJECT_NAME} PRIVATE Include_Command)
  target_link_libraries(${PROJECT_NAME} PRIVATE IOWrite_Command)
  target_link_libraries(${PROJECT_NAME} PRIVATE List_Command)
  target_link_libraries(${PROJECT_NAME} PRIVATE Rename_Command)
  target_link_libraries(${PROJECT_NAME} PRIVATE Rereadcfg_Command)
  target_link_libraries(${PROJECT_NAME} PRIVATE Save_Command)
  target_link_libraries(${PROJECT_NAME} PRIVATE Set_Command)
  target_link_libraries(${PROJECT_NAME} PRIVATE Setstate_Command)
  target_link_libraries(${PROJECT_NAME} PRIVATE Shutdown_Command)
  
  # the embedded Modules																						
  target_link_libraries(${PROJECT_NAME} PRIVATE Global_Module)
 
  # the additional Commands
   
  # the additional Modules
  #target_link_libraries(${PROJECT_NAME} PRIVATE Telnet_Module)
  #target_link_libraries(${PROJECT_NAME} PRIVATE WebIf_Module)        



# Install the target
install(TARGETS ${PROJECT_NAME} DESTINATION ${INSTALL_DIR})



