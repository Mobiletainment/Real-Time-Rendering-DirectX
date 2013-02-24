matrix worldMatrix;
matrix viewMatrix;
matrix projectionMatrix;

//The shader will require two light matrices to transform the vertex based on the light's view point. 
matrix lightViewMatrix;
matrix lightProjectionMatrix;

Texture2D shaderTexture;

//The depthMapTexture is the shadow map. This texture contains the scene depth buffer rendered from the light's perspective. 
Texture2D depthMapTexture;

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
};

//The PixelInputType has lightViewPosition for sending the light perspective transformed vertex into the pixel shader.
struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
    float4 lightViewPosition : TEXCOORD1;
	float3 lightPos : TEXCOORD2;
};

// Vertex Shader
PixelInputType ShadowVertexShader(VertexInputType input)
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

    // Calculate the position of the vertex in the world.
    worldPosition = mul(input.position, worldMatrix);

    // Determine the light position based on the position of the light and the position of the vertex in the world.
    output.lightPos = lightPosition.xyz - worldPosition.xyz;

    // Normalize the light position vector.
    output.lightPos = normalize(output.lightPos);

	return output;
}

// Pixel Shader
float4 ShadowPixelShader(PixelInputType input) : SV_Target
{
	float bias;
    float4 color;
	float2 projectTexCoord;
	float depthValue;
	float lightDepthValue;
    float lightIntensity;
	float4 textureColor;

	//Shadow mapping requires a bias adjustment when comparing the depth of the light and the depth of the object due to the low floating point precision of the depth map. 
	// Set the bias value for fixing the floating point precision issues.
	bias = 0.000001f;

	// Set the default output color to the ambient light value for all pixels.
    color = ambientColor;

	// Calculate the projected texture coordinates for sampling the shadow map (depth buffer texture) based on the light's viewing position.
	projectTexCoord.x =  input.lightViewPosition.x / input.lightViewPosition.w / 2.0f + 0.5f;
	projectTexCoord.y = -input.lightViewPosition.y / input.lightViewPosition.w / 2.0f + 0.5f;

	//Check if the projected coordinates are in the view of the light, if not then the pixel gets just an ambient value. 
	// Determine if the projected coordinates are in the 0 to 1 range.  If so then this pixel is in the view of the light.
	if((saturate(projectTexCoord.x) == projectTexCoord.x) && (saturate(projectTexCoord.y) == projectTexCoord.y))
	{
		//Now that we are in the view of the light we will retrieve the depth value from the shadow map (depthMapTexture). We only sample the red component since 
		//this is a grey scale texture. The depth value we get from the texture translates into the distance to the nearest object.
		//This is important since objects are what cast the shadows and hence why it is called a shadow map. 
		// Sample the shadow map depth value from the depth texture using the sampler at the projected texture coordinate location.
		depthValue = depthMapTexture.Sample(SampleTypeClamp, projectTexCoord).r;

		//Now that we have the depth of the object for this pixel we need the depth of the light to determine if it is in front or behind the object. We get this from the lightViewPosition.
		//Note that we need to subtract the bias from this or we will get the floating point precision issue. 
		// Calculate the depth of the light.
		lightDepthValue = input.lightViewPosition.z / input.lightViewPosition.w;

		// Subtract the bias from the lightDepthValue.
		lightDepthValue = lightDepthValue - bias;

		//Now we perform the comparison between the light depth and the object depth. If the light is closer to us then no shadow. But if the light is behind an object in the shadow map then it gets shadowed. Note that a shadow just means we only apply ambient light, we don't color it black or anything. 
		
		// Compare the depth of the shadow map value and the depth of the light to determine whether to shadow or to light this pixel.
		// If the light is in front of the object then light the pixel, if not then shadow this pixel since an object (occluder) is casting a shadow on it.
		if(lightDepthValue < depthValue)
		{
			//If the light was in front of the object then there is no shadow and we do regular lighting. 

		    // Calculate the amount of light on this pixel.
			lightIntensity = saturate(dot(input.normal, input.lightPos));

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
		lightIntensity = saturate(dot(input.normal, input.lightPos));
		color += 0.1 * diffuseColor * lightIntensity; // += (diffuseColor * lightIntensity);
		// Saturate the final light color.
		color = saturate(color);
	}

	// Sample the pixel color from the texture using the sampler at this texture coordinate location.
	textureColor = shaderTexture.Sample(SampleTypeWrap, input.tex);

	// Combine the light and texture color.
	color = color * textureColor;

    return color;
}

technique10 ShadowTechnique
{
    pass pass0
    {
        SetVertexShader(CompileShader(vs_4_0, ShadowVertexShader()));
        SetPixelShader(CompileShader(ps_4_0, ShadowPixelShader()));
        SetGeometryShader(NULL);
    }
}