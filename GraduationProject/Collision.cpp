#include "stdafx.h"
#include "Collision.h"
#include "GameObject.h"
#include "Shader.h"
#include "Scene.h"

CCollision::CCollision() : CGameObject(1)
{

}

void CCollision::SetCollisionMaterial(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature, ID3D12GraphicsCommandList* pd3dCommandList)
{
	CCollisionShader* pShader = new CCollisionShader();
	DXGI_FORMAT pdxgiRtvFormats[3] = { DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R8G8B8A8_UNORM };
	pShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature, 3, pdxgiRtvFormats, DXGI_FORMAT_D32_FLOAT);
	pShader->CreateShaderVariables(pd3dDevice, pd3dCommandList);

	CMaterial* pMaterial = new CMaterial(1);
	pMaterial->SetShader(pShader);
	SetMaterial(0, pMaterial);
}

CCollision::~CCollision()
{

}

////////////////////////////////

CBBCollision::CBBCollision(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, BoundingBox BB)
{
	SetBB(BB);
	SetBBMesh(pd3dDevice, pd3dCommandList);
	CCollision::SetCollisionMaterial(pd3dDevice, pd3dGraphicsRootSignature, pd3dCommandList);
}

void CBBCollision::SetBBMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	XMFLOAT3 xmf3Depth;
	XMStoreFloat3(&xmf3Depth, XMVectorScale(XMLoadFloat3(&m_xmBoundingBox.Extents), 2.0f));
	CCubeMeshDiffused* DebugBox = new CCubeMeshDiffused(pd3dDevice, pd3dCommandList, xmf3Depth.x, xmf3Depth.y, xmf3Depth.z);
	SetMesh(DebugBox);
}

void CBBCollision::SetBB(DirectX::BoundingBox& BB)
{
	XMFLOAT3 center = XMFLOAT3(0.0f, 0.0f, 0.0f);
	XMStoreFloat3(&m_xmBoundingBox.Center, XMLoadFloat3(&center));
	//XMStoreFloat3(&m_xmBoundingBox.Center, XMLoadFloat3(&BB.Center));
	XMStoreFloat3(&m_xmBoundingBox.Extents, XMLoadFloat3(&BB.Extents));
}

CBBCollision::~CBBCollision()
{

}

////////////////////////////////

CSphereCollision::CSphereCollision(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, float fradius)
{
	XMFLOAT3 center = XMFLOAT3(0.0f, 0.0f, 0.0f);
	SetBoundingSphere(center, fradius);
	
	CSphereMeshIlluminated* DebugSphere = new CSphereMeshIlluminated(pd3dDevice, pd3dCommandList, fradius);
	SetMesh(DebugSphere);

	SetCollisionMaterial(pd3dDevice, pd3dGraphicsRootSignature, pd3dCommandList);
}

void CSphereCollision::SetBoundingSphere(DirectX::XMFLOAT3& center, float fradius)
{
	XMStoreFloat3(&m_xmBoundingSphere.Center, XMLoadFloat3(&center));
	m_xmBoundingSphere.Radius = fradius;
}

CSphereCollision::~CSphereCollision()
{
}

void CSphereCollision::SetCollisionMaterial(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature, ID3D12GraphicsCommandList* pd3dCommandList)
{
	CSphereCollisionShader* pShader = new CSphereCollisionShader();
	DXGI_FORMAT pdxgiRtvFormats[3] = { DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R8G8B8A8_UNORM };
	pShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature, 3, pdxgiRtvFormats, DXGI_FORMAT_D32_FLOAT);
	pShader->CreateShaderVariables(pd3dDevice, pd3dCommandList);

	CMaterial* pMaterial = new CMaterial(1);
	pMaterial->SetShader(pShader);
	SetMaterial(0, pMaterial);
}

