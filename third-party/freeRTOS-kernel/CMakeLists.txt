add_subdirectory(freeRTOS-kernel)

target_sources(freertos_kernel
        PRIVATE
        $<$<BOOL:${PROF_ON}>:${CMAKE_CURRENT_SOURCE_DIR}/freeRTOS-kernel/extensions/prof.c>
        )

target_compile_definitions(freertos_kernel
        PUBLIC
        $<$<BOOL:${PROF_ON}>:PROF_ON=${PROF_ON}>
        )

target_include_directories(freertos_kernel
        PUBLIC
        freeRTOS-kernel/extensions
        )

target_link_libraries(freertos_kernel module-os)
