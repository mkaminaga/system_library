System Library
====

Library for game development on Windows.<br>
Provides you 2D graphics, sound and input functions. It also enables your game to stream large wave file.

Features
----

 * Structure and function based interface
 * Rucid dot plotting
 * Vecmath (Java(TM) 3D API 1.2.) C++ implementation is used
 * Single source and header
 * Provided with header file and static link library (*.lib)
 * MIT License

Requirement
----
You have to install these four toolchains before use this .

 * [Microsoft Visual Studio Comm2017](https://www.microsoft.com/ja-jp/dev/products/community.aspx)
 * [Windows SDK](https://developer.microsoft.com/ja-jp/windows/downloads/windows-8-1-sdk)
 * [Microsoft DirectX SDK (June 2010)](https://www.microsoft.com/en-us/download/details.aspx?id=6812)
 * [vecmath](http://objectclub.jp/download/vecmath1) (API 1.2 C++ port)

Microsoft Visual Studio is an IDE for Windows programming. This IDE must be setupped to be including VC++ toolchain. Even though you use different version of the IDE, you are probably able to use this library by setting properties correctly.<br>
Windows SDK and Microsoft DirectX SDK are a SDK for windows programming. Different version of these SDK are not checked.<br>
Vecmath is originally a java package for Java(TM) 3D API 1.2. C++ version of vecmath implementation is available.<br>

Specifications
----

 * Graphics
	* Available formats: png, jpeg, gif, and so on
	* Cache table of font images (16 x 4 charactors) for text draw
 * Sound
	* Cache wave data for play
	* Streaming wave file at a time
 * Input
	* Keyboard input
	* Joypad (A.K.A. Gamepad) input
	* Virtual input (mixture of above two input sources)

Build
----
There two things you have to do to use this library.<br>
First, set path to "system.h" and include this header from client source files.<br>
Second, add static link libraries (*.lib) to makefile of your project. Variable "LINK" is related. Required files are below.
```
"kernel32.lib"
"user32.lib"
"gdi32.lib"
"winspool.lib"
"comdlg32.lib"
"advapi32.lib"
"shell32.lib"
"ole32.lib"
"oleaut32.lib"
"uuid.lib"
"odbc32.lib"
"odbccp32.lib"
"dsound.lib"
"winmm.lib"
"Shcore.lib"
"winmm.lib"
"d3d11.lib"
"d3dx11.lib"
"dinput8.lib"
"dxguid.lib"
"system.lib"
```

Sample projects and instructions
----
These are sample and instructions in each sample projects. Each projects has example code and description of this library. For more information, read README.md in each folders.

|Folder|Description|
|---|---|
|samples/sample01_window|About how to create a plain window using basic functions and properties setter functions.|
|samples/sample02_picture|About how to manipulate images.|
|samples/sample03_font|About how to show text.|
|samples/sample04_input|About how to get input.|
|samples/sample05_sound|About how to play wave data.|
|samples/sample06_streaming|About how to play streaming of wave file.|

License
----
This software is released under the MIT License, see "LICENSE.md".
