#
# Try to find ASSIMP library and include path.
# Once done this will define
#
# ASSIMP_FOUND
# ASSIMP_INCLUDE_PATH
# ASSIMP_LIBRARY
# 

IF (MINGW)
	FIND_PATH( ASSIMP_INCLUDE_PATH assimp/ai_assert.h
		${DEPENDENCIES_PATH}/assimp/include/
	)

    FIND_LIBRARY( ASSIMP_LIBRARY
        NAMES libassimp.dll
        PATHS
        ${DEPENDENCIES_PATH}/assimp/mingw/lib
    )

    execute_process(COMMAND ${CMAKE_COMMAND}  -E  copy_if_different
        ${DEPENDENCIES_PATH}/assimp/mingw/bin/libassimp.dll
        ${PROJECT_BINARY_DIR}/bin/libassimp.dll
    )
	
	message("Minge")

ELSEIF (MSVC)
    FIND_PATH( ASSIMP_INCLUDE_PATH assimp/ai_assert.h
        ${DEPENDENCIES_PATH}/assimp/include/
    )

       FIND_LIBRARY( ASSIMP_LIBRARY
        NAMES libassimp
        PATHS
        ${DEPENDENCIES_PATH}/assimp/msvc/lib
    )

    foreach (CONFIGURATION_TYPE ${CMAKE_CONFIGURATION_TYPES})
        execute_process(COMMAND ${CMAKE_COMMAND}  -E  copy_if_different
            ${DEPENDENCIES_PATH}/assimp/mingw/bin/libassimp.dll
            ${PROJECT_BINARY_DIR}/bin/${CONFIGURATION_TYPE}/libassimp.dll
        )
    endforeach()
    

ELSEIF(APPLE)

ENDIF()
    

SET(ASSIMP_FOUND "NO")
IF (ASSIMP_INCLUDE_PATH AND ASSIMP_LIBRARY)
	SET(ASSIMP_LIBRARIES ${ASSIMP_LIBRARY})
	SET(ASSIMP_FOUND "YES")
    message("EXTERNAL LIBRARY 'ASSIMP' FOUND")
    message("ASSIMP_LIBRARY: " ${ASSIMP_LIBRARY})
    message("ASSIMP_INCLUDE_PATH: " ${ASSIMP_INCLUDE_PATH})
ELSE()
    message("ERROR: EXTERNAL LIBRARY 'ASSIMP' NOT FOUND")
ENDIF (ASSIMP_INCLUDE_PATH AND ASSIMP_LIBRARY)