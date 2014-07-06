sampler s0;
float4 Radius : register(c0);
float4 InnerColor : register(c1);
float4 OuterColor : register(c2);

struct PS_INPUT
{
	float4 Position   : POSITION;
	float2 Texture    : TEXCOORD0;
};

float4 PixelShaderFunction(PS_INPUT In) : COLOR0 {
	//float4 color = float4(0.0f, 0.0f, .0f, 0.5f);
	//color.rg = In.Texture;
	float centerDist = length(In.Texture - 0.5) * Radius.z;
	float distMax = Radius.y - centerDist;
    clip(distMax);
	float distMin = centerDist - Radius.x;
    clip(distMin);
	float4 color = InnerColor + distMin / (Radius.y - Radius.x) * (OuterColor - InnerColor);
	color.a = color.a * saturate(distMax / Radius.w) * saturate(distMin / Radius.w);
	return color;
}
/*
technique Technique1 {
    pass Pass1 {
        PixelShader = compile ps_2_0 PixelShaderFunction();
    }
}
*/
