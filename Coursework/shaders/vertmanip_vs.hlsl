// Light vertex shader
// Standard issue vertex shader, apply matrices, pass info to pixel shader

Texture2D heightmap : register(t0);
SamplerState SampleType : register(s0);

cbuffer MatrixBuffer : register(cb0)
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};

cbuffer CameraBuffer : register(cb1)
{
	float3 cameraPosition;
	float padding;
};

struct InputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
};

struct OutputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float3 viewDirection : TEXCOORD1;
	float3 position3D : TEXCOORD2;
};

OutputType main(InputType input)
{
    OutputType output;
	float4 worldPosition;
	float4 colors;

    input.position.w = 1.0f;

	worldPosition = mul(input.position, worldMatrix);

	colors = heightmap.SampleLevel(SampleType, input.tex.xy, 0);


	input.position.y = colors.xyz * 10;

    // Calculate the position of the vertex against the world, view, and projection matrices.
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    
    // Store the texture coordinates for the pixel shader.
    output.tex = input.tex;

	 // Calculate the normal vector against the world matrix only.
    output.normal = mul(input.normal, (float3x3)worldMatrix);
	
    // Normalize the normal vector.
    output.normal = normalize(output.normal);

	output.viewDirection = cameraPosition.xyz - worldPosition.xyz;
	// Normalize the viewing direction vector.
	output.viewDirection = normalize(output.viewDirection);

	output.position3D = mul(input.position, worldMatrix);

    return output;
}