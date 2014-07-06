float4x4 WORLD_VIEW_PROJECTION_MATRIX : register(c0);
float4x4 WORLD_MATRIX : register(c4);

struct VS_INPUT {
	float4 Position   : POSITION;
	float2 Texture    : TEXCOORD0;
};

struct VS_OUTPUT {
	float4 Position   : POSITION;
	float2 Texture    : TEXCOORD0;
};

void VertexShaderFunction(VS_INPUT In, out VS_OUTPUT Out) {
	Out.Texture = In.Texture;
	Out.Position = mul(In.Position, WORLD_VIEW_PROJECTION_MATRIX);
}