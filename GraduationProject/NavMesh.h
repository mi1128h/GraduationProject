#pragma once
#include "Mesh.h"
#include <set>

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
	vector<float> C2CCost;
	float gCost;
	float hCost;
	float fCost;
	int parentId = -1;

	vector<int> linkIdx;
	int nLink = 0;

	bool IsLinked(CCell* other);
	bool IsConnected(CCell other);
	bool IsSame(CCell other);
};

class CNavMesh : public CMesh
{
protected:
	XMFLOAT3 m_xmf3Scale;
	vector<CCell> m_NavCells;

	list<CCell*> closed;
	list<CCell*> open;

public:
	CNavMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, XMFLOAT3 xmf3Scale = XMFLOAT3(1.0f, 1.0f, 1.0f), void* pContext = NULL, bool uniqued = false);
	virtual ~CNavMesh();

	XMFLOAT3 GetScale() { return(m_xmf3Scale); }

	vector<CCell> CheckCells(CCell* cell, int n);
	void MakeLink(vector<CCell> cells);
	void SaveCells();

	void CalculateCells();

	CCell* FindCell(XMFLOAT3 xmf3Position);
	bool PointInCell(CCell* cell, XMFLOAT3 xmf3Position);

	list<int> MakePath(CCell* curCell, XMFLOAT3 xmf3Position);

	CCell GetCell(int n) { return m_NavCells[n]; }
};