////////////////////////////////////////
// file: vshader.fx
// brief: Vertex Shaders
// author: Mamoru Kaminaga
// date: 2016-11-05 15:19:02
////////////////////////////////////////
////////////////////////////////////////
// Globals
////////////////////////////////////////
cbuffer ConstantBufferVS : register(b0) {
	matrix World;
};
////////////////////////////////////////
// Typedefs
////////////////////////////////////////
struct VertexInput {
	float4 pos : POSITION0;
	float2 tex : TEXCOORD0;
};
struct VertexOutput {
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD0;
};
////////////////////////////////////////
// Vertex Shaders
////////////////////////////////////////
VertexOutput vshader1(VertexInput input) {
	VertexOutput output;
	output.pos = mul(input.pos, World);
	output.tex = input.tex;
	return output;
}
