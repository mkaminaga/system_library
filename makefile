LIB=system.lib
SRC=\
common.cc\
graphic.cc\
input.cc\
sound.cc\
system.cc
OBJ=\
build/common.obj\
build/graphic.obj\
build/input.obj\
build/sound.obj\
build/system.obj

$(LIB): $(OBJ)
	lib /out:$@ $**

$(OBJ): $(SRC)
	cl /W4 /Zi /O2 /MT /D"WIN32" /D"NODEBUG" /D"_LIB" /Fo"build\\" /EHsc\
		/I"C:\projects" /D"_UNICODE" /D"UNICODE" /D"_CRT_SECURE_NO_WARNINGS"\
		/D"DIRECTINPUT_VERSION=0x0800" /c $(SRC)

clean:
	for %a in ($(LIB) $(OBJ)) do del %a
