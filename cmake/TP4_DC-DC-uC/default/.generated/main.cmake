# cmake files support debug production
include("${CMAKE_CURRENT_LIST_DIR}/rule.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/file.cmake")

set(TP4_DC_DC_uC_default_library_list )

# Handle files with suffix s, for group default-XC32
if(TP4_DC_DC_uC_default_default_XC32_FILE_TYPE_assemble)
add_library(TP4_DC_DC_uC_default_default_XC32_assemble OBJECT ${TP4_DC_DC_uC_default_default_XC32_FILE_TYPE_assemble})
    TP4_DC_DC_uC_default_default_XC32_assemble_rule(TP4_DC_DC_uC_default_default_XC32_assemble)
    list(APPEND TP4_DC_DC_uC_default_library_list "$<TARGET_OBJECTS:TP4_DC_DC_uC_default_default_XC32_assemble>")
endif()

# Handle files with suffix S, for group default-XC32
if(TP4_DC_DC_uC_default_default_XC32_FILE_TYPE_assembleWithPreprocess)
add_library(TP4_DC_DC_uC_default_default_XC32_assembleWithPreprocess OBJECT ${TP4_DC_DC_uC_default_default_XC32_FILE_TYPE_assembleWithPreprocess})
    TP4_DC_DC_uC_default_default_XC32_assembleWithPreprocess_rule(TP4_DC_DC_uC_default_default_XC32_assembleWithPreprocess)
    list(APPEND TP4_DC_DC_uC_default_library_list "$<TARGET_OBJECTS:TP4_DC_DC_uC_default_default_XC32_assembleWithPreprocess>")
endif()

# Handle files with suffix [cC], for group default-XC32
if(TP4_DC_DC_uC_default_default_XC32_FILE_TYPE_compile)
add_library(TP4_DC_DC_uC_default_default_XC32_compile OBJECT ${TP4_DC_DC_uC_default_default_XC32_FILE_TYPE_compile})
    TP4_DC_DC_uC_default_default_XC32_compile_rule(TP4_DC_DC_uC_default_default_XC32_compile)
    list(APPEND TP4_DC_DC_uC_default_library_list "$<TARGET_OBJECTS:TP4_DC_DC_uC_default_default_XC32_compile>")
endif()

# Handle files with suffix cpp, for group default-XC32
if(TP4_DC_DC_uC_default_default_XC32_FILE_TYPE_compile_cpp)
add_library(TP4_DC_DC_uC_default_default_XC32_compile_cpp OBJECT ${TP4_DC_DC_uC_default_default_XC32_FILE_TYPE_compile_cpp})
    TP4_DC_DC_uC_default_default_XC32_compile_cpp_rule(TP4_DC_DC_uC_default_default_XC32_compile_cpp)
    list(APPEND TP4_DC_DC_uC_default_library_list "$<TARGET_OBJECTS:TP4_DC_DC_uC_default_default_XC32_compile_cpp>")
endif()


add_executable(${TP4_DC_DC_uC_default_image_name} ${TP4_DC_DC_uC_default_library_list})
set_target_properties(${TP4_DC_DC_uC_default_image_name} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${TP4_DC_DC_uC_default_output_dir})

target_link_libraries(${TP4_DC_DC_uC_default_image_name} PRIVATE ${TP4_DC_DC_uC_default_default_XC32_FILE_TYPE_link})

# Add the link options from the rule file.
TP4_DC_DC_uC_default_link_rule(${TP4_DC_DC_uC_default_image_name})

# Add bin2hex target for converting built file to a .hex file.
string(REGEX REPLACE [.]elf$ .hex TP4_DC_DC_uC_default_image_name_hex ${TP4_DC_DC_uC_default_image_name})
add_custom_target(TP4_DC_DC_uC_default_Bin2Hex ALL
    COMMAND ${MP_BIN2HEX} ${TP4_DC_DC_uC_default_output_dir}/${TP4_DC_DC_uC_default_image_name}
    BYPRODUCTS ${TP4_DC_DC_uC_default_output_dir}/${TP4_DC_DC_uC_default_image_name_hex}
    COMMENT Convert built file to .hex)
add_dependencies(TP4_DC_DC_uC_default_Bin2Hex ${TP4_DC_DC_uC_default_image_name})



