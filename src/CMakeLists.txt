set(LIBBACKEND backend.cc)
set(LIBCONFMGR conf_mgr.cc)
set(LIBINSMGR instruction_manager.cc)
set(LIBVARCONST variant_const.cc)
set(LIBLOGGER logger.cc)

add_library(backend STATIC ${LIBBACKEND})
set_target_properties(backend PROPERTIES OUTPUT_NAME "backend")
add_library(confmgr STATIC ${LIBCONFMGR})
set_target_properties(confmgr PROPERTIES OUTPUT_NAME "confmgr")
add_library(insmgr STATIC ${LIBINSMGR})
set_target_properties(insmgr PROPERTIES OUTPUT_NAME "insmgr")
add_library(varconst STATIC ${LIBVARCONST})
set_target_properties(varconst PROPERTIES OUTPUT_NAME "varconst")
add_library(logger STATIC ${LIBLOGGER})
set_target_properties(logger PROPERTIES OUTPUT_NAME "logger")


add_executable(main main.cc)
target_link_libraries(main backend confmgr insmgr httpserver 
	pthread varconst log4cpp logger)
