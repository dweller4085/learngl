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
/EHsc ^
/I"lib\\mkb\\include" ^
/I"lib\\glad\\include" ^
/I"lib\\glfw\\include" ^
/I"lib\\stb\\include" ^
/I"lib\\glm\\" ^
src\*.cc ^
/Fo"bin\\" ^
/Fd"bin\\"

rem lib\glad\src\glad.c ^

link.exe ^
/nologo ^
/debug:full ^
/libpath:"lib\\glfw\\build\\src\\Debug" ^
/libpath:"lib\\glm\\build\\glm\\Debug" ^
/libpath:"lib\\stb\\bin\\" ^
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
glm.lib ^
stb_image.lib ^
bin\*.obj
