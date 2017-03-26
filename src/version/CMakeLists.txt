cmake_minimum_required (VERSION 3.2)
project (VersionString)

function( WriteToVersion )
    FILE( WRITE ${PROJECT_SOURCE_DIR}/version.cpp "const char * VERSION_NUMBER = \"VERSION ${ISEAFrameVERSION_MAJOR}.${ISEAFrameVERSION_MINOR} ${ISEAFrameNAME} ${ISEAFrameDATE} ${REPO_VERSION}\\n\"; ")
    SET(UNINCLUDE "CIFS" "DOXYGEN" "UNINCLUDE" "MY_INFO" "SG_Filter" "SOURCE" "SUBDIRECTORIES" "Subversion" "unittest" "CXXTEST" "HEADERS" "SOURCE" "_boost_" "ISEAFrame_BINARY_DIR" "CMAKE_BINARY_DIR" "CMAKE_CACHEFILE_DIR" "CMAKE_CURRENT_BINARY_DIR" "CMAKE_CURRENT_LIST_DIR" "CMAKE_CURRENT_LIST_FILE" "CMAKE_HOME_DIRECTORY" "CMAKE_PARENT_LIST_FILE" "CMAKE_PLATFORM_INFO_DIR" "FIND_PACKAGE_MESSAGE_DETAILS_CxxTest" "FIND_PACKAGE_MESSAGE_DETAILS_Doxygen" "ISEAFrame_BINARY_DIR" "PROJECT_BINARY_DIR" "Boost_ERROR_REASON" "ARGC" "ARGN" "ARGV" "CMAKE_MATCH_0" "CMAKE_SYSTEM_PREFIX_PATH" "WIN32" "_Boost_LIB_VERSION_REGEX" "_Doxygen_GRAPHVIZ_BIN_DIRS" "CMAKE_CXX_COMPILER_ID_PLATFORM_CONTENT" )
    get_cmake_property(_variableNames VARIABLES)
    LIST( REMOVE_DUPLICATES _variableNames )
    foreach (_variableName ${_variableNames})
        foreach(g ${UNINCLUDE})
            string(FIND ${_variableName} ${g} tmp_found REVERSE)
            if ( NOT ${tmp_found} EQUAL -1)
                break()
            endif( NOT ${tmp_found} EQUAL -1)
        endforeach(g ${UNINCLUDE})

        if ( ${tmp_found} EQUAL -1 )
            string(MAKE_C_IDENTIFIER "${${_variableName}}" c_string)
            FILE( APPEND ${PROJECT_SOURCE_DIR}/version.cpp "\nconst char * ${_variableName} = \"${_variableName}: ${c_string}\\n\";")
        endif ( ${tmp_found} EQUAL -1 )
    endforeach()

endfunction( WriteToVersion )

WriteToVersion()

FILE(GLOB SOURCE *.cpp)
SET( COMPILE_FEATURES cxx_alias_templates cxx_auto_type)

SET( SOURCE
    ${SOURCE}
    )

if (MSVC)
    add_library(libVersion ${SOURCE} )
    SET_TARGET_PROPERTIES(libVersion PROPERTIES ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
    target_compile_features(libVersion PRIVATE ${COMPILE_FEATURES})
elseif(UNIX)
    add_library(Version ${SOURCE} )
    SET_TARGET_PROPERTIES(Version PROPERTIES ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
    target_compile_features(Version PRIVATE ${COMPILE_FEATURES})
else(  )
    add_library(Version ${SOURCE} )
    SET_TARGET_PROPERTIES(Version PROPERTIES ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
    target_compile_features(Version PRIVATE ${COMPILE_FEATURES})
endif (MSVC)
