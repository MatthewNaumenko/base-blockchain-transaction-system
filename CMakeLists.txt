cmake_minimum_required(VERSION 3.12)

project(transaction VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF) 

add_definitions(-DPROJECT_ROOT="${CMAKE_SOURCE_DIR}")

# Обязательно прописать свой путь к OpenSSL!!!
set(OPENSSL_ROOT_DIR "D:/openssl-3.4.1")

find_package(OpenSSL REQUIRED)
if(NOT OpenSSL_FOUND)
    message(FATAL_ERROR "OpenSSL not found. Please install OpenSSL and set OPENSSL_ROOT_DIR.")
else()
    message(STATUS "OpenSSL Include Dir: ${OPENSSL_INCLUDE_DIR}")
    message(STATUS "OpenSSL Libraries: ${OPENSSL_LIBRARIES}")
    message(STATUS "OpenSSL Version: ${OPENSSL_VERSION}")
endif()

add_executable(BlockchainSystem
    src/BC_Utilities.cpp    
    src/BC_CryptoUtils.cpp
    src/BC_Transaction.cpp
    src/BC_Block.cpp
    src/BC_Blockchain.cpp
    src/BC_RSAKeyGenerator.cpp  
    src/BC_KeyManager.cpp
    src/BC_Controller.cpp
    src/main.cpp
)

target_include_directories(BlockchainSystem PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${OPENSSL_INCLUDE_DIR} 
)

target_link_libraries(BlockchainSystem
    PRIVATE OpenSSL::SSL
    PRIVATE OpenSSL::Crypto
)


if(MSVC)
    # Для MSVC (Visual Studio)
    target_compile_options(BlockchainSystem PRIVATE
        /W4         # Высокий уровень предупреждений
        /sdl        # Безопасные функции времени выполнения
        /guard:cf   # Защита от атак контроля потока
    )
else()
    # Для GCC/Clang
    target_compile_options(BlockchainSystem PRIVATE
        -Wall       # Все предупреждения
        -Wextra     # Дополнительные предупреждения
        -pedantic   # Строгая проверка стандартов
    )
endif()

set_target_properties(BlockchainSystem PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    INTERPROCEDURAL_OPTIMIZATION_RELEASE ON
)

message(STATUS "Building project: ${PROJECT_NAME} version ${PROJECT_VERSION}")
message(STATUS "Executable target: BlockchainSystem")