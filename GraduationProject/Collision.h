#pragma once
#include "GameObject.h"
#include "Mesh.h"
#include "CollisionShader.h"
#include "Camera.h"
#include "Animation.h"

class CCollision : public CGameObject
{
public:
	CCollision();
	~CCollision();

	bool IsRotate() { return m_bRotate; }
	void SetIsRotate(bool bVal) { m_bRotate = bVal; }
	virtual void SetCollisionMaterial(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature, ID3D12GraphicsCommandList* pd3dCommandList);
	void SetFrameObject(CGameObject* pObject) { if (FrameObject == nullptr) FrameObject = pObject; }
	virtual void UpdateBoundings(XMFLOAT4X4 pxmf4x4World);

protected:
	bool m_bRotate = false;
	CGameObject* FrameObject = nullptr;
};

////////////////////////////////

class CBBCollision : public CCollision
{
public:
	CBBCollision(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, BoundingBox BB);
	~CBBCollision();

	void SetBBMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	void SetBB(DirectX::BoundingBox& BB);
private:
	BoundingBox m_xmBoundingBox;
};

/////////////////////////////////
class CSphereCollision : public CCollision
{
public:
	CSphereCollision(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, float fradius);
	~CSphereCollision();
	virtual void SetCollisionMaterial(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature, ID3D12GraphicsCommandList* pd3dCommandList);
	void SetBoundingSphere(DirectX::XMFLOAT3& center, float fradius);

private:
	BoundingSphere m_xmBoundingSphere;
};
