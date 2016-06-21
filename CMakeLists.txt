# cmake requirements
CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

# project id
PROJECT(Tomo)

# force outut into several folders
SET (CMAKE_LIBRARY_OUTPUT_DIRECTORY
  ${PROJECT_BINARY_DIR}/bin )
SET (CMAKE_RUNTIME_OUTPUT_DIRECTORY
  ${PROJECT_BINARY_DIR}/bin )
SET (CMAKE_ARCHIVE_OUTPUT_DIRECTORY
  ${PROJECT_BINARY_DIR}/bin )
#SET (CMAKE_CURRENT_BINARY_DIR
#  ${PROJECT_BINARY_DIR}/moc )
SET (CMAKE_PROJECT_DOC_DIR
  ${PROJECT_BINARY_DIR}/doc )


# requirements
FIND_PACKAGE(Qt4 REQUIRED)

#SET(TOMO_NO_GUI YES)
IF(TOMO_NO_GUI MATCHES YES)
  ADD_DEFINITIONS("-DTOMO_NO_GUI")
  SET(tomo_src 
    ./src/app/tomo.cpp 
    ./src/app/PluginLoader.cpp 
    #./src/app/Toolchain_query.cpp 
    )
ELSE()
  # qt4
  SET(QT_USE_QTOPENGL TRUE)
  INCLUDE(${QT_USE_FILE})
  FIND_PACKAGE(Qt4 REQUIRED)
  
  AUX_SOURCE_DIRECTORY(./src/app tomo_cpp_files)
  SET(tomo_headers
        ../TbdLib/include/tbd/qt/ParameterForm.h
        ../TbdLib/include/tbd/qt/ColorButton.h
        ../TbdLib/include/tbd/qt/ConfigTree.h
        ./src/app/MainWindow.h 
        ./src/app/widget/ToolLibrary.h
        ./src/app/widget/ToolchainNavigator.h
        ./src/app/widget/ToolPreview.h
        ./src/app/widget/ToolLibraryTree.h
        ./src/app/widget/ToolchainEditor.h 
        ./src/app/widget/ToolchainTreeModel.h 
        ./src/app/widget/ToolchainComboBox.h 
        ./src/app/widget/ToolchainThumbnail.h 
        ./src/app/widget/VerticalScrollArea.h 
        ./src/app/widget/ToolchainTree.h 
        ./src/app/widget/ElementForm.h 
        ./src/app/widget/ToolchainGL.h 
        ./src/app/widget/ColorRange.h 
        ./src/app/widget/Rotation.h 
        ./src/app/widget/Range.h 
        ./src/app/widget/Vector.h 
        ) 
  SET(tomo_forms   
    ./src/app/ToolLibrary.ui
    ./src/app/ToolchainNavigator.ui
    ./src/app/ElementForm.ui
    ./src/app/Range.ui
    ./src/app/Vector.ui
    ./src/app/Rotation.ui
    ./src/app/MainWindow.ui)
  SET(tomo_resources ./src/app/style.qrc)
  QT4_WRAP_CPP(tomo_headers_moc ${tomo_headers})
  QT4_ADD_RESOURCES(tomo_resources_rcc ${tomo_resources})
  ADD_DEFINITIONS(${QT_DEFINITIONS})
  QT4_WRAP_UI(tomo_forms_headers ${tomo_forms})
  SET(tomo_src 
    ../TbdLib/src/ParameterForm.cpp
    ../TbdLib/src/ColorButton.cpp
    ../TbdLib/src/ConfigTree.cpp
    ${tomo_cpp_files} 
    ${tomo_headers_moc} 
    ${tomo_forms_headers} 
    ${tomo_resources_rcc})
ENDIF()

IF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  SET(Boost_INCLUDE_DIR ../boost_1_54_0)
  SET(Boost_LIBRARY_DIR ../boost_1_54_0/stage/lib)
  SET(OperatingSystem "Mac OS X")
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin") 

IF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")

  SET(Boost_INCLUDE_DIR ../boost_1_54_0)
  SET(Boost_LIBRARY_DIR ../boost_1_54_0/stage/lib)

  SET(OperatingSystem "Linux")
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")

find_package(Boost 1.54 COMPONENTS program_options system filesystem thread REQUIRED)

IF (Boost_FOUND)
  cmake_policy(SET CMP0015 NEW)
  cmake_policy(SET CMP0003 OLD)
  include_directories(${Boost_INCLUDE_DIR})
  ADD_DEFINITIONS( "-DHAS_BOOST" )
  # Qt Fix on Ubuntu 12.04 / osx works fine with it as well
  ADD_DEFINITIONS( "-DBOOST_TT_HAS_OPERATOR_HPP_INCLUDED")
  ADD_DEFINITIONS( "-DBOOST_PP_VARIADICS " ) 
ENDIF()

ADD_DEFINITIONS("-DDEBUG")
INCLUDE_DIRECTORIES(. ${CMAKE_CURRENT_BINARY_DIR} ./include ../TbdLib/include ../GEx/include ./src/app )

IF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  INCLUDE_DIRECTORIES(/usr/include/eigen3)
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")

IF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  INCLUDE_DIRECTORIES(/usr/local/include/eigen3)
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin") 

INCLUDE(${CMAKE_CURRENT_SOURCE_DIR}/src/toolset/CMakeLists.txt)
toolset_build(fdm)
toolset_build(generic)

# OpenGL
IF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
   INCLUDE_DIRECTORIES ( /System/Library/Frameworks )
   FIND_LIBRARY(OpenGL_LIBRARY OpenGL )
   FIND_LIBRARY(GLUT_LIBRARY GLUT )
   SET(EXTRA_LIBS ${COCOA_LIBRARY} ${GLUT_LIBRARY} ${OpenGL_LIBRARY})
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin") 

IF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  SET(OpenGL_LIBRARY GL GLU GLEW )
  SET(GLUT_LIBRARY glut )
  SET(EXTRA_LIBS ${OpenGL_LIBRARY} ${GLUT_LIBRARY} dl) 
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
# Slicer
ADD_EXECUTABLE(tomo ${tomo_src} )



link_directories(${Boost_LIBRARY_DIR})
TARGET_LINK_LIBRARIES(tomo ${Boost_LIBRARIES} ${QT_LIBRARIES} ${EXTRA_LIBS} ${QT_QTOPENGL_LIBRARY} )

ADD_EXECUTABLE(ToolchainTest ./src/test/ToolchainTest.cpp ./src/app/PluginLoader.cpp)
TARGET_LINK_LIBRARIES(ToolchainTest ${Boost_LIBRARIES} ${EXTRA_LIBS})


IF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  ADD_DEFINITIONS(-g -Os -fsigned-char -Wall -fPIC -std=c++11)
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")

IF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  #ADD_DEFINITIONS(-g -Os -fsigned-char -Wall -fPIC)
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -stdlib=libc++ -mmacosx-version-min=10.8 -O2 -arch x86_64 -Wno-unused-variable -DNDEBUG -DQT_NO_DEBUG -DQT_SVG_LIB -DQT_XML_LIB -DQT_OPENGL_LIB -DQT_GUI_LIB -DQT_CORE_LIB -DQT_HAVE_SSE -DQT_HAVE_MMXEXT -DQT_HAVE_SSE2 -DQT_SHARED")
  SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -std=c++0x -stdlib=libc++ -mmacosx-version-min=10.8 -arch x86_64")
  SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wno-unused-variable -std=c++0x -stdlib=libc++ -mmacosx-version-min=10.8 -arch x86_64")
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin") 

ADD_DEFINITIONS(-ferror-limit=5 -fcolor-diagnostics -fdiagnostics-show-template-tree)

# add a target to generate API documentation with Doxygen
FIND_PACKAGE(Doxygen)
IF(DOXYGEN_FOUND)
  CONFIGURE_FILE(${CMAKE_PROJECT_DOC_DIR}/tomo.doxygen ${CMAKE_PROJECT_DOC_DIR} @ONLY)
  ADD_CUSTOM_TARGET(doc
    ${DOXYGEN_EXECUTABLE} ${CMAKE_PROJECT_DOC_DIR}/tomo.doxygen
    WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
    COMMENT "Generating API documentation with Doxygen (see ./doc/html/index.html)" VERBATIM
    )
ENDIF(DOXYGEN_FOUND)
