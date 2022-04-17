#pragma once
#include "GameObject.h"
#include "Mesh.h"
#include "CollisionShader.h"
#include "Camera.h"
#include "Animation.h"

class CCollision : public CGameObject
{
public:
	CCollision(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature,BoundingBox BB);
	~CCollision();

	bool IsRotate() { return m_bRotate; }
	void SetIsRotate(bool bVal) { m_bRotate = bVal; }

private:
	BoundingBox m_xmBoundingBox;
	bool m_bRotate = false;
};
