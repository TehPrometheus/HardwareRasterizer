#include "pch.h"
#include "Mesh.h"
#include <assert.h>

Mesh::Mesh(ID3D11Device* pDeviceInput, const std::vector<Vertex_PosCol>& verticesInput, const std::vector<uint32_t>& indicesInput)
{
	m_pEffect = new Effect(pDeviceInput,L"Resources/PosCol3D.fx");

	//Create vertex buffer
	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_IMMUTABLE;
	bd.ByteWidth = sizeof(Vertex_PosCol) * static_cast<uint32_t>(verticesInput.size());
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA initData = {};
	initData.pSysMem = verticesInput.data();
	
	HRESULT result = pDeviceInput->CreateBuffer(&bd, &initData, &m_pVertexBuffer);
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

Mesh::~Mesh()
{
	delete m_pEffect;
	m_pVertexBuffer->Release();
	m_pIndexBuffer->Release();
}


void Mesh::Render(ID3D11DeviceContext* pDeviceContext, Camera* pCamera) const
{
	//1. Set Matrices
	// TODO: start here. Triangle isn't appearing. Must be the worldmatrix?
	//worldmatrix
	dae::Matrix worldMatrix
	{
		Vector4{ 1,0,0, 0},
		Vector4{ 0,1,0, 0},
		Vector4{ 0,0,1, 0},
		Vector4{ 0,0,0 ,1}
	};
	dae::Matrix viewMatrix{ pCamera->GetViewMatrix().Inverse() };
	dae::Matrix inverseViewMatrix{ pCamera->GetViewMatrix() };
	dae::Matrix projectionMatrix{ pCamera->GetProjectionMatrix() };
	dae::Matrix worldViewProjectionMatrix{ worldMatrix * viewMatrix * projectionMatrix};

	m_pEffect->SetWorldViewProjectionMatrix(worldViewProjectionMatrix);

	//1. Set Primitive Topology
	pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//2. Set Input Layout
	pDeviceContext->IASetInputLayout(m_pEffect->GetInputLayoutPtr());

	//3. Set VertexBuffer
	constexpr UINT stride = sizeof(Vertex_PosCol);
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
