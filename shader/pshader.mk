#
# Makefile for pixcel shader compilation
#
SHADER=pshader.fx
FXC=fxc.exe
SHADERVERSION=ps_4_0

"pshader1.h": $(SHADER)
	$(FXC) $(SHADER) /T $(SHADERVERSION) /E "pshader1" /Fh "pshader1.h"
