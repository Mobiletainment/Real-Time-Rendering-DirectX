#include "Graphics.h"

Graphics::Graphics()
{
	m_D3D = 0;
	m_Camera = 0;
	m_ShadowShader = 0;
	m_Light = 0;
	m_DepthShader = 0;
	m_Render2Texture = 0;
	m_CurrentShader = 0;
	m_isWireFrameEnabled = 0;
	CurrentRenderMode = RenderModeBumpAndShadowMap;
}

Graphics::Graphics(const Graphics& other)
{
}

Graphics::~Graphics()
{
}

bool Graphics::Initialize(int screenWidth, int screenHeight, HWND hwnd, int currentSampleCount, int currentSampleQuality)
{
	bool result = InitializeD3D(screenWidth, screenHeight, hwnd, currentSampleCount, currentSampleQuality);

	if (result == false)
		return false;

	// Create the camera object.
	m_Camera = new Camera;
	if(!m_Camera)
	{
		return false;
	}

	// Set the initial position of the camera.
	//m_Camera->InitCamera(-57.7f, 62.6f, -70.0f);
	m_Camera->Initialize(-37.99f, -5.588f, -34.273f);
	m_Camera->YawPitchRollCamera(0, 0, 0);
	//m_Camera->YawPitchRollCamera(-12, -12, 0);
	//m_Camera->m_qRotation = D3DXQUATERNION(-0.0633554533f, -0.372116894f, 0.0295272041f, 0.0f);
	m_Models = new vector<Model *>(ROWS*ROWS*ROWS); //5x5x5

	m_Cube = new Model;
	result = m_Cube->Init(m_D3D->GetDevice(), "resources/cube.txt", L"resources/stone01.dds");
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return false;
	}

	result = m_Cube->LoadBumpTexture(m_D3D->GetDevice(), L"resources/bump01.dds");
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the bump map for model object.", L"Error", MB_OK);
		return false;
	}


	//Create the Sun object
	Model *sphere = new Model;
	if(!sphere)
	{
		return false;
	}
	result = sphere->Init(m_D3D->GetDevice(), "resources/sphere.txt", L"resources/stone01.dds");
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return false;
	}

	result = sphere->LoadBumpTexture(m_D3D->GetDevice(), L"resources/bump01.dds");
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the bump map for model object.", L"Error", MB_OK);
		return false;
	}



	for (unsigned int i = 0; i < m_Models->size(); i++)
	{
		if (i % 3 != 0)
			m_Models->at(i) = new Model(*m_Cube);
		else
			m_Models->at(i) = new Model(*sphere);
	}

	float xPos = -3.0f, yPos = 9.0f, zPos = 0.0f;
	float spacing = 3.0f;
	for (int z = 0; z < ROWS; z++)
	{
		yPos -= spacing * ROWS;
		zPos += spacing;
		
		for (int y = 0; y < ROWS; y++)
		{
			xPos -= spacing * ROWS;
			yPos += spacing;
			
			for (int x = 0; x < ROWS; x++)
			{
				// Prepare transforms
				xPos += spacing;
				Model *model = m_Models->at(z* (ROWS*ROWS) + y* ROWS + x);
				model->SetPosition(xPos, yPos, zPos);
			}
		}
	}

	
	//Create the Sun object
	m_SunSphere = new Model;
	if(!m_SunSphere)
	{
		return false;
	}
	result = m_SunSphere->Init(m_D3D->GetDevice(), "resources/sphere.txt", L"resources/sunmap.dds");
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return false;
	}

	result = m_SunSphere->LoadBumpTexture(m_D3D->GetDevice(), L"resources/bump01.dds");
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the bump map for model object.", L"Error", MB_OK);
		return false;
	}

	
	// Create the light object.
	m_Light = new SpotLight;
	if(!m_Light)
	{
		return false;
	}

	// Initialize the light object.
	float lightPosX = -9.3f;
	float lightPosY = 19.19f;
	float lightPosZ = -9.89f;

	m_Light->SetAmbientColor(0.10f, 0.10f, 0.10f, 1.0f);
	m_Light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	//m_Light->SetDirection(0.0f, -1.0f, 0.0f);
	m_Light->SetPosition(lightPosX, lightPosY, lightPosZ);
	//The lookAt is now set in the light object and then we can generate the projection matrix that this light will have. 
	m_Light->SetLookAt(0.0f, 0.0f, 0.0f);
	m_Light->GenerateProjectionMatrix(SCREEN_DEPTH, SCREEN_NEAR);
	m_SunSphere->SetPosition(lightPosX, lightPosY, lightPosZ);



	return true;
}

void Graphics::ShutdownD3D()
{
	// Release the light shader object.
	if(m_ShadowShader)
	{
		m_ShadowShader->Shutdown();
		delete m_ShadowShader;
		m_ShadowShader = 0;
	}

	// Release the depth shader object.
	if(m_DepthShader)
	{
		m_DepthShader->Shutdown();
		delete m_DepthShader;
		m_DepthShader = 0;
	}

	if(m_ShadowBumpShader)
	{
		m_ShadowBumpShader->Shutdown();
		delete m_ShadowBumpShader;
		m_ShadowBumpShader = 0;
	}

	// Release the Render2Texture object.
	if(m_Render2Texture)
	{
		m_Render2Texture->Shutdown();
		delete m_Render2Texture;
		m_Render2Texture = 0;
	}

	// Release the D3D object.
	if(m_D3D)
	{
		m_D3D->Shutdown();
		delete m_D3D;
		m_D3D = 0;
	}
}

bool Graphics::InitializeD3D(int screenWidth, int screenHeight, HWND hwnd, int currentSampleCount, int currentSampleQuality) 
{
	bool result;
	// Create the Direct3D object.
	m_D3D = new D3D(currentSampleCount, currentSampleQuality);
	if(!m_D3D)
	{
		return false;
	}

	// Initialize the Direct3D object.
	result = m_D3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize Direct3D.", L"Error", MB_OK);
		return false;
	}


	//We create a Render2Texture object that will be used as the shadow map. The depth buffer of the scene will be rendered from the light's perspective onto this Render2Texture object.
	m_Render2Texture = new Render2Texture;
	if(!m_Render2Texture)
	{
		return false;
	}

	// Initialize the render as texture object.
	result = m_Render2Texture->Initialize(m_D3D->GetDevice(), SHADOWMAP_WIDTH, SHADOWMAP_HEIGHT, SCREEN_DEPTH, SCREEN_NEAR);
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the Render2Texture object.", L"Error", MB_OK);
		return false;
	}

	// Create the depth shader object.
	m_DepthShader = new DepthShader;
	if(!m_DepthShader)
	{
		return false;
	}

	// Initialize the depth shader object.
	result = m_DepthShader->Initialize(m_D3D->GetDevice(), hwnd);
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the depth shader object.", L"Error", MB_OK);
		return false;
	}


	// Create the shadow shader. 
	m_ShadowShader = new ShadowShader;
	if(!m_ShadowShader)
	{
		return false;
	}

	// Initialize the shadow shader object.
	result = m_ShadowShader->Initialize(m_D3D->GetDevice(), hwnd);
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the light shader object.", L"Error", MB_OK);
		return false;
	}


	//new in v3: Bump Map Shader

	// Create the bump map + shadow shader. 
	m_ShadowBumpShader = new ShadowAndBumpShader;
	if(!m_ShadowBumpShader)
	{
		return false;
	}
	result = m_ShadowBumpShader->Init(m_D3D->GetDevice(), hwnd);
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the bumpmap shader object.", L"Error", MB_OK);
		return false;
	}

	//set the active shader
	m_CurrentShader = m_ShadowBumpShader;

	return true;
}

int Graphics::GetMultiSampleQualityLevel(int sampleCount)
{
	return m_D3D->GetMultisampleQualityLevels(sampleCount, DXGI_FORMAT_R8G8B8A8_UNORM);
}

void Graphics::Shutdown()
{
		// Release the light object.
	if(m_Light)
	{
		delete m_Light;
		m_Light = 0;
	}

	
	// Release the model objects.
	m_Cube->Shutdown();
	delete m_Cube;
	m_Cube = 0;


	// Release the camera object.
	if(m_Camera)
	{
		delete m_Camera;
		m_Camera = 0;
	}

	ShutdownD3D();

	return;
}

bool Graphics::Update(float syncTime)
{
	m_Camera->SetSyncTime(syncTime);
	return true;
}

void Graphics::UpdateCameraMovement(float moveForward, float moveSideways)
{
	m_Camera->MoveCamera(moveForward,moveSideways);
	return;
}

void Graphics::UpdateCameraOrientation(float mouseDeltaX, float mouseDeltaY, float roll)
{
	m_Camera->YawPitchRollCamera(mouseDeltaX, mouseDeltaY, roll);
	return;
}

void Graphics::UpdateLightPosition(float forward, float sideways, float upwards)
{
	D3DXMATRIX camRotation;
	D3DXVECTOR3 lightPosition = m_Light->GetPosition();

	m_Camera->GetDirection(camRotation);
	
	D3DXVECTOR3 camRight = D3DXVECTOR3( camRotation(0,0), camRotation(1,0), camRotation(2,0) );
	D3DXVECTOR3 camUp = D3DXVECTOR3( camRotation(0,1), camRotation(1,1), camRotation(2,1) );
	D3DXVECTOR3 camDirection = D3DXVECTOR3( camRotation(0,2), camRotation(1,2), camRotation(2,2) );
	
	D3DXVECTOR3 lightMove = ((camDirection * forward) + (camRight * sideways) + (camUp * upwards));
	D3DXVec3Normalize(&lightMove, &lightMove);

	lightPosition += lightMove;
	m_Light->SetPosition(lightPosition.x, lightPosition.y, lightPosition.z);
	m_SunSphere->SetPosition(lightPosition.x, lightPosition.y, lightPosition.z);
	return;
}
/*
void Graphics::UpdateLightDirection(bool moveForward)
{
	D3DXVECTOR3 lightDirection = m_Light->GetPosition();
	
	float direction = 0.005f;
	
	if (moveForward == false)
		direction *= -1;

	lightDirection.z += direction;
	lightDirection.x += direction;

	D3DXVec3Normalize(&lightDirection, &lightDirection);

	m_Light->SetPosition(lightDirection.x, lightDirection.y, lightDirection.z);
}
*/


void Graphics::SavePoint()
{
	m_Camera->SavePoint();
}

void Graphics::RunPoints()
{
	m_Camera->RunPoints();
}

//The RenderSceneToTexture function is called at the beginning of the frame rendering.
//We basically render the depth buffer of the scene from the perspective of the light into the 
//Render2Texture object which then becomes our shadow map.
bool Graphics::RenderSceneToTexture()
{
	D3DXMATRIX worldMatrix, lightViewMatrix, lightProjectionMatrix, translateMatrix;
	float posX, posY, posZ;
	
	// Set the render target to be the render2texture object.
	m_Render2Texture->SetRenderTarget(m_D3D->GetDevice());

	// Clear the Render2Texture.
	m_Render2Texture->ClearRenderTarget(m_D3D->GetDevice(), 0.0f, 0.0f, 0.0f, 1.0f);

	//generate the view matrix of the light based on the light's position.
	m_Light->GenerateViewMatrix();

	// Get the world matrix from the d3d object.
	m_D3D->GetWorldMatrix(worldMatrix);

	// Get the view and orthographic matrices from the light object.
	m_Light->GetViewMatrix(lightViewMatrix);
	m_Light->GetProjectionMatrix(lightProjectionMatrix);
	
	// Render all the objects in the scene using the depth shader and the light view and projection matrices.
	
	// render models
	for (unsigned int i = 0; i < m_Models->size(); i++)
	{
		float x, y, z;
		Model *model = m_Models->at(i);
		model->GetPosition(x,y,z);
		// Reset the world matrix.
		m_D3D->GetWorldMatrix(worldMatrix);
		//translate to the model's position
		D3DXMatrixTranslation(&translateMatrix, x, y, z);
		D3DXMatrixMultiply(&worldMatrix, &worldMatrix, &translateMatrix); 
		
		model->Render(m_D3D->GetDevice());
		// Render the model using the depth shader.
		m_DepthShader->Render( m_D3D->GetDevice(), model->GetIndexCount(), worldMatrix, lightViewMatrix, lightProjectionMatrix);
	}

	//render the 'sun'-Model
	m_D3D->GetWorldMatrix(worldMatrix);
	m_SunSphere->GetPosition(posX, posY, posZ);
	D3DXMatrixTranslation(&worldMatrix, posX, posY, posZ);
	m_SunSphere->Render(m_D3D->GetDevice());
	m_DepthShader->Render(m_D3D->GetDevice(), m_SunSphere->GetIndexCount(), worldMatrix, lightViewMatrix, lightProjectionMatrix);

	
	// Reset the render target back to the original back buffer and not the Render2Texture anymore.
	m_D3D->SetBackBufferRenderTarget();

	// Reset the viewport back to the original.
	m_D3D->ResetViewport();

	return true;
}

bool Graphics::Render()
{
	D3DXMATRIX worldMatrix, viewMatrix, projectionMatrix, translateMatrix;
	D3DXMATRIX lightViewMatrix, lightProjectionMatrix; //new in V2

	// First render the scene to a texture object to get our shadow map
	bool result = RenderSceneToTexture();
	if(!result)
	{
		return false;
	}

	// Clear the buffers to begin the scene.
	m_D3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// Generate the view matrix based on the camera's position.
	m_Camera->Render();

	// Generate the light view matrix based on the light's position.
	m_Light->GenerateViewMatrix();

	// Get the world, view, and projection matrices from the camera and d3d objects.
	m_Camera->GetViewMatrix(viewMatrix);
	m_D3D->GetWorldMatrix(worldMatrix);
	m_D3D->GetProjectionMatrix(projectionMatrix);

	// Get the light's view and projection matrices from the light object.
	m_Light->GetViewMatrix(lightViewMatrix);
	m_Light->GetProjectionMatrix(lightProjectionMatrix);


	// Get the world, view, and projection matrices from the camera and d3d objects.
	m_Camera->GetViewMatrix(viewMatrix);
	m_D3D->GetWorldMatrix(worldMatrix);
	m_D3D->GetProjectionMatrix(projectionMatrix);

	// Get the light's view and projection matrices from the light object.
	m_Light->GetViewMatrix(lightViewMatrix);
	m_Light->GetProjectionMatrix(lightProjectionMatrix);
	
	//Now render each model using the shadow map shader, the light matrices, and the shadow map texture. 
	float xPos, yPos, zPos; 
	// Prepare transforms
	for (unsigned int i = 0; i < m_Models->size(); i++)
	{	
		Model *model = m_Models->at(i);
		model->GetPosition(xPos, yPos, zPos);
		m_D3D->GetWorldMatrix(worldMatrix);
		// Setup the translation matrix for the cube model.
		D3DXMatrixTranslation(&worldMatrix, xPos, yPos, zPos);
		// Put the model vertex and index buffers on the graphics pipeline to prepare them for drawing.
		model->Render(m_D3D->GetDevice());
		// Render the model using the shadow shader.
		
		//Obsolete:
		//m_ShadowShader->Render(m_D3D->GetDevice(), model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, lightViewMatrix, lightProjectionMatrix, model->GetTexture(), 
		//		m_Render2Texture->GetShaderResourceView(), m_Light->GetPosition(), m_Light->GetAmbientColor(), m_Light->GetDiffuseColor());

		
		m_CurrentShader->Render(m_D3D->GetDevice(), model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,lightViewMatrix,lightProjectionMatrix, model->GetTextureArray(),
				m_Render2Texture->GetShaderResourceView(), m_Light->GetPosition(), m_Light->GetAmbientColor(), m_Light->GetDiffuseColor());
	}

	//render the sun
	m_D3D->GetWorldMatrix(worldMatrix);
	m_SunSphere->GetPosition(xPos, yPos, zPos);
	D3DXMatrixTranslation(&worldMatrix, xPos, yPos, zPos);
	m_SunSphere->Render(m_D3D->GetDevice());
	m_CurrentShader->Render(m_D3D->GetDevice(), m_SunSphere->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, lightViewMatrix, 
		lightProjectionMatrix, m_SunSphere->GetTextureArray(), m_Render2Texture->GetShaderResourceView(), m_Light->GetPosition(), 
		m_Light->GetAmbientColor(), m_Light->GetDiffuseColor());

	
	// Present the rendered scene to the screen.
	m_D3D->EndScene();

	return true;
}

void Graphics::ActivateBumpMapRenderMode()
{
	m_CurrentShader = m_ShadowBumpShader;
	CurrentRenderMode = RenderModeBumpAndShadowMap;
}

void Graphics::ActivateShadowMapRenderMode()
{
	m_CurrentShader = m_ShadowShader;
	CurrentRenderMode = RenderModeShadowMap;

}

void Graphics::ToggleWireFrameMode(bool switchOn)
{
	if (switchOn == true)
	{
		m_D3D->GetDevice()->RSSetState(m_D3D->m_rasterStateWire);
	}
	else
	{
		//normal
		m_D3D->GetDevice()->RSSetState(m_D3D->m_rasterState);
	}

	m_isWireFrameEnabled = switchOn;
}