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
	int id;

	XMFLOAT3 center;
	CLine lines[3]{};
	float fArrivCost[3];
	
	vector<CCell*> link;
	int nLink = 0;

	bool IsLinked(CCell* other);
	bool IsConnected(CCell other);
	bool IsSame(CCell other);
};

class CNavMesh : public CMesh
{
protected:
	XMFLOAT3						m_xmf3Scale;
	vector<CCell> m_NavCells;

public:
	CNavMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, XMFLOAT3 xmf3Scale = XMFLOAT3(1.0f, 1.0f, 1.0f), bool uniqued = false);
	virtual ~CNavMesh();

	XMFLOAT3 GetScale() { return(m_xmf3Scale); }

	vector<CCell> CheckCells(CCell* cell, int n);
	void MakeLink(vector<CCell> cells);
	void SaveCells();

	void CalculateCells();

};