# The following variables contains the files used by the different stages of the build process.
set(TP4_DC_DC_uC_default_default_XC32_FILE_TYPE_assemble)
set_source_files_properties(${TP4_DC_DC_uC_default_default_XC32_FILE_TYPE_assemble} PROPERTIES LANGUAGE ASM)
set(TP4_DC_DC_uC_default_default_XC32_FILE_TYPE_assembleWithPreprocess)
set_source_files_properties(${TP4_DC_DC_uC_default_default_XC32_FILE_TYPE_assembleWithPreprocess} PROPERTIES LANGUAGE ASM)
set(TP4_DC_DC_uC_default_default_XC32_FILE_TYPE_compile
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../framework/system/int/src/sys_int_pic32.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../firmware/src/app.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../firmware/src/main.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../firmware/src/system_config/default/framework/driver/adc/src/drv_adc_static.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../firmware/src/system_config/default/framework/driver/oc/src/drv_oc_mapping.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../firmware/src/system_config/default/framework/driver/oc/src/drv_oc_static.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../firmware/src/system_config/default/framework/driver/tmr/src/drv_tmr_mapping.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../firmware/src/system_config/default/framework/driver/tmr/src/drv_tmr_static.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../firmware/src/system_config/default/framework/system/clk/src/sys_clk_pic32mx.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../firmware/src/system_config/default/framework/system/devcon/src/sys_devcon.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../firmware/src/system_config/default/framework/system/devcon/src/sys_devcon_pic32mx.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../firmware/src/system_config/default/framework/system/ports/src/sys_ports_static.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../firmware/src/system_config/default/system_exceptions.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../firmware/src/system_config/default/system_init.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../firmware/src/system_config/default/system_interrupt.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../firmware/src/system_config/default/system_tasks.c")
set_source_files_properties(${TP4_DC_DC_uC_default_default_XC32_FILE_TYPE_compile} PROPERTIES LANGUAGE C)
set(TP4_DC_DC_uC_default_default_XC32_FILE_TYPE_compile_cpp)
set_source_files_properties(${TP4_DC_DC_uC_default_default_XC32_FILE_TYPE_compile_cpp} PROPERTIES LANGUAGE CXX)
set(TP4_DC_DC_uC_default_default_XC32_FILE_TYPE_link "${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../bin/framework/peripheral/PIC32MX130F064B_peripherals.a")

# The (internal) path to the resulting build image.
set(TP4_DC_DC_uC_default_internal_image_name "${CMAKE_CURRENT_SOURCE_DIR}/../../../_build/TP4_DC-DC-uC/default/default.elf")

# The name of the resulting image, including namespace for configuration.
set(TP4_DC_DC_uC_default_image_name "TP4_DC_DC_uC_default_default.elf")

# The name of the image, excluding the namespace for configuration.
set(TP4_DC_DC_uC_default_original_image_name "default.elf")

# The output directory of the final image.
set(TP4_DC_DC_uC_default_output_dir "${CMAKE_CURRENT_SOURCE_DIR}/../../../out/TP4_DC-DC-uC")
