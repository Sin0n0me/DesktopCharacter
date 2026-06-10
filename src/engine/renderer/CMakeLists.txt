cmake_minimum_required (VERSION 3.14)
project(renderer)

# 以下依存設定
set(DEPENDENCIES)
set(DEPENDENT_LOACAL_DIRS)

# ローカル
set(DEPENDENT_LOACAL_DIRS
    "../platform"
)

set(DEPENDENCIES
    glm
    platform
    engine_types
    foundation
)


# 各プラットフォームに応じた使用ライブラリの変更
if(USE_DIRECTX_11)
    target_compile_definitions(${PROJECT_NAME} PRIVATE USE_DIRECTX_11)

    set(DEPENDENCIES 
        ${DEPENDENCIES} 
        d3d11
        dxgi
        dxguid
    )
elseif(USE_DIRECTX_12)
    target_compile_definitions(${PROJECT_NAME} PRIVATE USE_DIRECTX_12)    
    
    set(DEPENDENCIES 
        ${DEPENDENCIES} 
        d3d12
        dxgi
        dxguid
    )
elseif(USE_OPENGL)
    target_compile_definitions(${PROJECT_NAME} PRIVATE USE_OPENGL)    

elseif(USE_VULKAN)
    target_compile_definitions(${PROJECT_NAME} PRIVATE USE_VULKAN)
    
    set(DEPENDENCIES 
        ${DEPENDENCIES} 
        Vulkan::Vulkan
    )
else()

endif()

#
# 以降は触らなくて良い
#

# MSVCコンパイラの場合のみ、ソースコードと実行バイナリの文字セットをUTF-8に設定
if(MSVC)
    add_compile_options("$<$<COMPILE_LANGUAGE:CXX>:/utf-8>")
    add_compile_options("$<$<COMPILE_LANGUAGE:C>:/utf-8>")
endif()

# 依存しているローカルディレクトリの追加
foreach(dir_path IN LISTS DEPENDENT_LOACAL_DIRS)
    get_filename_component(dir_name "${dir_path}" NAME)
    add_subdirectory("${dir_path}" "${CMAKE_CURRENT_BINARY_DIR}/build_${dir_name}")
endforeach()

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS 
    "src/*.h"
    "src/*.c"
    "src/*.hpp"
    "src/*.cpp"
)

add_library(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PUBLIC src)

target_link_libraries(${PROJECT_NAME} PRIVATE ${DEPENDENCIES})

set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 23
    CXX_STANDARD_REQUIRED ON
)

# MSVCコンパイラのホットリロードを有効に
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()


# 以下テスト用設定
option(BUILD_TESTING OFF)

if(BUILD_TESTING)
    enable_testing()

    # テスト用実行ファイルの生成
    file(GLOB TEST_SOURCES
        "tests/*.h"
        "tests/*.cpp" 
        "tests/*.hpp" 
        "tests/*.cpp"
    )
    add_executable(${PROJECT_NAME}_test ${SOURCES} ${TEST_SOURCES})

    # テスト用exeにメインのライブラリ(lib)をリンクする
    #target_link_libraries(tests PRIVATE my_lib)

    # CTestにテストケースを登録
    add_test(NAME RunAllTests COMMAND tests)
endif()
