@echo off

set BUILD_DIR_NAME=build_cmake

@rem bullet
cd %~dp0
cd lib\bullet
mkdir %BUILD_DIR_NAME%
cd %BUILD_DIR_NAME%
cmake ^
    -DBUILD_UNIT_TESTS=OFF ^
    -DBUILD_BULLET2_DEMOS=OFF ^
    -DBUILD_CPU_DEMOS=OFF ^
    -DBUILD_OPENGL3_DEMOS=OFF ^
    -DBUILD_PYBULLET=OFF ^
    -DBUILD_EXTRAS=OFF ^
    -DBUILD_SHARED_LIBS=OFF ^
    -DUSE_GLUT=OFF ^
    -DUSE_DOUBLE_PRECISION=OFF ^
    -DUSE_MSVC_RUNTIME_LIBRARY_DLL=OFF ^
    -DINSTALL_LIBS=ON ^
    -DBUILD_BULLET3=ON ^
    -DCMAKE_BUILD_TYPE=Release ^
    -G "Visual Studio 17 2022" ..

cmake --build . --config Release
cmake --build . --config Debug


@rem glm
cd %~dp0
cd lib\glm
mkdir %BUILD_DIR_NAME%
cd %BUILD_DIR_NAME%
cmake ^
    -DGLM_BUILD_TESTS=OFF ^
    -DBUILD_SHARED_LIBS=OFF ^
    -G "Visual Studio 17 2022" ..
    
cmake --build . --config Release
cmake --build . --config Debug


@rem imgui
cd %~dp0
cd lib\imgui
mkdir %BUILD_DIR_NAME%
cd %BUILD_DIR_NAME%
