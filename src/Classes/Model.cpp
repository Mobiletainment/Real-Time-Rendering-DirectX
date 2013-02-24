#include "model.h"
#include "Helper.h"

Model::Model(void) { 
	m_vertexBuffer = 0;
	m_indexBuffer = 0;
	m_texture = 0;
	m_textureBump = 0;
	textures[0] = 0;
	textures[1] = 0;
	m_modelType = 0;
}

Model::~Model(void) {}

bool Model::Init(ID3D10Device* device, char* filename, WCHAR* texture) {
	bool result;
	result = LoadModel(filename);
	if(!result) {
		ERR(L"Error loading Modelfile");
		return false;
	}

	CalculateModelVectors();

	result = InitBuffers(device);
	if(!result) {
		ERR(L"Error init. Buffers");
		return false;
	}
	result = LoadTexture(device, texture);
	if(!result) {
		ERR(L"Error loading Texture");
		return false;
	}
	return true;
}


ID3D10ShaderResourceView** Model::GetTextureArray(void) {
	return textures;
}

void Model::Shutdown(void) {
	ReleaseTexture();
	ShutdownBuffers();
	ReleaseModel();
}

void Model::Render(ID3D10Device* device) {
	RenderBuffers(device);
}

bool Model::InitBuffers(ID3D10Device* device) {
	VertexType* vertices;
	unsigned long* indices;
	D3D10_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D10_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;
	int i;
	vertices = new VertexType[m_vertexCount];
	if(!vertices) {
		return false;
	}
	indices = new unsigned long[m_indexCount];
	if(!indices) {
		return false;
	}
	for(i=0; i<m_vertexCount;i++) {
		vertices[i].position = D3DXVECTOR3(m_modelType[i].x, m_modelType[i].y, m_modelType[i].z);
		vertices[i].texture = D3DXVECTOR2(m_modelType[i].tu, m_modelType[i].tv);
		vertices[i].normal = D3DXVECTOR3(m_modelType[i].nx, m_modelType[i].ny, m_modelType[i].nz);
		vertices[i].tangent = D3DXVECTOR3(m_modelType[i].tx, m_modelType[i].ty, m_modelType[i].tz);
		vertices[i].binormal = D3DXVECTOR3(m_modelType[i].bx, m_modelType[i].by, m_modelType[i].bz);

		indices[i] = i;
	}
	vertexBufferDesc.Usage = D3D10_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D10_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexData.pSysMem = vertices;
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if(FAILED(result)) {
		return false;
	}
	indexBufferDesc.Usage = D3D10_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D10_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexData.pSysMem = indices;
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if(FAILED(result)) {
		return false;
	}
	delete [] vertices;
	vertices = 0;
	delete [] indices;
	indices = 0;
	return true;
}

void Model::ShutdownBuffers(void) {
	SAFE_RELEASE(m_indexBuffer);
	SAFE_RELEASE(m_vertexBuffer);
}

void Model::RenderBuffers(ID3D10Device* device) {
	unsigned int stride;
	unsigned int offset;
	stride = sizeof(VertexType); 
	offset = 0;
	device->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);
	device->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	device->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

bool Model::LoadTexture(ID3D10Device* device, WCHAR* filename) {
	bool result;
	m_texture = new Texture;
	if(!m_texture) {
		return false;
	}
	result = m_texture->Initialize(device, filename);
	if(!result) {
		return false;
	}
	textures[0] = m_texture->GetTexture();
	return true;
}

bool Model::LoadBumpTexture(ID3D10Device* device, WCHAR* filename) {
	bool result;
	m_textureBump = new Texture;
	if(!m_texture) {
		return false;
	}
	result = m_textureBump->Initialize(device, filename);
	if(!result) {
		return false;
	}
	textures[1] = m_textureBump->GetTexture();
	return true;
}

void Model::ReleaseTexture(void) {
	if(m_texture) {
		m_texture->Shutdown();
		delete m_texture;
		m_texture = 0;
	}
	if(m_textureBump) {
		m_textureBump->Shutdown();
		delete m_textureBump;
		m_textureBump = 0;
	}
	if(textures[0])
	{
		textures[0]->Release();
		textures[0] = 0;
	}

	if(textures[1])
	{
		//TODO: FIXME
		//textures[1]->Release();
		textures[1] = 0;
	}
}

bool Model::LoadModel(char* filename) {
	ifstream fin;
	char input;
	int i;
	fin.open(filename);
	if(fin.fail()) {
		return false;
	}
	fin.get(input);
	while(input != ':') {
		fin.get(input);
	}
	fin >> m_vertexCount;
	m_indexCount = m_vertexCount;
	m_modelType = new ModelType[m_vertexCount];
	if(!m_modelType) {
		return false;
	}
	fin.get(input);
	while(input != ':')	{
		fin.get(input);
	}
	fin.get(input);
	fin.get(input);
	for(i=0; i<m_vertexCount; i++) {
		fin >> m_modelType[i].x >> m_modelType[i].y >> m_modelType[i].z;
		fin >> m_modelType[i].tu >> m_modelType[i].tv;
		fin >> m_modelType[i].nx >> m_modelType[i].ny >> m_modelType[i].nz;
	}
	fin.close();
	return true;
}

void Model::ReleaseModel(void) {
	if(m_modelType) {
		delete[] m_modelType;
		m_modelType = 0;
	}
}

void Model::SetPosition(float x,float y, float z) {
	m_positionX = x;
	m_positionY = y;
	m_positionZ = z;
} 

void Model::GetPosition(float& x, float& y, float& z) {
	x = m_positionX;
	y = m_positionY;
	z = m_positionZ;
	return;
}

void Model::CalculateModelVectors(void) {
	int faceCount, i , index;
	TempVertexType vertex1, vertex2, vertex3;
	VectorType tangent, binormal, normal;
	faceCount = m_vertexCount/3;
	index = 0;
	for(i=0;i<faceCount;i++) {
		vertex1.x = m_modelType[index].x;
		vertex1.y = m_modelType[index].y;
		vertex1.z = m_modelType[index].z;
		vertex1.tu = m_modelType[index].tu;
		vertex1.tv = m_modelType[index].tv;
		vertex1.nx = m_modelType[index].nx;
		vertex1.ny = m_modelType[index].ny;
		vertex1.nz = m_modelType[index].nz;
		index++;

		vertex2.x = m_modelType[index].x;
		vertex2.y = m_modelType[index].y;
		vertex2.z = m_modelType[index].z;
		vertex2.tu = m_modelType[index].tu;
		vertex2.tv = m_modelType[index].tv;
		vertex2.nx = m_modelType[index].nx;
		vertex2.ny = m_modelType[index].ny;
		vertex2.nz = m_modelType[index].nz;
		index++;

		vertex3.x = m_modelType[index].x;
		vertex3.y = m_modelType[index].y;
		vertex3.z = m_modelType[index].z;
		vertex3.tu = m_modelType[index].tu;
		vertex3.tv = m_modelType[index].tv;
		vertex3.nx = m_modelType[index].nx;
		vertex3.ny = m_modelType[index].ny;
		vertex3.nz = m_modelType[index].nz;
		index++;
		// Calculate the tangent and binormal of that face.
		CalculateTangentBinormal(vertex1, vertex2, vertex3, tangent, binormal);

		// Calculate the new normal using the tangent and binormal.
		CalculateNormal(tangent, binormal, normal);

		// Store the normal, tangent, and binormal for this face back in the model structure.
		m_modelType[index-1].nx = normal.x;
		m_modelType[index-1].ny = normal.y;
		m_modelType[index-1].nz = normal.z;
		m_modelType[index-1].tx = tangent.x;
		m_modelType[index-1].ty = tangent.y;
		m_modelType[index-1].tz = tangent.z;
		m_modelType[index-1].bx = binormal.x;
		m_modelType[index-1].by = binormal.y;
		m_modelType[index-1].bz = binormal.z;

		m_modelType[index-2].nx = normal.x;
		m_modelType[index-2].ny = normal.y;
		m_modelType[index-2].nz = normal.z;
		m_modelType[index-2].tx = tangent.x;
		m_modelType[index-2].ty = tangent.y;
		m_modelType[index-2].tz = tangent.z;
		m_modelType[index-2].bx = binormal.x;
		m_modelType[index-2].by = binormal.y;
		m_modelType[index-2].bz = binormal.z;

		m_modelType[index-3].nx = normal.x;
		m_modelType[index-3].ny = normal.y;
		m_modelType[index-3].nz = normal.z;
		m_modelType[index-3].tx = tangent.x;
		m_modelType[index-3].ty = tangent.y;
		m_modelType[index-3].tz = tangent.z;
		m_modelType[index-3].bx = binormal.x;
		m_modelType[index-3].by = binormal.y;
		m_modelType[index-3].bz = binormal.z;
	}
	return;
}

void Model::CalculateTangentBinormal(TempVertexType vertex1, TempVertexType vertex2, TempVertexType vertex3, VectorType& tangent, VectorType& binormal) {
	float vector1[3], vector2[3];
	float tuVector[2], tvVector[2];
	float den;
	float length;
	// Calculate the two vectors for this face.
	vector1[0] = vertex2.x - vertex1.x;
	vector1[1] = vertex2.y - vertex1.y;
	vector1[2] = vertex2.z - vertex1.z;

	vector2[0] = vertex3.x - vertex1.x;
	vector2[1] = vertex3.y - vertex1.y;
	vector2[2] = vertex3.z - vertex1.z;

	// Calculate the tu and tv texture space vectors.
	tuVector[0] = vertex2.tu - vertex1.tu;
	tvVector[0] = vertex2.tv - vertex1.tv;

	tuVector[1] = vertex3.tu - vertex1.tu;
	tvVector[1] = vertex3.tv - vertex1.tv;

	// Calculate the denominator of the tangent/binormal equation.
	den = 1.0f / (tuVector[0] * tvVector[1] - tuVector[1] * tvVector[0]);

	// Calculate the cross products and multiply by the coefficient to get the tangent and binormal.
	tangent.x = (tvVector[1] * vector1[0] - tvVector[0] * vector2[0]) * den;
	tangent.y = (tvVector[1] * vector1[1] - tvVector[0] * vector2[1]) * den;
	tangent.z = (tvVector[1] * vector1[2] - tvVector[0] * vector2[2]) * den;

	binormal.x = (tuVector[0] * vector2[0] - tuVector[1] * vector1[0]) * den;
	binormal.y = (tuVector[0] * vector2[1] - tuVector[1] * vector1[1]) * den;
	binormal.z = (tuVector[0] * vector2[2] - tuVector[1] * vector1[2]) * den;

	// Calculate the length of this normal.
	length = sqrt((tangent.x * tangent.x) + (tangent.y * tangent.y) + (tangent.z * tangent.z));
			
	// Normalize the normal and then store it
	tangent.x = tangent.x / length;
	tangent.y = tangent.y / length;
	tangent.z = tangent.z / length;

	// Calculate the length of this normal.
	length = sqrt((binormal.x * binormal.x) + (binormal.y * binormal.y) + (binormal.z * binormal.z));
			
	// Normalize the normal and then store it
	binormal.x = binormal.x / length;
	binormal.y = binormal.y / length;
	binormal.z = binormal.z / length;

	return;
}

void Model::CalculateNormal(VectorType tangent, VectorType binormal, VectorType& normal) {
	float length;
	// Calculate the cross product of the tangent and binormal which will give the normal vector.
	normal.x = (tangent.y * binormal.z) - (tangent.z * binormal.y);
	normal.y = (tangent.z * binormal.x) - (tangent.x * binormal.z);
	normal.z = (tangent.x * binormal.y) - (tangent.y * binormal.x);

	// Calculate the length of the normal.
	length = sqrt((normal.x * normal.x) + (normal.y * normal.y) + (normal.z * normal.z));

	// Normalize the normal.
	normal.x = normal.x / length;
	normal.y = normal.y / length;
	normal.z = normal.z / length;

	return;
}