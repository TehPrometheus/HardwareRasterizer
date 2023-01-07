#pragma once
#include "DataTypes.h"
#include "Effect.h"
#include "Camera.h"
#include <fstream>

struct Vertex_PosCol
{
	dae::Vector3 position{};
	dae::ColorRGB color{ dae::colors::White };
};

struct Vertex_PosTex
{
	dae::Vector3 position{};
	dae::Vector2 uv{};
};

class Mesh final
{
public:
	Mesh(ID3D11Device* pDeviceInput, const std::vector<Vertex_PosCol>& verticesInput, const std::vector<uint32_t>& indicesInput);
	Mesh(ID3D11Device* pDeviceInput, const std::string& objPath, const std::string& diffuseMapPath);
	~Mesh();

	// -----------------------------------------------
	// Copy/move constructors and assignment operators
	// -----------------------------------------------
	Mesh(const Mesh& other)					= default;
	Mesh(Mesh&& other) noexcept				= default;
	Mesh& operator=(const Mesh& other)		= default;
	Mesh& operator=(Mesh&& other) noexcept	= default;

	//------------------------------------------------
	// Public member functions						
	//------------------------------------------------
	void Update(const Timer* pTimer);
	void Render(ID3D11DeviceContext* pDeviceContext, Camera* pCamera) const;
	ID3D11InputLayout* GetInputLayoutPtr();
	Effect* GetEffectPtr() const;
	void ToggleRotation();

private:

	//------------------------------------------------
	// Member variables						
	//------------------------------------------------
	float m_VehicleYaw{};
	float m_AccuSec{};
	bool m_IsRotating{ true };

	Effect* m_pEffect{};
	ID3D11InputLayout* m_pInputLayout{};

	ID3D11Buffer* m_pVertexBuffer{};
	ID3D11Buffer* m_pIndexBuffer{};
	uint32_t m_NumIndices{};

	Texture* m_pDiffuseMap{nullptr};

	std::vector<Vertex_PosTex> m_Vertices{};
	std::vector<uint32_t> m_Indices{};

	void ParsePosTex(const std::string& filename);

	bool ParseObj(const std::string& filename, std::vector<Vertex_In>& vertices, std::vector<uint32_t>& indices, bool flipAxisAndWinding = true);
};

