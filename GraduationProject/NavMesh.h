#pragma once
#include "Mesh.h"

class CLine
{
public:
	XMFLOAT3 start;
	XMFLOAT3 end;

	int startIndex, endIndex;
	
	bool Compare(CLine other) {
		if (Vector3::Distance(start, other.start) <= 0) {
			if (Vector3::Distance(end, other.end) <= 0)
				return true;
		}
		else if (Vector3::Distance(end, other.start) <= 0) {
			if (Vector3::Distance(start, other.end) <= 0)
				return true;
		}
	}

	bool CompareByIndex(CLine other) {
		if ((startIndex == other.startIndex) && (endIndex == other.endIndex)) return true;
		else if ((startIndex == other.endIndex) && (endIndex == other.startIndex)) return true;
	}
};

class CCell
{
public:
	CLine lines[3];
	CCell* link[3];

	bool Compare(CCell* other) {
		for (int i = 0; i < 3; ++i) {
			for (int j = 0; j < 3; ++j) {
				if (lines[i].CompareByIndex(other->lines[j]))
					return true;
			}
		}
		return false;
	}
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