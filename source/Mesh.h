#pragma once
#include "DataTypes.h"
#include "Effect.h"

struct Vertex_PosCol
{
	dae::Vector3 position{};
	dae::ColorRGB color{ dae::colors::White };
};


class Mesh final
{
public:


	Mesh(ID3D11Device* pDeviceInput, const std::vector<Vertex_PosCol>& verticesInput, const std::vector<uint32_t>& indicesInput);
	~Mesh();

	// -----------------------------------------------
	// Copy/move constructors and assignment operators
	// -----------------------------------------------
	Mesh(const Mesh& other)					= default;
	Mesh(Mesh&& other) noexcept				= default;
	Mesh& operator=(const Mesh& other)		= default;
	Mesh& operator=(Mesh&& other) noexcept	= default;

	//------------------------------------------------
	// Member functions						
	//------------------------------------------------
	void Render(ID3D11DeviceContext* pDeviceContext) const;

private:

	//------------------------------------------------
	// Member variables						
	//------------------------------------------------
	Effect* m_pEffect{};
	ID3D11InputLayout* m_pTechnique{};
	ID3D11Buffer* m_pVertexBuffer{};
	ID3D11Buffer* m_pIndexBuffer{};
	uint32_t m_NumIndices{};
};

