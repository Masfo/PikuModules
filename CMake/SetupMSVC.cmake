
function(default_msvc target)

    set_property(TARGET "${target}" PROPERTY CXX_STANDARD         23)
    set_property(TARGET "${target}" PROPERTY CXX_STANDARD_REQUIRED ON)
    set_property(TARGET "${target}" PROPERTY CXX_EXTENSIONS        OFF)

    if(UNITY_BUILD)
    message(STATUS "Unity Build for ${target}")
    set_target_properties(${target} PROPERTIES
                                    UNITY_BUILD ON
                                    UNITY_BUILD_MODE BATCH
                                    UNITY_BUILD_BATCH_SIZE 8)
    endif()



    
    set_target_properties("${target}" PROPERTIES 
                          RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/bin
                          RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/bin
                          RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_SOURCE_DIR}/bin
                     
        set_target_properties("${target}" PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
        set_target_properties("${target}" PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
    
endif()

