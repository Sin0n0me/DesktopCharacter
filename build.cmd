@echo off

cd lib\bullet
mkdir build_cmake
cd build_cmake
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
