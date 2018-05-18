# makefile
# date 2017-06-17
# Copyright 2017 Mamoru Kaminaga
VCBIN="C:\\Program Files (x86)\\Microsoft Visual Studio 14.0\\VC\\bin"
CC = $(VCBIN)\\cl.exe
LINK = $(VCBIN)\\link.exe

OUTDIR = .
TARGET = main.exe
PDBFILE = main.pdb
MAPFILE = main.map
RESFILE = resource.res
SRC = main.cc
OBJS = $(OUTDIR)/main.obj
LIBS = "kernel32.lib" "user32.lib" "gdi32.lib" "winspool.lib" "comdlg32.lib"\
"advapi32.lib" "shell32.lib" "ole32.lib" "oleaut32.lib" "uuid.lib"\
"odbc32.lib" "odbccp32.lib"\
"dsound.lib" "winmm.lib" "Shcore.lib" "winmm.lib" "d3d11.lib" "d3dx11.lib"\
"dinput8.lib" "dxguid.lib" "../../system.lib"

# Debug build
#CPPFLAGS = /nologo /W4 /Zi /O2 /MT /D"UNICODE" /D"_UNICODE" /TP /EHsc\
# 	/Fd"$(OUTDIR)/" /D"DIRECTINPUT_VERSION=0x0800"
#LFLAGS = $(LIBS) /NOLOGO /SUBSYSTEM:WINDOWS /PDB:"$(PDBFILE)"\
# 	/MAP:"$(MAPFILE)" /DEBUG

# Release build
CPPFLAGS = /nologo /W4 /Zi /O2 /MT /D"UNICODE" /D"_UNICODE" /TP /EHsc\
 	/Fd"$(OUTDIR)/" /D"DIRECTINPUT_VERSION=0x0800" /I"C:\projects\library\vecmath-c++-1.2-1.4"
LFLAGS = $(LIBS) /NOLOGO /SUBSYSTEM:WINDOWS /PDB:"$(PDBFILE)" /MAP:"$(MAPFILE)"

ALL: $(TARGET)

$(TARGET): $(OBJS) $(RESFILE)
	$(LINK) $(LFLAGS) /OUT:$(TARGET) $(OBJS) $(RESFILE)

.cc{$(OUTDIR)}.obj:
	@[ -d $(OUTDIR) ] || mkdir $(OUTDIR)
	$(CC) $(CPPFLAGS) /Fo"$(OUTDIR)\\" /c $<
