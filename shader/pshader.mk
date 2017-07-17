#
# Makefile for pixcel shader compilation
#
SHADER=pshader.fx
FXC=fxc.exe
SHADERVERSION=ps_4_0

"pshader2.h": $(SHADER)
	$(FXC) $(SHADER) /T $(SHADERVERSION) /E "pshader1" /Fh "pshader1.h"
	$(FXC) $(SHADER) /T $(SHADERVERSION) /E "pshader2" /Fh "pshader2.h"
