Texture2D shaderTexture  :register( t0 );
SamplerState SampleType : register( s0 );

struct PixelInputType
{
	float4 vertices : SV_POSITION;
	float2 texCoords : TEXCOORD;
};

////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
float4 main( PixelInputType input) : SV_TARGET
{
	float4 textureColor;
	textureColor = shaderTexture.Sample( SampleType, input.texCoords );// * input.color;
    return textureColor;
}