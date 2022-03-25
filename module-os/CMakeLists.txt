project(module-os VERSION 1.0
        DESCRIPTION "OS module library")

add_library(${PROJECT_NAME} STATIC

        RTOSWrapper/ccondition_variable.cpp
        RTOSWrapper/cevent_groups.cpp
        RTOSWrapper/cmem_pool.cpp
        RTOSWrapper/cmutex.cpp
        RTOSWrapper/cqueue.cpp
        RTOSWrapper/cread_write_lock.cpp
        RTOSWrapper/csemaphore.cpp
        RTOSWrapper/ctasklet.cpp
        RTOSWrapper/cthread.cpp
        RTOSWrapper/ctickhook.cpp
        RTOSWrapper/ctimer.cpp
        RTOSWrapper/cworkqueue.cpp

        application.c
        memory/usermem.c

        CriticalSectionGuard.cpp
        LockGuard.cpp
        )

add_board_subdirectory(board)

# Board specific compilation definitions,options,include directories and features
target_compile_definitions(${PROJECT_NAME} PUBLIC ${PROJECT_CONFIG_DEFINITIONS})
target_compile_definitions(${PROJECT_NAME} PUBLIC ${PROJECT_TARGET})
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_INCLUDES})

target_compile_definitions(${PROJECT_NAME}

        PUBLIC

        CPP_FREERTOS_CONDITION_VARIABLES
        )

# enforce no optimization for RelWithDebInfo configuration to debug system memory
if (${CMAKE_BUILD_TYPE} STREQUAL "RelWithDebInfo")
    target_compile_options(${PROJECT_NAME} PRIVATE "-O0")
endif ()

if (${CMAKE_BUILD_TYPE} STREQUAL "Debug" OR ${CMAKE_BUILD_TYPE} STREQUAL "RelWithDebInfo")
    target_compile_definitions(${PROJECT_NAME} PRIVATE DEBUG_FREERTOS)
endif ()

target_include_directories(${PROJECT_NAME}

        PUBLIC

        ${CMAKE_CURRENT_SOURCE_DIR}

        RTOSWrapper/include

        )

#Add FreeRTOS SystemvView headers if enabled
if ((${PROJECT_TARGET} STREQUAL "TARGET_RT1051") AND (${SYSTEM_VIEW_ENABLED}))
    target_link_libraries(${PROJECT_NAME} PUBLIC segger::systemview)
endif ()

target_link_libraries(${PROJECT_NAME} PUBLIC log-api board-config freertos_kernel)

if (${ENABLE_TESTS})
    add_subdirectory(test)
endif ()
