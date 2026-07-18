# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "debug_GCC_STM32C5A3RGT6")
  file(REMOVE_RECURSE
  "STM32C5A3R_Project1.elf"
  "STM32C5A3R_Project1.map"
  )
endif()
