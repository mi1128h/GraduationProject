#pragma once
#include "Mesh.h"

class CLine
{
	XMFLOAT3 start;
	XMFLOAT3 end;
};

class CCell
{
	CLine lines[3];
	CCell* link[3];
};

class CNavMesh : public CMesh
{
protected:
	XMFLOAT3						m_xmf3Scale;
	CCell** m_NavCells = NULL;

public:
	CNavMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, XMFLOAT3 xmf3Scale = XMFLOAT3(1.0f, 1.0f, 1.0f));
	virtual ~CNavMesh();

	XMFLOAT3 GetScale() { return(m_xmf3Scale); }
};