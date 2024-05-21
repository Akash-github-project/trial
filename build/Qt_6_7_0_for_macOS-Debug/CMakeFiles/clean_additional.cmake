# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles/video_player_testing_autogen.dir/AutogenUsed.txt"
  "CMakeFiles/video_player_testing_autogen.dir/ParseCache.txt"
  "video_player_testing_autogen"
  )
endif()
