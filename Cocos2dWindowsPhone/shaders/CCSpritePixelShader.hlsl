cbuffer TextureColorBuffer
{
	bool istexture[16];
};

Texture2D shaderTexture  :register( t0 );
SamplerState SampleType : register( s0 );

struct PixelInputType
{
	float4 vertices : SV_POSITION;
    float4 color : COLOR;
	float2 texCoords : TEXCOORD;
};

////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
float4 main( PixelInputType input) : SV_TARGET
{
	float4 textureColor;
	if(istexture[0])
	{
		textureColor = shaderTexture.Sample( SampleType, input.texCoords ) * input.color;
	}
	else
	{
		textureColor = input.color;
	}
	
    return textureColor;
}