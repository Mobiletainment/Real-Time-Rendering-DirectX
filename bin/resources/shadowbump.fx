////////////////////////////////////////////////////////////////////////////////
// Filename: shadow.fx
////////////////////////////////////////////////////////////////////////////////


//////////////
// MATRICES //
//////////////
matrix worldMatrix;
matrix viewMatrix;
matrix projectionMatrix;
matrix lightViewMatrix;
matrix lightProjectionMatrix;



Texture2D shaderTextures[2];
//The depthMapTexture is the shadow map. This texture contains the scene depth buffer rendered from the light's perspective. 
Texture2D depthMapTexture;


/////////////
// GLOBALS //
/////////////
float4 ambientColor;
float4 diffuseColor;
float3 lightPosition;


//We require a clamp based sampler when sampling the depth buffer so that it doesn't wrap around and sample incorrect information.
SamplerState SampleTypeClamp
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Clamp;
    AddressV = Clamp;
};

SamplerState SampleTypeWrap
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};


struct VertexInputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float3 binormal : BINORMAL;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
    float3 binormal : BINORMAL;
    float4 lightViewPosition : TEXCOORD1;
	float3 lightPos : TEXCOORD2;
};


////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
PixelInputType ShadowBumpVertexShader(VertexInputType input)
{
    PixelInputType output;
	float4 worldPosition;
    
    
	// Change the position vector to be 4 units for proper matrix calculations.
    input.position.w = 1.0f;

	// Calculate the position of the vertex against the world, view, and projection matrices.
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
  
	//Here we transform the vertex based on the light's perspective.  
	// Calculate the position of the vertice as viewed by the light source.
    output.lightViewPosition = mul(input.position, worldMatrix);
    output.lightViewPosition = mul(output.lightViewPosition, lightViewMatrix);
    output.lightViewPosition = mul(output.lightViewPosition, lightProjectionMatrix);

	// Store the texture coordinates for the pixel shader.
    output.tex = input.tex;
    
	// Calculate the normal vector against the world matrix only.
    output.normal = mul(input.normal, (float3x3)worldMatrix);
	
    // Normalize the normal vector.
    output.normal = normalize(output.normal);

	// Calculate the tangent vector against the world matrix only and then normalize the final value.
	output.tangent = mul(input.tangent, (float3x3)worldMatrix);
	output.tangent = normalize(output.tangent);
	
	// Calculate the binormal vector against the world matrix only and then normalize the final value.
    output.binormal = mul(input.binormal, (float3x3)worldMatrix);
	output.binormal = normalize(output.binormal);

	
    // Calculate the position of the vertex in the world.
    worldPosition = mul(input.position, worldMatrix);

    // Determine the light position based on the position of the light and the position of the vertex in the world.
    output.lightPos = lightPosition.xyz - worldPosition.xyz;

    // Normalize the light position vector.
    output.lightPos = normalize(output.lightPos);

	return output;
}


////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
float4 ShadowBumpPixelShader(PixelInputType input) : SV_Target
{
	float bias;
    float4 color;
	float2 projectTexCoord;
	float depthValue;
	float lightDepthValue;
    float lightIntensity;
	float4 textureColor;
	float4 bumpMap;
    float3 bumpNormal;

	//Shadow mapping requires a bias adjustment when comparing the depth of the light and the depth of the object due to the low floating point precision of the depth map. 
	// Set the bias value for fixing the floating point precision issues.
	bias = 0.000001f;

	// Set the default output color to the ambient light value for all pixels.
    color = ambientColor;
	
	// Sample the pixel in the bump map.
    bumpMap = shaderTextures[1].Sample(SampleTypeWrap, input.tex);


    // Expand the range of the normal value from (0, +1) to (-1, +1).
    bumpMap = (bumpMap * 2.0f) - 1.0f;

    // Calculate the normal from the data in the bump map.
    bumpNormal = input.normal + bumpMap.x * input.tangent + bumpMap.y * input.binormal;
	
    // Normalize the resulting bump normal.
    bumpNormal = normalize(bumpNormal);
	
	
	// Calculate the projected texture coordinates for sampling the shadow map (depth buffer texture) based on the light's viewing position.
	projectTexCoord.x =  input.lightViewPosition.x / input.lightViewPosition.w / 2.0f + 0.5f;
	projectTexCoord.y = -input.lightViewPosition.y / input.lightViewPosition.w / 2.0f + 0.5f;

	//Check if the projected coordinates are in the view of the light, if not then the pixel gets just an ambient value. 
	
	// Determine if the projected coordinates are in the 0 to 1 range.  If so then this pixel is in the view of the light.
	if((saturate(projectTexCoord.x) == projectTexCoord.x) && (saturate(projectTexCoord.y) == projectTexCoord.y))
	{
		// Sample the shadow map depth value from the depth texture using the sampler at the projected texture coordinate location.
		depthValue = depthMapTexture.Sample(SampleTypeClamp, projectTexCoord).r;

		// Calculate the depth of the light.
		lightDepthValue = input.lightViewPosition.z / input.lightViewPosition.w;

		// Subtract the bias from the lightDepthValue.
		lightDepthValue = lightDepthValue - bias;

		// Compare the depth of the shadow map value and the depth of the light to determine whether to shadow or to light this pixel.
		// If the light is in front of the object then light the pixel, if not then shadow this pixel since an object (occluder) is casting a shadow on it.
		if(lightDepthValue < depthValue)
		{
		    // Calculate the amount of light on this pixel.
			lightIntensity = saturate(dot(bumpNormal, input.lightPos));

		    if(lightIntensity > 0.0f)
			{
				// Determine the final diffuse color based on the diffuse color and the amount of light intensity.
				color += (diffuseColor * lightIntensity);

				// Saturate the final light color.
				color = saturate(color);
			}
		}
	}
	else //FIXME: better exposure of geometry when in shadow
	{
		lightIntensity = saturate(dot(bumpNormal, input.lightPos));
		color += 0.1 * diffuseColor * lightIntensity; // += (diffuseColor * lightIntensity);
		// Saturate the final light color.
		color = saturate(color);
	}

	// Sample the pixel color from the texture using the sampler at this texture coordinate location.
	textureColor = shaderTextures[0].Sample(SampleTypeWrap, input.tex);

	// Combine the light and texture color.
	color = color * textureColor;

    return color;
}


////////////////////////////////////////////////////////////////////////////////
// Technique
////////////////////////////////////////////////////////////////////////////////
technique10 ShadowBumpTechnique
{
    pass pass0
    {
        SetVertexShader(CompileShader(vs_4_0, ShadowBumpVertexShader()));
        SetPixelShader(CompileShader(ps_4_0, ShadowBumpPixelShader()));
        SetGeometryShader(NULL);
    }
}