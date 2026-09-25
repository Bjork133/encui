cmake_minimum_required(VERSION 3.14)

project(EncEncryptor LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find dependencies
find_package(OpenSSL REQUIRED)
find_path(SODIUM_INCLUDE_DIR sodium.h REQUIRED)
find_library(SODIUM_LIBRARY sodium REQUIRED)
find_package(Curses REQUIRED)

# GoogleTest (fetch if not installed)
include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.tar.gz
)
# For Windows: prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)


# Main executable
add_executable(encui
    src/main.cpp
    src/CryptoEngine.cpp
    src/AES.cpp
    src/ChaCha20.cpp
    src/RSA.cpp
    src/CustomCipher.cpp
)

target_include_directories(encui PRIVATE ${OPENSSL_INCLUDE_DIR} ${CURSES_INCLUDE_DIR} ${PROJECT_SOURCE_DIR}/src ${PROJECT_SOURCE_DIR}/include)

target_link_libraries(encui PRIVATE OpenSSL::SSL OpenSSL::Crypto ${SODIUM_LIBRARY} ${CURSES_LIBRARY})

# Unit tests (GoogleTest)
option(ENABLE_TESTS "Build tests" OFF)
if(ENABLE_TESTS)
    enable_testing()
    find_package(GTest QUIET)
    if(GTest_FOUND)
        add_executable(enc_tests
            tests/main_test.cpp
            src/CryptoEngine.cpp
            src/AES.cpp
            src/RSA.cpp
            src/ChaCha20.cpp
            src/CustomCipher.cpp
        )
        target_include_directories(enc_tests PRIVATE ${OPENSSL_INCLUDE_DIR} ${SODIUM_INCLUDE_DIR})
        target_link_libraries(enc_tests PRIVATE OpenSSL::SSL OpenSSL::Crypto sodium GTest::gtest GTest::gtest_main)
        add_test(NAME enc_tests COMMAND enc_tests)
    else()
        message(WARNING "GTest not found, tests will not be built")
    endif()
endif()
