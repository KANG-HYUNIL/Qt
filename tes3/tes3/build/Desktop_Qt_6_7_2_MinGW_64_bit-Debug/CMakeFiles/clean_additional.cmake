# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\tes3_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\tes3_autogen.dir\\ParseCache.txt"
  "tes3_autogen"
  )
endif()
