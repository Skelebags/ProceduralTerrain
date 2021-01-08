// Light pixel shader
// Calculate diffuse lighting for a single directional light (also texturing)

Texture2D shaderTextures[2] : register(t0);
SamplerState SampleType : register(s0);

cbuffer LightBuffer : register(cb0)
{
	float4 ambientColour;
    float4 diffuseColor;
    float3 lightDirection;
	float padding;
};

struct InputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
	float3 viewDirection : TEXCOORD1;
};

float4 main(InputType input) : SV_TARGET
{
    float4 textureColor;
    float3 lightDir;
    float lightIntensity;
    float4 color;
	float slope;

	Texture2D lowTexture = shaderTextures[0];
	Texture2D highTexture = shaderTextures[1];

	slope = 1.0f - input.normal.y;

    // Sample the pixel color from the texture using the sampler at this texture coordinate location.
    //textureColor = shaderTexture.Sample(SampleType, input.tex);

	textureColor = lerp(lowTexture.Sample(SampleType, input.tex), highTexture.Sample(SampleType, input.tex), slope / 0.6f);

	color = ambientColour;

	// Invert the light direction for calculations.
    lightDir = -lightDirection;

    // Calculate the amount of light on this pixel.
    lightIntensity = saturate(dot(input.normal, lightDir));

	if (lightIntensity > 0.0f)
	{
		color += (diffuseColor * lightIntensity);
		color = saturate(color);
		
	}

    // Multiply the texture pixel and the final diffuse color to get the final pixel color result.
    color = color * textureColor;

    return color;
}

