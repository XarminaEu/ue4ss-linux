# GenerateEmbeddedAssets.cmake
# Generates a C header file containing the embedded assets archive

function(generate_embedded_assets ASSETS_DIR OUTPUT_HEADER)
    set(ARCHIVE_PATH "${CMAKE_CURRENT_BINARY_DIR}/ue4ss_assets.tar.gz")
    
    # Create the tar.gz archive
    message(STATUS "Creating embedded assets archive...")
    execute_process(
        COMMAND ${CMAKE_COMMAND} -E tar czf ${ARCHIVE_PATH}
            UE4SS-settings.ini
            Mods
            UE4SS_Signatures
        WORKING_DIRECTORY ${ASSETS_DIR}
        RESULT_VARIABLE TAR_RESULT
    )
    
    if(NOT TAR_RESULT EQUAL 0)
        message(FATAL_ERROR "Failed to create assets archive")
    endif()
    
    # Get archive size
    file(SIZE ${ARCHIVE_PATH} ARCHIVE_SIZE)
    message(STATUS "Assets archive size: ${ARCHIVE_SIZE} bytes")
    
    # Read archive as hex
    file(READ ${ARCHIVE_PATH} ARCHIVE_HEX HEX)
    
    # Convert hex string to C array format
    string(LENGTH "${ARCHIVE_HEX}" HEX_LENGTH)
    set(C_ARRAY "")
    set(LINE_BYTES 0)
    math(EXPR BYTE_COUNT "${HEX_LENGTH} / 2")
    
    foreach(i RANGE 0 ${HEX_LENGTH} 2)
        if(i GREATER_EQUAL ${HEX_LENGTH})
            break()
        endif()
        string(SUBSTRING "${ARCHIVE_HEX}" ${i} 2 BYTE_HEX)
        if(NOT BYTE_HEX STREQUAL "")
            string(APPEND C_ARRAY "0x${BYTE_HEX},")
            math(EXPR LINE_BYTES "${LINE_BYTES} + 1")
            if(LINE_BYTES GREATER_EQUAL 16)
                string(APPEND C_ARRAY "\n    ")
                set(LINE_BYTES 0)
            endif()
        endif()
    endforeach()
    
    # Generate header file
    file(WRITE ${OUTPUT_HEADER}
"// Auto-generated embedded assets header
// DO NOT EDIT - regenerated at build time

#pragma once
#include <cstddef>

static const unsigned char embedded_assets_data[] = {
    ${C_ARRAY}
};

static const size_t embedded_assets_size = ${ARCHIVE_SIZE};
")
    
    message(STATUS "Generated embedded assets header: ${OUTPUT_HEADER}")
endfunction()
