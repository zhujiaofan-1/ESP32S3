set(INCLUDES "./ui/custom" "./ui/generated" "./ui/generated/guider_customer_fonts")
file(GLOB_RECURSE SOURCES ./*.c)

idf_component_register(
    SRCS
        ${SOURCES}
        
    INCLUDE_DIRS
        "."
        "iot"
        "lvgl"
        ${INCLUDES}
)
