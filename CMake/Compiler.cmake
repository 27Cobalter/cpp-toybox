set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# add_compile_definitions(-DNDEBUG)

if(NOT CMAKE_CXX_COMPILER_ID MATCHES "MSVC") # GNU or Clang
      if (CMAKE_SYSTEM_PROCESSOR MATCHES "^(x86_64|amd64)$")
            string(APPEND CMAKE_CXX_FLAGS " -march=x86-64-v3")
      elseif (CMAKE_SYSTEM_PROCESSOR MATCHES "^(aarch64|arm64)$")
            string(APPEND CMAKE_CXX_FLAGS " -march=armv8-a")
      endif()

      string(APPEND CMAKE_CXX_FLAGS "")
      string(APPEND CMAKE_CXX_FLAGS_DEBUG " -O0")
      string(APPEND CMAKE_CXX_FLAGS_RELWITHDEBINFO " -O1")
      string(APPEND CMAKE_CXX_FLAGS_RELEASE " -O3")
endif()

if(NOT MSVC) # GNU like
      string(APPEND CMAKE_CXX_FLAGS " -pipe -fno-plt -fvisibility=hidden -Wl,-z,relro -Wl,-z,now -Wl,-z,noexecstack -Wl,-Bsymbolic")
      string(APPEND CMAKE_CXX_FLAGS_DEBUG " -g3 -fstack-protector-strong -fstack-clash-protection -D_GLIBCXX_ASSERTIONS -D_LIBCPP_ASSERT -fno-omit-frame-pointer -fno-optimize-sibling-calls -fno-common")
      string(APPEND CMAKE_CXX_FLAGS_RELWITHDEBINFO " -g3 -fno-stack-clash-protection -fno-stack-protector -fomit-frame-pointer -foptimize-sibling-calls -fcommon -flto=auto -U_FORTIFY_SOURCE -D_FORTIFY_SOURCE=0")
      string(APPEND CMAKE_CXX_FLAGS_RELEASE " -g0 -fno-stack-clash-protection -fno-stack-protector -fomit-frame-pointer -foptimize-sibling-calls -fcommon -flto=auto -U_FORTIFY_SOURCE -D_FORTIFY_SOURCE=0 -s")

      # string(APPEND CMAKE_CXX_LINKER_FLAGS " -Wl,-z,relro -Wl,-z,now -Wl,-z,noexecstack -Wl,-Bsymbolic")
      # string(APPEND CMAKE_CXX_LINKER_FLAGS_DEBUG "")
      # string(APPEND CMAKE_CXX_LINKER_FLAGS_RELWITHDEBINFO "")
      # string(APPEND CMAKE_CXX_LINKER_FLAGS_RELEASE " -flto=auto -s")

      if(UNIX)
      endif()
endif()
