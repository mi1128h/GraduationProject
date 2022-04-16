#include "stdafx.h"
#include "Collision.h"
#include "GameObject.h"
#include "Shader.h"
#include "Scene.h"

CCollision::CCollision(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature,
	XMFLOAT3 xmf3Min, XMFLOAT3 xmf3Max, XMFLOAT3 xmf3Center, XMFLOAT4X4 mxf4x4World) : CGameObject(1)
{
	XMStoreFloat3(&m_xmBoundingBox.Center, XMLoadFloat3(&xmf3Center));
	XMFLOAT3 xmf3Depth = Vector3::Subtract(xmf3Max, xmf3Min);
	XMStoreFloat3(&m_xmBoundingBox.Extents, XMVectorScale(XMLoadFloat3(&xmf3Depth), 0.5f));


	CCubeMeshTextured* DebugBox = new CCubeMeshTextured(pd3dDevice, pd3dCommandList, xmf3Depth.x, xmf3Depth.y, xmf3Depth.z);
	SetMesh(DebugBox);
	
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

