////////////////////////////////////////
// file: pshader.fx
// brief: Pixel Shaders.
// author: Mamoru Kaminaga
// date: 2016-11-05 15:17:10
////////////////////////////////////////
////////////////////////////////////////
// Globals
////////////////////////////////////////
Texture2D ShaderTexture : register(t0);
SamplerState SampleType : register(s0);
cbuffer ConstantBufferPS {
	float4 src;
};
////////////////////////////////////////
// Typedefs
////////////////////////////////////////
struct PixelInput {
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD0;
};
////////////////////////////////////////
// Pixel Shaders
////////////////////////////////////////
float4 pshader1(PixelInput input) : SV_Target {
	float4 cd = ShaderTexture.Sample(SampleType, input.tex);
	return cd * (1.0f - src.a) + src * src.a * cd.a;
}
