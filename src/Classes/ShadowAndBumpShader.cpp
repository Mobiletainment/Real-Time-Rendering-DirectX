#include "ShadowAndBumpShader.h"
#include "Helper.h"

ShadowAndBumpShader::ShadowAndBumpShader(void) {
	m_effect = 0;
	m_technique = 0;
	m_layout = 0;
	m_worldMatrixPtr = 0;
	m_viewMatrixPtr = 0;
	m_projectionMatrixPtr = 0;
	m_lightViewMatrixPtr = 0;
	m_lightProjectionMatrixPtr = 0;
	m_textureArrayPtr = 0;
	m_depthMapTexturePtr = 0;
	m_lightPositionPtr = 0;
	m_ambientColorPtr = 0;
	m_diffuseColorPtr = 0;
}

ShadowAndBumpShader::~ShadowAndBumpShader(void) {}


bool ShadowAndBumpShader::Init(ID3D10Device* device, HWND hwnd) {
	bool result;
	result = InitShader(device, hwnd, L"resources/shadowbump.fx");
	if(!result) {
		return false;
	}
	return true;
}

void ShadowAndBumpShader::Shutdown(void) {
	ShutdownShader();
}


void ShadowAndBumpShader::Render(ID3D10Device* device, int indexCount, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix, D3DXMATRIX lightViewMatrix, D3DXMATRIX lightProjectionMatrix, ID3D10ShaderResourceView** texture, 
						  ID3D10ShaderResourceView* depthMapTexture, D3DXVECTOR3 lightPosition, D3DXVECTOR4 ambientColor, D3DXVECTOR4 diffuseColor) 
{
							  SetShaderParameters(worldMatrix, viewMatrix, projectionMatrix, lightViewMatrix, lightProjectionMatrix, texture, depthMapTexture,
								  lightPosition, ambientColor, diffuseColor);
							  RenderShader(device, indexCount);
}


bool ShadowAndBumpShader::InitShader(ID3D10Device* device, HWND hwnd, WCHAR* filename) {
	HRESULT result;
	ID3D10Blob* errorMessage;
	D3D10_INPUT_ELEMENT_DESC polygonLayout[5];
	unsigned int numElements;
	D3D10_PASS_DESC passDesc;
	errorMessage = 0;
	result = D3DX10CreateEffectFromFile(filename, NULL, NULL, "fx_4_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, device, NULL, NULL, &m_effect, &errorMessage, 
		NULL);
	if(FAILED(result))
	{
		if(errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, filename);
		}
		else
		{
			MessageBox(hwnd, filename, L"Missing Shader File", MB_OK);
		}

		return false;
	}

	m_technique = m_effect->GetTechniqueByName("ShadowBumpTechnique");
	if(!m_technique) {
		return false;
	}

	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D10_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "TEXCOORD";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D10_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D10_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	polygonLayout[2].SemanticName = "NORMAL";
	polygonLayout[2].SemanticIndex = 0;
	polygonLayout[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[2].InputSlot = 0;
	polygonLayout[2].AlignedByteOffset = D3D10_APPEND_ALIGNED_ELEMENT;
	polygonLayout[2].InputSlotClass = D3D10_INPUT_PER_VERTEX_DATA;
	polygonLayout[2].InstanceDataStepRate = 0;

	polygonLayout[3].SemanticName = "TANGENT";
	polygonLayout[3].SemanticIndex = 0;
	polygonLayout[3].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[3].InputSlot = 0;
	polygonLayout[3].AlignedByteOffset = D3D10_APPEND_ALIGNED_ELEMENT;
	polygonLayout[3].InputSlotClass = D3D10_INPUT_PER_VERTEX_DATA;
	polygonLayout[3].InstanceDataStepRate = 0;

	polygonLayout[4].SemanticName = "BINORMAL";
	polygonLayout[4].SemanticIndex = 0;
	polygonLayout[4].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[4].InputSlot = 0;
	polygonLayout[4].AlignedByteOffset = D3D10_APPEND_ALIGNED_ELEMENT;
	polygonLayout[4].InputSlotClass = D3D10_INPUT_PER_VERTEX_DATA;
	polygonLayout[4].InstanceDataStepRate = 0;

	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);
	m_technique->GetPassByIndex(0)->GetDesc(&passDesc);

	result = device->CreateInputLayout(polygonLayout, numElements, passDesc.pIAInputSignature, passDesc.IAInputSignatureSize, &m_layout);
	if(FAILED(result)) {
		return false;
	}

	m_worldMatrixPtr = m_effect->GetVariableByName("worldMatrix")->AsMatrix();
	m_viewMatrixPtr = m_effect->GetVariableByName("viewMatrix")->AsMatrix();
	m_projectionMatrixPtr = m_effect->GetVariableByName("projectionMatrix")->AsMatrix();
	m_lightViewMatrixPtr = m_effect->GetVariableByName("lightViewMatrix")->AsMatrix();
	m_lightProjectionMatrixPtr = m_effect->GetVariableByName("lightProjectionMatrix")->AsMatrix();
	m_textureArrayPtr = m_effect->GetVariableByName("shaderTextures")->AsShaderResource();
	m_depthMapTexturePtr = m_effect->GetVariableByName("depthMapTexture")->AsShaderResource();
	m_lightPositionPtr = m_effect->GetVariableByName("lightPosition")->AsVector();
	m_ambientColorPtr = m_effect->GetVariableByName("ambientColor")->AsVector();
	m_diffuseColorPtr = m_effect->GetVariableByName("diffuseColor")->AsVector();

	return true;
}

void ShadowAndBumpShader::ShutdownShader(void) {
	m_lightPositionPtr = 0;
	m_ambientColorPtr = 0;
	m_diffuseColorPtr = 0;
	m_textureArrayPtr = 0;
	m_depthMapTexturePtr = 0;
	m_lightViewMatrixPtr = 0;
	m_lightProjectionMatrixPtr = 0;
	m_worldMatrixPtr = 0;
	m_viewMatrixPtr = 0;
	m_projectionMatrixPtr = 0;
	SAFE_RELEASE(m_layout);
	m_technique = 0;
	SAFE_RELEASE(m_effect);
}

void ShadowAndBumpShader::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFilename) {
	char* compileErrors;
	unsigned long bufferSize, i;
	ofstream fout;
	compileErrors = (char*)(errorMessage->GetBufferPointer());
	bufferSize = errorMessage->GetBufferSize();
	fout.open("shader-error.txt");
	for(i=0; i<bufferSize; i++)	{
		fout << compileErrors[i];
	}
	fout.close();
	errorMessage->Release();
	errorMessage = 0;
	MessageBox(hwnd, L"Error compiling shader.  Check shader-error.txt for message.", shaderFilename, MB_OK);
}

void ShadowAndBumpShader::SetShaderParameters(D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix, D3DXMATRIX lightViewMatrix, 
											D3DXMATRIX lightProjectionMatrix, ID3D10ShaderResourceView** texture, ID3D10ShaderResourceView* depthMapTexture, 
											D3DXVECTOR3 lightPosition, D3DXVECTOR4 ambientColor, D3DXVECTOR4 diffuseColor) {
    m_worldMatrixPtr->SetMatrix((float*)&worldMatrix);
	m_viewMatrixPtr->SetMatrix((float*)&viewMatrix);
    m_projectionMatrixPtr->SetMatrix((float*)&projectionMatrix);
	m_lightViewMatrixPtr->SetMatrix((float*)&lightViewMatrix);
    m_lightProjectionMatrixPtr->SetMatrix((float*)&lightProjectionMatrix);
	m_textureArrayPtr->SetResourceArray(texture,0,2);
	m_depthMapTexturePtr->SetResource(depthMapTexture);
	m_lightPositionPtr->SetFloatVector((float*)&lightPosition);
	m_ambientColorPtr->SetFloatVector((float*)&ambientColor);
	m_diffuseColorPtr->SetFloatVector((float*)&diffuseColor);
}

void ShadowAndBumpShader::RenderShader(ID3D10Device* device, int indexCount) {
    D3D10_TECHNIQUE_DESC techniqueDesc;
	unsigned int i;
	device->IASetInputLayout(m_layout);
	m_technique->GetDesc(&techniqueDesc);

	for(i=0; i<techniqueDesc.Passes; ++i) {
        m_technique->GetPassByIndex(i)->Apply(0);
        device->DrawIndexed(indexCount, 0, 0);
    }
}
