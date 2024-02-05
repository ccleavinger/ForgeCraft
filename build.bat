@echo off
Setlocal EnableDelayedExpansion

cd assets\shaders\

for /r %%i in (*) do (
	set input=%%i
	set output=!input:\assets\shaders\=\assets\spirv\!.spv

	set pathOfInput=%%~dpi
	set pathToCreate=!pathOfInput:\assets\shaders\=\assets\spirv\!
	mkdir !pathToCreate! 2>NUL

	echo Compiling shader %%i
	call %VULKAN_SDK%\Bin\glslc.exe !input! -o !output!
)

cd ..\..    
call premake\premake5.exe vs2022
PAUSE
