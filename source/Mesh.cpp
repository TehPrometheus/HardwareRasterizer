#include "pch.h"
#include "Mesh.h"
#include "Effect_PosTex.h"
#include <assert.h>
//TODO: start from slide 15. Create the other sampling methods. Or make these mesh constructors templated.
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

Mesh::Mesh(ID3D11Device* pDeviceInput, const std::vector<Vertex_PosTex>& verticesInput, const std::vector<uint32_t>& indicesInput, const std::string& diffuseMapPath)
{
	m_pEffect = new Effect_PosTex(pDeviceInput,L"Resources/PosTex3D.fx");


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
	bd.ByteWidth = sizeof(Vertex_PosTex) * static_cast<uint32_t>(verticesInput.size());
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


void Mesh::Render(ID3D11DeviceContext* pDeviceContext, Camera* pCamera) const
{
	//1. Set Matrices
	dae::Matrix worldMatrix
	{
		Vector4{1,0,0,0},
		Vector4{0,1,0,0},
		Vector4{0,0,1,0},
		Vector4{0,0,0,1}
	};
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
