set BUILD_DIR=%~dp0\build-w32
set GLUT_PATH="%BUILD_DIR%\..\..\lib\freeglut-MSVC-2.8.0-1.mp\freeglut"
set GLEW_PATH="%BUILD_DIR%\..\..\lib\glew-1.9.0"

mkdir %BUILD_DIR%
cd %BUILD_DIR%

if not defined CMAKE (
  set CMAKE=cmake
)

"%CMAKE%" -DCMAKE_PREFIX_PATH="%GLUT_PATH%\include" -DGLUT_ROOT_PATH="%GLUT_PATH%" -DGLEW_ROOT_PATH="%GLEW_PATH%" -G "Visual Studio 9 2008" ..

if defined VS90COMNTOOLS (
  call "%VS90COMNTOOLS%\..\..\VC\vcvarsall.bat" x86
  msbuild /target:cloth-playground /property:Configuration=Release cloth-playground.sln
)
