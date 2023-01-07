#include "pch.h"
#include "Mesh.h"
#include "Effect_PosTex.h"
#include <assert.h>

Mesh::Mesh(ID3D11Device* pDeviceInput, const std::vector<Vertex_PosCol>& verticesInput, const std::vector<uint32_t>& indicesInput)
{
	m_pEffect = new Effect(pDeviceInput,L"Resources/PosCol3D.fx");

	//Create Vertex Layout
	static constexpr uint32_t numElements{ 2 };
	D3D11_INPUT_ELEMENT_DESC vertexDesc[numElements]{};

	vertexDesc[0].SemanticName = "POSITION";
	vertexDesc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	vertexDesc[0].AlignedByteOffset = 0;
	vertexDesc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	vertexDesc[1].SemanticName = "COLOR";
	vertexDesc[1].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	vertexDesc[1].AlignedByteOffset = 12;
	vertexDesc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	//Create Input Layout
	D3DX11_PASS_DESC passDesc{};
	m_pEffect->GetTechniquePtr()->GetPassByIndex(0)->GetDesc(&passDesc);

	HRESULT result = pDeviceInput->CreateInputLayout(
		vertexDesc,
		numElements,
		passDesc.pIAInputSignature,
		passDesc.IAInputSignatureSize,
		&m_pInputLayout);

	if (FAILED(result))
	{
		assert(false && "Unable to create input layout in constructor of Mesh class");
	}


	//Create vertex buffer
	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_IMMUTABLE;
	bd.ByteWidth = sizeof(Vertex_PosCol) * static_cast<uint32_t>(verticesInput.size());
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA initData = {};
	initData.pSysMem = verticesInput.data();
	
	result = pDeviceInput->CreateBuffer(&bd, &initData, &m_pVertexBuffer);
	if (FAILED(result))
	{
		assert(false && "Unable to create vertex buffer in constructor of Mesh class");
	}

	//Create index buffer
	m_NumIndices = static_cast<uint32_t>(indicesInput.size());
	bd.Usage = D3D11_USAGE_IMMUTABLE;
	bd.ByteWidth = sizeof(uint32_t) * m_NumIndices;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	
	initData.pSysMem = indicesInput.data();

	result = pDeviceInput->CreateBuffer(&bd, &initData, &m_pIndexBuffer);
	if (FAILED(result))
	{
		assert(false && "Unable to create index buffer in constructor of Mesh class");
	}
}

Mesh::Mesh(ID3D11Device* pDeviceInput, const std::string& objPath, const std::string& diffuseMapPath)
{
	m_pEffect = new Effect_PosTex(pDeviceInput,L"Resources/PosTex3D.fx");

	//Parse OBJ
	ParsePosTex(objPath);

	//Create Vertex Layout
	static constexpr uint32_t numElements{ 2 };
	D3D11_INPUT_ELEMENT_DESC vertexDesc[numElements]{};

	vertexDesc[0].SemanticName = "POSITION";
	vertexDesc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	vertexDesc[0].AlignedByteOffset = 0;
	vertexDesc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	vertexDesc[1].SemanticName = "TEXCOORD";
	vertexDesc[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	vertexDesc[1].AlignedByteOffset = 12;
	vertexDesc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	//Create Input Layout
	D3DX11_PASS_DESC passDesc{};
	m_pEffect->GetTechniquePtr()->GetPassByIndex(0)->GetDesc(&passDesc);

	HRESULT result = pDeviceInput->CreateInputLayout(
		vertexDesc,
		numElements,
		passDesc.pIAInputSignature,
		passDesc.IAInputSignatureSize,
		&m_pInputLayout);

	if (FAILED(result))
	{
		assert(false && "Unable to create input layout in constructor of Mesh class");
	}

	//Create vertex buffer
	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_IMMUTABLE;
	bd.ByteWidth = sizeof(Vertex_PosTex) * static_cast<uint32_t>(m_Vertices.size());
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA initData = {};
	initData.pSysMem = m_Vertices.data();
	
	result = pDeviceInput->CreateBuffer(&bd, &initData, &m_pVertexBuffer);
	if (FAILED(result))
	{
		assert(false && "Unable to create vertex buffer in constructor of Mesh class");
	}

	//Create index buffer
	m_NumIndices = static_cast<uint32_t>(m_Indices.size());
	bd.Usage = D3D11_USAGE_IMMUTABLE;
	bd.ByteWidth = sizeof(uint32_t) * m_NumIndices;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	
	initData.pSysMem = m_Indices.data();

	result = pDeviceInput->CreateBuffer(&bd, &initData, &m_pIndexBuffer);
	if (FAILED(result))
	{
		assert(false && "Unable to create index buffer in constructor of Mesh class");
	}

	m_pDiffuseMap = new dae::Texture(pDeviceInput, diffuseMapPath.c_str());
	m_pEffect->SetDiffuseMap(m_pDiffuseMap);

}

Mesh::~Mesh()
{
	delete m_pEffect;
	delete m_pDiffuseMap;
	m_pVertexBuffer->Release();
	m_pIndexBuffer->Release();
	m_pInputLayout->Release();
}


void Mesh::Update(const Timer* pTimer)
{
	if (m_IsRotating)
	{
		m_AccuSec += pTimer->GetElapsed();
	}

	m_VehicleYaw = PI_DIV_2 * m_AccuSec;
}

void Mesh::Render(ID3D11DeviceContext* pDeviceContext, Camera* pCamera) const
{
	//1. Set Matrices
	dae::Matrix worldMatrix = Matrix::CreateRotationY(m_VehicleYaw);
	dae::Matrix viewMatrix{ pCamera->GetViewMatrix().Inverse() };
	dae::Matrix inverseViewMatrix{ pCamera->GetViewMatrix() };
	dae::Matrix projectionMatrix{ pCamera->GetProjectionMatrix() };
	dae::Matrix worldViewProjectionMatrix{ worldMatrix * viewMatrix * projectionMatrix};

	m_pEffect->SetWorldViewProjectionMatrix(worldViewProjectionMatrix);

	//1. Set Primitive Topology
	pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//2. Set Input Layout
	pDeviceContext->IASetInputLayout(m_pInputLayout);

	//3. Set VertexBuffer
	UINT stride{};

	if (dynamic_cast<Effect_PosTex*>(m_pEffect))
	{
		stride = sizeof(Vertex_PosTex);
	}
	
	constexpr UINT offset = 0;
	pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);

	//4. Set IndexBuffer
	pDeviceContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	//5. Draw
	D3DX11_TECHNIQUE_DESC techDesc{};
	m_pEffect->GetTechniquePtr()->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; p++)
	{
		m_pEffect->GetTechniquePtr()->GetPassByIndex(p)->Apply(0, pDeviceContext);
		pDeviceContext->DrawIndexed(m_NumIndices, 0, 0);
	}


}

ID3D11InputLayout* Mesh::GetInputLayoutPtr()
{
	return m_pInputLayout;
}

Effect* Mesh::GetEffectPtr() const
{
	return m_pEffect;
}

void Mesh::ToggleRotation()
{
	m_IsRotating = !m_IsRotating;

	std::cout << "Rotation is ";
	if (m_IsRotating)
	{
		std::cout << "enabled\n";
	}
	else
	{
		std::cout << "disabled\n";
	}
}


void Mesh::ParsePosTex(const std::string& filename)
{
	std::vector<Vertex_In> parserVertices{};

	ParseObj(filename, parserVertices, m_Indices);

	for (const Vertex_In& vert : parserVertices)
	{
		Vertex_PosTex v{};
		v.position = vert.position;
		v.uv = vert.uv;

		m_Vertices.push_back(v);
	}

}

bool Mesh::ParseObj(const std::string& filename, std::vector<Vertex_In>& vertices, std::vector<uint32_t>& indices, bool flipAxisAndWinding)
{
	std::ifstream file(filename);
	if (!file)
		return false;

	std::vector<Vector3> positions{};
	std::vector<Vector3> normals{};
	std::vector<Vector2> UVs{};

	vertices.clear();
	indices.clear();

	std::string sCommand;
	// start a while iteration ending when the end of file is reached (ios::eof)
	while (!file.eof())
	{
		//read the first word of the string, use the >> operator (istream::operator>>) 
		file >> sCommand;
		//use conditional statements to process the different commands	
		if (sCommand == "#")
		{
			// Ignore Comment
		}
		else if (sCommand == "v")
		{
			//Vertex
			float x, y, z;
			file >> x >> y >> z;

			positions.emplace_back(x, y, z);
		}
		else if (sCommand == "vt")
		{
			// Vertex TexCoord
			float u, v;
			file >> u >> v;
			UVs.emplace_back(u, 1 - v);
		}
		else if (sCommand == "vn")
		{
			// Vertex Normal
			float x, y, z;
			file >> x >> y >> z;

			normals.emplace_back(x, y, z);
		}
		else if (sCommand == "f")
		{
			//if a face is read:
			//construct the 3 vertices, add them to the vertex array
			//add three indices to the index array
			//add the material index as attibute to the attribute array
			//
			// Faces or triangles
			Vertex_In vertex{};
			size_t iPosition, iTexCoord, iNormal;

			uint32_t tempIndices[3];
			for (size_t iFace = 0; iFace < 3; iFace++)
			{
				// OBJ format uses 1-based arrays
				file >> iPosition;
				vertex.position = positions[iPosition - 1];

				if ('/' == file.peek())//is next in buffer ==  '/' ?
				{
					file.ignore();//read and ignore one element ('/')

					if ('/' != file.peek())
					{
						// Optional texture coordinate
						file >> iTexCoord;
						vertex.uv = UVs[iTexCoord - 1];
					}

					if ('/' == file.peek())
					{
						file.ignore();

						// Optional vertex normal
						file >> iNormal;
						vertex.normal = normals[iNormal - 1];
					}
				}

				vertices.push_back(vertex);
				tempIndices[iFace] = uint32_t(vertices.size()) - 1;
				//indices.push_back(uint32_t(vertices.size()) - 1);
			}

			indices.push_back(tempIndices[0]);
			if (flipAxisAndWinding)
			{
				indices.push_back(tempIndices[2]);
				indices.push_back(tempIndices[1]);
			}
			else
			{
				indices.push_back(tempIndices[1]);
				indices.push_back(tempIndices[2]);
			}
		}
		//read till end of line and ignore all remaining chars
		file.ignore(1000, '\n');
	}

	//Cheap Tangent Calculations
	for (uint32_t i = 0; i < indices.size(); i += 3)
	{
		uint32_t index0 = indices[i];
		uint32_t index1 = indices[size_t(i) + 1];
		uint32_t index2 = indices[size_t(i) + 2];

		const Vector3& p0 = vertices[index0].position;
		const Vector3& p1 = vertices[index1].position;
		const Vector3& p2 = vertices[index2].position;
		const Vector2& uv0 = vertices[index0].uv;
		const Vector2& uv1 = vertices[index1].uv;
		const Vector2& uv2 = vertices[index2].uv;

		const Vector3 edge0 = p1 - p0;
		const Vector3 edge1 = p2 - p0;
		const Vector2 diffX = Vector2(uv1.x - uv0.x, uv2.x - uv0.x);
		const Vector2 diffY = Vector2(uv1.y - uv0.y, uv2.y - uv0.y);
		float r = 1.f / Vector2::Cross(diffX, diffY);

		Vector3 tangent = (edge0 * diffY.y - edge1 * diffY.x) * r;
		vertices[index0].tangent += tangent;
		vertices[index1].tangent += tangent;
		vertices[index2].tangent += tangent;
	}

	//Create the Tangents (reject)
	for (auto& v : vertices)
	{
		v.tangent = Vector3::Reject(v.tangent, v.normal).Normalized();

		if (flipAxisAndWinding)
		{
			v.position.z *= -1.f;
			v.normal.z *= -1.f;
			v.tangent.z *= -1.f;
		}

	}

	return true;
}


