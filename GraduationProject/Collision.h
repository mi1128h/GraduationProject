#pragma once
#include "GameObject.h"
#include "Mesh.h"
#include "CollisionShader.h"
#include "Camera.h"
#include "Animation.h"

class CCollision : public CGameObject
{
public:
	CCollision(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature,
		XMFLOAT3 xmf3Min, XMFLOAT3 xmf3Max, XMFLOAT3 xmf3Center,XMFLOAT4X4 mxf4x4World);
	~CCollision();

private:
	BoundingBox m_xmBoundingBox;
};
