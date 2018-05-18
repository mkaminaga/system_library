# makefile
# date 2017-07-28
# Copyright 2017 Mamoru Kaminaga
VCBIN="C:\\Program Files (x86)\\Microsoft Visual Studio 14.0\\VC\\bin"
CC = $(VCBIN)\\cl.exe
LIB = $(VCBIN)\\lib.exe

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
	/D"UNICODE" /D"DIRECTINPUT_VERSION=0x0800" /Fo"$(OUTDIR)\\" /I"C:\projects\library\vecmath-c++-1.2-1.4"

$(TARGET): $(OBJS)
	$(LIB) /OUT:$(TARGET) $(OBJS)

.cc{$(OUTDIR)}.obj:
	@[ -d $(OUTDIR) ] || mkdir $(OUTDIR)
	$(CC) $(CCFLAGS) /c $<
