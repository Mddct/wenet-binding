cmake_minimum_required(VERSION 3.15 FATAL_ERROR)

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}
  )


set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
set(LIBRARY_OUTPUT_DIRECTORY, ${CMAKE_BINARY_DIR}/bin)
set(ARCHIVE_OUTPUT_DIRECTORY, ${CMAKE_BINARY_DIR}/bin)


project(wenet_python)




## https://stackoverflow.com/questions/9742003/platform-detection-in-cmake
#if(WIN32)
#  macro(get_WIN32_WINNT version)
#    if(CMAKE_SYSTEM_VERSION)
#      set(ver ${CMAKE_SYSTEM_VERSION})
#      string(REGEX MATCH "^([0-9]+).([0-9])" ver ${ver})
#      string(REGEX MATCH "^([0-9]+)" verMajor ${ver})
#      # Check for Windows 10, b/c we'll need to convert to hex 'A'.
#      if("${verMajor}" MATCHES "10")
#        set(verMajor "A")
#        string(REGEX REPLACE "^([0-9]+)" ${verMajor} ver ${ver})
#      endif()
#      # Remove all remaining '.' characters.
#      string(REPLACE "." "" ver ${ver})
#      # Prepend each digit with a zero.
#      string(REGEX REPLACE "([0-9A-Z])" "0\\1" ver ${ver})
#      set(${version} "0x${ver}")
#    endif()
#  endmacro()
#  get_WIN32_WINNT(ver)
#  add_definitions(-D_WIN32_WINNT=${ver})
#endif()

add_subdirectory(wenet_wrapper/interface/wenet/runtime/server/x86)
if(NOT MSVC)
  link_directories(${openfst_PREFIX_DIR}/lib)
else()
  link_directories(${boost_SOURCE_DIR}/stage/lib)
endif()

# TODO: use boost from wenet
FetchContent_Declare(boost
  URL      https://boostorg.jfrog.io/artifactory/main/release/1.75.0/source/boost_1_75_0.tar.gz
  URL_HASH SHA256=aeb26f80e80945e82ee93e5939baebdca47b9dee80a07d3144be1e1a6a66dd6a
  )
FetchContent_MakeAvailable(boost)
include_directories(${boost_SOURCE_DIR})
link_directories(${boost_SOURCE_DIR}/stage/lib)

find_package(Torch REQUIRED)
message("wenet_runtime_binding CMakeLists: ${TORCH_CXX_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${TORCH_CXX_FLAGS} -DC10_USE_GLOG")

add_library(runtime_wrapper SHARED
  wenet_wrapper/interface/runtime_wrapper.cc
  )
target_include_directories(runtime_wrapper PUBLIC
  wenet_wrapper/interface/wenet/runtime/server/x86
  wenet_wrapper/interface/wenet/runtime/server/x86/kaldi
  ${openfst_SOURCE_DIR}/src/include
  ${gflags_BINARY_DIR}/include
  ${glog_SOURCE_DIR}/src ${glog_BINARY_DIR}
  )
target_link_libraries(runtime_wrapper
  frontend
  decoder
  )

add_subdirectory(python/pybind11)
pybind11_add_module(_pywrap_wenet MODULE
  python/py_asr_wrapper.cc
  )

target_link_libraries(_pywrap_wenet PRIVATE
  runtime_wrapper
  )

# hard code
# TODO: passing variable
function(mv_wrap_lib)
  set(libtorch_SOURCE_DIR "../wenet_wrapper/interface/wenet/runtime/server/x86/fc_base/libtorch-src/")
  set(openfst_SOURCE_DIR "..//wenet_wrapper/interface/wenet/runtime/server/x86/fc_base/openfst-build/")
  set(python_LIB_DIR "../python/wenet/lib")

  add_custom_command(TARGET _pywrap_wenet  POST_BUILD
    # COMMAND cp ${libtorch_SOURCE_DIR}/lib/libtorch_cpu.so lib
    # COMMAND cp ${libtorch_SOURCE_DIR}/lib/libtorch.so lib
    # COMMAND cp ${libtorch_SOURCE_DIR}/lib/libc10.so lib
    # COMMAND cp ${libtorch_SOURCE_DIR}/lib/libgomp-52f2fd74.so.1 lib
    # COMMAND cp ${libtorch_SOURCE_DIR}/lib/libgomp-52f2fd74.so.1 lib
    # COMMAND cp -r ${openfst_SOURCE_DIR}/src/lib/.libs/libfst.so.8 lib
    COMMAND rm -rf  ${python_LIB_DIR}/*.so
    COMMAND cp _pywrap_wenet*.so ${python_LIB_DIR}
    COMMAND cp libruntime_wrapper*.so ${python_LIB_DIR}}

    COMMAND cp -L ${libtorch_SOURCE_DIR}/lib/libtorch_cpu.so ${python_LIB_DIR}
    COMMAND cp -L ${libtorch_SOURCE_DIR}/lib/libtorch.so ${python_LIB_DIR}
    COMMAND cp -L ${libtorch_SOURCE_DIR}/lib/libc10.so ${python_LIB_DIR}
    COMMAND cp -L ${libtorch_SOURCE_DIR}/lib/libgomp-52f2fd74.so.1 ${python_LIB_DIR}
    COMMAND cp -L ${libtorch_SOURCE_DIR}/lib/libgomp-52f2fd74.so.1 ${python_LIB_DIR}
    COMMAND cp  ${openfst_SOURCE_DIR}/src/lib/.libs/libfst.so* ${python_LIB_DIR}

    )
endfunction()
mv_wrap_lib()
