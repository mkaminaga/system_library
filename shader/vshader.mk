#
# Makefile for vertex shader compilation
#
SHADER=vshader.fx
FXC=fxc.exe
SHADERVERSION=vs_4_0

"vshader1.h": $(SHADER)
	$(FXC) $(SHADER) /T $(SHADERVERSION) /E "vshader1" /Fh "vshader1.h"
