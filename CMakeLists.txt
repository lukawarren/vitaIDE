cmake_minimum_required(VERSION 2.8)

# Include VitaSDK
if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  if(DEFINED ENV{VITASDK})
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VITASDK}/share/vita.toolchain.cmake" CACHE PATH "toolchain file")
  else()
    message(FATAL_ERROR "Please define VITASDK to point to your SDK path!")
  endif()
endif()

project(vitaIDE)
include("${VITASDK}/share/vita.cmake" REQUIRED)

# Project params
set(VITA_APP_NAME "vitaIDE")
set(VITA_TITLEID  "LUKA00005")
set(VITA_VERSION  "01.00")

# Build flags (no warnings for external C for sanity)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2 -DLUA_USE_C89 -g")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -O2 -std=c++17 -g")

# Source and include directories
include_directories(include/ debugscreen/ lua/)
file(GLOB SOURCES src/*.cpp src/*.c lua/*.c)

# Link directories
link_directories(${CMAKE_CURRENT_BINARY_DIR})

## Build and link
add_executable(${PROJECT_NAME}
  ${SOURCES}
  debugscreen/debugScreen.c
)

# Libraries
target_link_libraries(${PROJECT_NAME}
  SceLibKernel_stub
  SceDisplay_stub     # For debugscreen
  SceSysmodule_stub   # For loading modules
  SceNet_stub         # For networking
  SceNetCtl_stub      # For networking
  ftpvita
)

## Create Vita files
vita_create_self(${PROJECT_NAME}.self ${PROJECT_NAME})
vita_create_vpk(${PROJECT_NAME}.vpk ${VITA_TITLEID} ${PROJECT_NAME}.self
  VERSION ${VITA_VERSION}
  NAME ${VITA_APP_NAME}
  FILE sce_sys/icon0.png sce_sys/icon0.png
  FILE sce_sys/livearea/contents/bg.png sce_sys/livearea/contents/bg.png
  FILE sce_sys/livearea/contents/startup.png sce_sys/livearea/contents/startup.png
  FILE sce_sys/livearea/contents/template.xml sce_sys/livearea/contents/template.xml
)
