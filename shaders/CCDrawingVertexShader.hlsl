cbuffer MatrixBuffer
{
	matrix viewMatrix;
	matrix projectionMatrix;
};

struct VertexInputType
{
	float4 vertices : POSITION;
	float4 color : COLOR;
};

struct PixelInputType
{
	float4 vertices : SV_POSITION;
	float4 color : COLOR;
};


PixelInputType main( VertexInputType input )
{
    PixelInputType output;
    

	// Change the position vector to be 4 units for proper matrix calculations.
    input.vertices.w = 1.0f;

	// Calculate the position of the vertex against the world, view, and projection matrices.
    output.vertices = mul(input.vertices, viewMatrix);
    output.vertices = mul(output.vertices, projectionMatrix);
    
	// Store the input color for the pixel shader to use.
    output.color = input.color;

	// Store the texture coordinates for the pixel shader.
	//output.texCoords = input.texCoords;
    
    return output;
}

