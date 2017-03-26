cmake_minimum_required (VERSION 3.3)
project (ErrorLib)


SET(ERROR_LANG EN CACHE STRING "Choose error Language: DE, EN(default)")
SET_PROPERTY(CACHE ERROR_LANG PROPERTY STRINGS EN DE)

function ( findLastTag currentIndex maxLength  )
    STRING(SUBSTRING "${xmlData}" ${currentIndex} ${maxLength} sstr)
    string(FIND "${sstr}" ">" c_counter REVERSE)
    MATH(EXPR c_counter "${c_counter} + 1")
    SET(INDEX ${c_counter} PARENT_SCOPE)
endfunction ( findLastTag currentIndex maxLength  )

function( ConvertXMLToCString )
    FILE(READ errors/${ERROR_LANG}.xml xmlData)
    STRING(REGEX REPLACE "\n\n" "\n" xmlData "${xmlData}")
    STRING(REGEX REPLACE "\n\n" "\n" xmlData "${xmlData}")
    STRING(REGEX REPLACE "\n\n" "\n" xmlData "${xmlData}")
    STRING(REGEX REPLACE "\n\n" "\n" xmlData "${xmlData}")
    STRING(REGEX REPLACE "  " " " xmlData "${xmlData}")
    STRING(REGEX REPLACE "\\\\([^;])" "\\\\\\\\\\1" xmlData "${xmlData}")
    STRING(REPLACE "\"" "\\\"" xmlData "${xmlData}")
    STRING(REPLACE "\n" " \\\n" xmlData "${xmlData}")
    STRING(LENGTH "${xmlData}" length) 

    FILE( WRITE ${PROJECT_SOURCE_DIR}/xmlData.cpp "#include <string>\n")

    SET(MAX_LENGTH 1024)
    SET(STARTINDEX 0)
    SET(counter 1)

    while(TRUE)
        MATH(EXPR tmp_counter "${STARTINDEX} + ${MAX_LENGTH}")
        if ( tmp_counter GREATER length)
            SET( INDEX 1024 )
        else( tmp_counter GREATER length)
            findLastTag( ${STARTINDEX} ${MAX_LENGTH} )
        endif ( tmp_counter GREATER length)

        STRING(SUBSTRING "${xmlData}" ${STARTINDEX} ${INDEX} substr)

        FILE( APPEND ${PROJECT_SOURCE_DIR}/xmlData.cpp " \n const std::string XML_ERRORS${counter} = \"${substr}\";")
        MATH(EXPR STARTINDEX "${STARTINDEX} + ${INDEX} + 1")
        MATH(EXPR counter "${counter} + 1")

        if ( STARTINDEX GREATER length OR STARTINDEX EQUAL length )
            break()
        endif ( STARTINDEX GREATER length OR STARTINDEX EQUAL length )
    endwhile(TRUE)

    FILE( APPEND ${PROJECT_SOURCE_DIR}/xmlData.cpp " \nstd::string XML_ERRORS = XML_ERRORS1")

    MATH(EXPR counter "${counter} - 1")
    foreach(loop_var RANGE 2 ${counter} 1 )
        FILE( APPEND ${PROJECT_SOURCE_DIR}/xmlData.cpp " + XML_ERRORS${loop_var}")
    endforeach(loop_var RANGE 2 ${counter} 1 )

    FILE( APPEND ${PROJECT_SOURCE_DIR}/xmlData.cpp ";")

endfunction( ConvertXMLToCString )

ConvertXMLToCString()

FILE(GLOB SOURCE *.cpp)  
SET( COMPILE_FEATURES cxx_alias_templates cxx_auto_type)

SET( SOURCE 
    ${SOURCE}
    ../misc/StrCont.cpp
    ../factory/exceptions/classnotfoundexception.cpp
    ../factory/exceptions/nochildrenexception.cpp
    ../factory/exceptions/classnotcachedexception.cpp
    )


if (MSVC)
    add_library(libError ${SOURCE} )
    SET_TARGET_PROPERTIES(libError PROPERTIES ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
    target_compile_features(libError PRIVATE ${COMPILE_FEATURES})
elseif(UNIX)
    add_library(Error ${SOURCE} )
    SET_TARGET_PROPERTIES(Error PROPERTIES ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
    target_compile_features(Error PRIVATE ${COMPILE_FEATURES})
else(  )
    add_library(Error ${SOURCE} )
    SET_TARGET_PROPERTIES(Error PROPERTIES ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
    target_compile_features(Error PRIVATE ${COMPILE_FEATURES})
endif (MSVC)

