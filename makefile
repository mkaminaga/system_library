# makefile
# date 2017-07-28
# Copyright 2017 Mamoru kaminaga
CC = C:\"Program Files (x86)"\"Microsoft Visual Studio"\2017\Community\VC\Tools\MSVC\14.10.25017\bin\HostX86\x86\cl.exe
LIB = C:\"Program Files (x86)"\"Microsoft Visual Studio"\2017\Community\VC\Tools\MSVC\14.10.25017\bin\HostX86\x86\lib.exe
OUTDIR = build
TARGET = system.lib
SRC =\
	common.cc\
	graphic.cc\
	input.cc\
	sound.cc\
	system.cc
OBJS =\
	$(OUTDIR)/common.obj\
	$(OUTDIR)/graphic.obj\
	$(OUTDIR)/input.obj\
	$(OUTDIR)/sound.obj\
	$(OUTDIR)/system.obj
CCFLAGS = /W4 /Zi /O2 /MT /EHsc /D"WIN32" /D"NODEBUG" /D"_LIB" /D"_UNICODE"\
	/D"UNICODE" /D"DIRECTINPUT_VERSION=0x0800" /Fo"$(OUTDIR)\\"

$(TARGET): $(OBJS)
	$(LIB) /OUT:$(TARGET) $(OBJS)

.cc{$(OUTDIR)}.obj:
	$(CC) $(CCFLAGS) /c $<
