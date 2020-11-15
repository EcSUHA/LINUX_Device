# CMakeLists.txt for this Project (needs to be customized)
# Platform: LINUX



# Set values of hypothetical variables that control the build of `foo`
set(FOO_BUILD_STATIC OFF)
set(FOO_BUILD_TESTS OFF)    



# Create and import the library targets

 # The SCDE default library targets
   include(scde_core/DefaultLibTargets.txt)
 
 # This SCDE projects additional library targets
 
  # the additional Modules
  add_subdirectory(lib/modules/Telnet_Module)
  #add_subdirectory(lib/modules/WebIf_Module) 



# Add this SCDE project executables
  add_executable(${PROJECT_NAME} "${CMAKE_CURRENT_SOURCE_DIR}/main.c"
						#		"${CMAKE_CURRENT_SOURCE_DIR}/SCDE.c"
								"${CMAKE_CURRENT_SOURCE_DIR}/logging.c")

# Set this SCDE project additional include dirs
include_directories("include")



# Link the librarys

 # The SCDE default librarys
  include(scde_core/DefaultLibsToLink.txt)
  
 # This SCDE projects additional librarys
 
  # the additional Modules
  target_link_libraries(${PROJECT_NAME} PUBLIC Telnet_Module)
  #target_link_libraries(${PROJECT_NAME} PUBLIC WebIf_Module)        


# Install the target
install(TARGETS ${PROJECT_NAME} DESTINATION ${INSTALL_DIR})



