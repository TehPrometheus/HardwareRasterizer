//------------------------------------------------
// Variables						
//------------------------------------------------
float4x4 gWorldViewProj : WorldViewProjection;
Texture2D gDiffuseMap : DiffuseMap;


//------------------------------------------------
// I/O Structs						
//------------------------------------------------

struct VS_INPUT
{
	float3 Position : POSITION;
	float2 TexCoord : TEXCOORD;
};

struct VS_OUTPUT
{
	float4 Position : SV_POSITION;
	float2 TexCoord : TEXCOORD;
};

//------------------------------------------------
// Sample States						
//------------------------------------------------

SamplerState samPoint
{
	Filter = MIN_MAG_MIP_POINT;
	AddressU = Wrap;
	AddressV = Wrap;
};

SamplerState samLinear
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Wrap;
	AddressV = Wrap;
};

SamplerState samAnisotropic
{
	Filter = ANISOTROPIC;
	AddressU = Wrap;
	AddressV = Wrap;
};

//------------------------------------------------
// Vertex Shader					
//------------------------------------------------

VS_OUTPUT VS(VS_INPUT input)
{
	VS_OUTPUT output = (VS_OUTPUT)0;
	output.Position = mul(float4(input.Position, 1.f),  gWorldViewProj);
	output.TexCoord = input.TexCoord;
	return output;
}

//------------------------------------------------
// Pixel Shader						
//------------------------------------------------

float4 PS_POINT(VS_OUTPUT input) : SV_TARGET
{
	return gDiffuseMap.Sample(samPoint,input.TexCoord);
}

float4 PS_LINEAR(VS_OUTPUT input) : SV_TARGET
{
	return gDiffuseMap.Sample(samLinear,input.TexCoord);
}

float4 PS_ANISOTROPIC(VS_OUTPUT input) : SV_TARGET
{
	return gDiffuseMap.Sample(samAnisotropic,input.TexCoord);
}


//------------------------------------------------
// Techniques					
//------------------------------------------------

technique11 PointTechnique
{
	pass P0
	{
		SetVertexShader(CompileShader( vs_5_0, VS() ) );
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS_POINT()));
	}
}

technique11 LinearTechnique
{
	pass P0
	{
		SetVertexShader(CompileShader( vs_5_0, VS() ) );
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS_LINEAR()));
	}
}

technique11 AnisotropicTechnique
{
	pass P0
	{
		SetVertexShader(CompileShader( vs_5_0, VS() ) );
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS_ANISOTROPIC()));
	}
}

