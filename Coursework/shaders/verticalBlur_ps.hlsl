Texture2D shaderTexture : register(t0);
SamplerState SampleType : register(s0);

struct InputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float2 texCoord1 : TEXCOORD1;
    float2 texCoord2 : TEXCOORD2;
    float2 texCoord3 : TEXCOORD3;
    float2 texCoord4 : TEXCOORD4;
    float2 texCoord5 : TEXCOORD5;
	float2 texCoord6 : TEXCOORD6;
	float2 texCoord7 : TEXCOORD7;
	float2 texCoord8 : TEXCOORD8;
	float2 texCoord9 : TEXCOORD9;
  
};

float4 main(InputType input) : SV_TARGET
{
    float weight0, weight1, weight2, weight3, weight4;
    float4 colour;

	// Create the weights that each neighbor pixel will contribute to the blur.
	weight0 = 0.382928;
	weight1 = 0.241732;
	weight2 = 0.060598;
	weight3 = 0.005977;
	weight4 = 0.000229;

    // Initialize the colour to black.
    colour = float4(0.0f, 0.0f, 0.0f, 0.0f);

    // Add the nine vertical pixels to the colour by the specific weight of each.
    colour += shaderTexture.Sample(SampleType, input.texCoord1) * weight4;
    colour += shaderTexture.Sample(SampleType, input.texCoord2) * weight3;
    colour += shaderTexture.Sample(SampleType, input.texCoord3) * weight2;
    colour += shaderTexture.Sample(SampleType, input.texCoord4) * weight1;
    colour += shaderTexture.Sample(SampleType, input.texCoord5) * weight0;
	colour += shaderTexture.Sample(SampleType, input.texCoord6) * weight1;
	colour += shaderTexture.Sample(SampleType, input.texCoord7) * weight2;
	colour += shaderTexture.Sample(SampleType, input.texCoord8) * weight3;
	colour += shaderTexture.Sample(SampleType, input.texCoord9) * weight4;
 
    // Set the alpha channel to one.
    colour.a = 1.0f;

    return colour;
}

