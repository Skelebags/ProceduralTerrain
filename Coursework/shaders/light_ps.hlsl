// Light pixel shader
// Calculate diffuse lighting for a single directional light (also texturing)

Texture2D shaderTexture : register(t0);
SamplerState SampleType : register(s0);

cbuffer LightBuffer : register(cb0)
{
	float4 ambientColour;
	float4 diffuseColor[2];
	float4 lightDirection[2];
	float4 position[2];
	float4 specularColour;
	float specularPower;
};

struct InputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float3 viewDirection : TEXCOORD1;
	float3 position3D : TEXCOORD2;
};

float4 main(InputType input) : SV_TARGET
{
	float4 textureColor;
	float3 lightDir[2];
	float lightIntensity[2];
	float4 color;
	float4 diffuseComponent;
	float spec;
	float4 finalSpec;
	float3 halfway;
	float lightDist[2];

	float attenuation;
	float constFactor = 1.0f;
	float lineFactor = 0.125f;
	float quadratic = 0.0f;
	float range = 25.0f;

	// Sample the pixel color from the texture using the sampler at this texture coordinate location.
	textureColor = shaderTexture.Sample(SampleType, input.tex);

	finalSpec = float4(0.0f, 0.0f, 0.0f, 0.0f);

	color = ambientColour;

	for (int i = 0; i < 2; i++)
	{

		lightDir[i] = input.position3D - position[i];
		lightDist[i] = length(lightDir[i]);

		if (lightDist[i] < range)
		{
			lightDir[i] = normalize(lightDir[i]);

			// Calculate the amount of light on this pixel.
			lightIntensity[i] = saturate(dot(input.normal, -lightDir[i]));

			if (lightIntensity[i] >= 0.0f)
			{
				diffuseComponent = (diffuseColor[i] * lightIntensity[i]);
				attenuation = 1 / (constFactor + lineFactor * lightDist[i] + quadratic * pow(lightDist[i], 2));
				if (attenuation > 0)
				{
					color = color + diffuseComponent * attenuation;
				}
				else
				{
					color =  color + diffuseComponent;
				}
				color = saturate(color);
			}
		}
	}
	if (lightIntensity[0] > lightIntensity[1])
	{
		// blinn fong
		halfway = normalize(lightDir[0] + input.viewDirection);
		spec = pow(max(dot(input.normal, halfway), 0.0f), specularPower);
		finalSpec = saturate(specularColour * spec);
	}
	else
	{
		// blinn fong
		halfway = normalize(lightDir[1] + input.viewDirection);
		spec = pow(max(dot(input.normal, halfway), 0.0f), specularPower);
		finalSpec = saturate(specularColour * spec);
	}
	// Add the specular component last to the output colour.
	color = saturate(color + finalSpec);
	

	// Multiply the texture pixel and the final diffuse color to get the final pixel color result.
	color = color * textureColor;

	return color;
}

