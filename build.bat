@echo off

set name="learngl"

mkdir bin

cl.exe ^
/nologo ^
/c ^
/std:c++20 ^
/TP ^
/Od ^
/Zi ^
/MDd ^
/I"lib\\mkb\\include" ^
/I"lib\\glad\\include" ^
/I"lib\\glfw\\include" ^
lib\glad\src\glad.c ^
src\*.cc ^
/Fo"bin\\" ^
/Fd"bin\\"

link.exe ^
/nologo ^
/debug:full ^
/libpath:"lib\glfw\build\src\Debug" ^
/machine:x64 ^
/pdb:"bin\\%name%.pdb" ^
/out:"bin\\%name%.exe" ^
/subsystem:console ^
msvcrtd.lib ^
vcruntimed.lib ^
shell32.lib ^
kernel32.lib ^
gdi32.lib ^
user32.lib ^
opengl32.lib ^
glfw3.lib ^
bin\*.obj
