#pragma once
#include "Mesh.h"

class CLine
{
public:
	XMFLOAT3 start;
	XMFLOAT3 end;

	int startIndex, endIndex;
	
	bool Compare(CLine other);
	bool CompareByIndex(CLine other);
};

class CCell
{
public:
	CLine lines[3];
	CCell* link[3]{ NULL };
	int nLink = 0;

	bool Compare(CCell other);
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
	void MakeLink(CCell* cells, int n);
};