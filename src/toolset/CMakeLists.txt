PROJECT(tomo_fdm_plugins)



FUNCTION(toolset_build toolset)
# Find all the input files
  SET(plugin_dir "${CMAKE_CURRENT_SOURCE_DIR}/src/toolset/${toolset}")

  FILE(GLOB plugin_src RELATIVE "${plugin_dir}"
      "${plugin_dir}/*.cpp")
  MESSAGE(STATUS "Build plugins for toolset ${toolset} in ${plugin_dir}")

  FOREACH(plugin_rel_filename ${plugin_src})
    SET(plugin_filename "${plugin_dir}/${plugin_rel_filename}")
    SET(plugin_name "${toolset}_${plugin_rel_filename}")
    STRING(REGEX REPLACE ".cpp" "" plugin_name "${plugin_name}")
  
    SET_SOURCE_FILES_PROPERTIES(${plugin_filename} -DTOMO_PLUGIN)
    MESSAGE(STATUS "Build plugin: ${plugin_name} ${plugin_filename}")
    ADD_LIBRARY(${plugin_name} SHARED ${plugin_filename})
    TARGET_LINK_LIBRARIES(${plugin_name} ${Boost_LIBRARIES} ${OpenGL_LIBRARY})

  ENDFOREACH()

  SET(test_dir ${plugin_dir}/test)

  FILE(GLOB test_src RELATIVE "${test_dir}"
      "${test_dir}/*.cpp")
  MESSAGE(STATUS "Build tests toolset plugins ${toolset} in ${test_dir}")

  FOREACH(test_rel_filename ${test_src})
    SET(test_filename "${test_dir}/${test_rel_filename}")
    SET(test_name "${toolset}_test_${test_rel_filename}")
    STRING(REGEX REPLACE ".cpp" "" test_name "${test_name}")

    SET_SOURCE_FILES_PROPERTIES(${test_filename} -DTOMO_TEST)
    MESSAGE(STATUS "Build test: ${test_name} ${test_filename}")
    ADD_EXECUTABLE(${test_name} ${test_filename})
    TARGET_LINK_LIBRARIES(${test_name} ${Boost_LIBRARIES}  ${OpenGL_LIBRARY})
  ENDFOREACH()

ENDFUNCTION()


