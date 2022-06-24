#include "NavMesh.h"


bool CLine::Compare(CLine other)
{
	if (Vector3::Distance(start, other.start) <= 0.0001) {
		if (Vector3::Distance(end, other.end) <= 0.0001)
			return true;
	}
	else if (Vector3::Distance(end, other.start) <= 0.0001) {
		if (Vector3::Distance(start, other.end) <= 0.0001)
			return true;
	}
	return false;
}

bool CLine::CompareByIndex(CLine other)
{
	if ((startIndex == other.startIndex) && (endIndex == other.endIndex)) return true;
	else if ((startIndex == other.endIndex) && (endIndex == other.startIndex)) return true;
	return false;
}

bool CCell::IsLinked(CCell* other)
{
	if (this == other) return true;
	for (int i = 0; i < nLink; ++i) {
		if (link[i] == other)
			return true;
	}
	return false;
}

bool CCell::IsConnected(CCell other)
{
	for (int i = 0; i < 3; ++i) {
		for (int j = 0; j < 3; ++j) {
			if (lines[i].Compare(other.lines[j]))
				return true;
		}
	}
	return false;
}

bool CCell::IsSame(CCell other)
{
	int n = 0;
	for (int i = 0; i < 3; ++i) {
		for (int j = 0; j < 3; ++j) {
			if (lines[i].Compare(other.lines[j])) {
				n++;
				break;
			}
		}
	}
	if (n == 3) return true;
	else return false;
}

CNavMesh::CNavMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, XMFLOAT3 xmf3Scale, bool uniqued) : CMesh(pd3dDevice, pd3dCommandList)
{
	if (uniqued) {
		ifstream meshInfo("../Assets/Image/Terrain/LinkedNavMeshCells.txt");

		string s;
		while (meshInfo >> s) {
			if (s.compare("total:") == 0) {
				int nCell;
				meshInfo >> nCell;
				m_nVertices = nCell * 3;
				m_pxmf3Positions = new XMFLOAT3[m_nVertices];
				break;
			}
		}

		m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		m_xmf3Scale = xmf3Scale;

		vector<CCell> vCells;

		int id;
		float fx1, fy1, fz1, fx2, fy2, fz2, fx3, fy3, fz3;
		for (int i = 0; i < m_nVertices;) {
			meshInfo >> s >> id >> fx1 >> fy1 >> fz1 >> fx2 >> fy2 >> fz2 >> fx3 >> fy3 >> fz3;
			if (s.compare("cell") != 0) break;
			m_pxmf3Positions[i++] = XMFLOAT3(fx1, fy1, fz1);
			m_pxmf3Positions[i++] = XMFLOAT3(fx2, fy2, fz2);
			m_pxmf3Positions[i++] = XMFLOAT3(fx3, fy3, fz3);

			CCell cell;
			cell.id = id;
			cell.lines[0].start = XMFLOAT3(fx1, fy1, fz1);
			cell.lines[0].end = XMFLOAT3(fx2, fy2, fz2);
			cell.lines[1].start = XMFLOAT3(fx2, fy2, fz2);
			cell.lines[1].end = XMFLOAT3(fx3, fy3, fz3);
			cell.lines[2].start = XMFLOAT3(fx3, fy3, fz3);
			cell.lines[2].end = XMFLOAT3(fx1, fy1, fz1);

			vCells.push_back(cell);
		}

		m_pd3dPositionBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pxmf3Positions, sizeof(XMFLOAT3) * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dPositionUploadBuffer);

		m_d3dPositionBufferView.BufferLocation = m_pd3dPositionBuffer->GetGPUVirtualAddress();
		m_d3dPositionBufferView.StrideInBytes = sizeof(XMFLOAT3);
		m_d3dPositionBufferView.SizeInBytes = sizeof(XMFLOAT3) * m_nVertices;

		int nlink, idx;
		for (int i = 0; i < vCells.size(); ++i) {
			meshInfo >> id >> nlink;

			vCells[id].nLink = nlink;
			for (int j = 0; j < nlink; ++j) {
				meshInfo >> idx;
				vCells[id].link.push_back(&vCells[idx]);
			}
		}

		m_NavCells = vCells;
	}
	else {
		ifstream meshInfo("../Assets/Image/Terrain/SampleScene Exported NavMesh.txt");

		string s;
		while (meshInfo >> s) {
			if (s.compare("vCount") == 0) {
				meshInfo >> m_nVertices;
				m_pxmf3Positions = new XMFLOAT3[m_nVertices];
				break;
			}
		}

		m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		m_xmf3Scale = xmf3Scale;

		float fx, fy, fz;
		for (int i = 0; i < m_nVertices; ++i) {
			meshInfo >> s >> fx >> fy >> fz;
			if (s.compare("v") != 0) break;
			m_pxmf3Positions[i] = XMFLOAT3((fx * m_xmf3Scale.x * 220.0f / 150.0f) + 1000.0f, 50, (fz * m_xmf3Scale.z * 220.0f / 150.0f) + 1000.0f);
		}

		m_pd3dPositionBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pxmf3Positions, sizeof(XMFLOAT3) * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dPositionUploadBuffer);

		m_d3dPositionBufferView.BufferLocation = m_pd3dPositionBuffer->GetGPUVirtualAddress();
		m_d3dPositionBufferView.StrideInBytes = sizeof(XMFLOAT3);
		m_d3dPositionBufferView.SizeInBytes = sizeof(XMFLOAT3) * m_nVertices;

		m_nSubMeshes = 1;
		m_pnSubSetIndices = new int[m_nSubMeshes];
		m_ppnSubSetIndices = new UINT * [m_nSubMeshes];

		m_ppd3dSubSetIndexBuffers = new ID3D12Resource * [m_nSubMeshes];
		m_ppd3dSubSetIndexUploadBuffers = new ID3D12Resource * [m_nSubMeshes];
		m_pd3dSubSetIndexBufferViews = new D3D12_INDEX_BUFFER_VIEW[m_nSubMeshes];

		CCell* cells = NULL;

		while (meshInfo >> s) {
			if (s.compare("fCount") == 0) {
				meshInfo >> m_pnSubSetIndices[0];
				cells = new CCell[m_pnSubSetIndices[0]]{};
				m_pnSubSetIndices[0] *= 3;
				m_ppnSubSetIndices[0] = new UINT[m_pnSubSetIndices[0]];
				break;
			}
		}

		int k = 0;
		int idx1, idx2, idx3;
		for (int i = 0; i < m_pnSubSetIndices[0];) {
			meshInfo >> s >> idx1 >> idx2 >> idx3;
			if (s.compare("f") != 0) break;
			m_ppnSubSetIndices[0][i++] = idx1;
			m_ppnSubSetIndices[0][i++] = idx2;
			m_ppnSubSetIndices[0][i++] = idx3;
			cells[k].lines[0].start = m_pxmf3Positions[idx1];
			cells[k].lines[0].end = m_pxmf3Positions[idx2];
			cells[k].lines[1].start = m_pxmf3Positions[idx2];
			cells[k].lines[1].end = m_pxmf3Positions[idx3];
			cells[k].lines[2].start = m_pxmf3Positions[idx3];
			cells[k].lines[2].end = m_pxmf3Positions[idx1];
			cells[k].id = k;
			k++;
		}

		vector<CCell> vCells = CheckCells(cells, k);
		MakeLink(vCells);
		SaveCells();

		m_ppd3dSubSetIndexBuffers[0] = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_ppnSubSetIndices[0], sizeof(UINT) * m_pnSubSetIndices[0], D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER, &m_ppd3dSubSetIndexUploadBuffers[0]);

		m_pd3dSubSetIndexBufferViews[0].BufferLocation = m_ppd3dSubSetIndexBuffers[0]->GetGPUVirtualAddress();
		m_pd3dSubSetIndexBufferViews[0].Format = DXGI_FORMAT_R32_UINT;
		m_pd3dSubSetIndexBufferViews[0].SizeInBytes = sizeof(UINT) * m_pnSubSetIndices[0];

		delete[] cells;
	}
}

CNavMesh::~CNavMesh()
{
}

vector<CCell> CNavMesh::CheckCells(CCell* cell, int n)
{
	vector<CCell> result;
	result.push_back(cell[0]);

	for (int i = 0; i < n; ++i) {
		bool same = true;
		for (int j = 0; j < result.size(); ++j) {
			same = result[j].IsSame(cell[i]);
			if (same)
				break;
		}
		if (!same) result.push_back(cell[i]);
	}

	return result;
}

void CNavMesh::MakeLink(vector<CCell> vCells)
{
	for (int i = 0; i < vCells.size(); ++i) {
		for (int j = 0; j < vCells.size(); ++j) {
			if (i != j) {
				if (!vCells[i].IsLinked(&vCells[j])) {
					if (vCells[i].IsConnected(vCells[j])) {
						vCells[i].link.push_back(&vCells[j]);
						vCells[j].link.push_back(&vCells[i]);
						vCells[i].nLink++;
						vCells[j].nLink++;
					}
				}
			}
		}
	}

	m_NavCells = vCells;
}

void CNavMesh::SaveCells()
{
	vector<CCell> cells = m_NavCells;

	ofstream out("../Assets/Image/Terrain/LinkedNavMeshCells.txt");
	out << "total: " << cells.size() << endl;
	for (int i = 0; i < cells.size(); ++i) {
		//out << "(" << cells[i].lines[0].start.x << ", " << cells[i].lines[0].start.y << ", " << cells[i].lines[0].start.z << "),"
		//	<< "(" << cells[i].lines[1].start.x << ", " << cells[i].lines[1].start.y << ", " << cells[i].lines[1].start.z << "),"
		//	<< "(" << cells[i].lines[2].start.x << ", " << cells[i].lines[2].start.y << ", " << cells[i].lines[2].start.z << ")"
		//	<< endl;
		out << "cell " << cells[i].id << " ";
		out << cells[i].lines[0].start.x << " " << cells[i].lines[0].start.y << " " << cells[i].lines[0].start.z << " "
			<< cells[i].lines[1].start.x << " " << cells[i].lines[1].start.y << " " << cells[i].lines[1].start.z << " "
			<< cells[i].lines[2].start.x << " " << cells[i].lines[2].start.y << " " << cells[i].lines[2].start.z << " ";
		
		out << endl;
	}

	for (int i = 0; i < cells.size(); ++i) {
		out << cells[i].id << " " << cells[i].nLink << " ";
		for (auto cell : cells[i].link) {
			out << cell->id << " ";
		}
		out << endl;
	}
}
