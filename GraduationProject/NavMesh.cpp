#include "NavMesh.h"

CNavMesh::CNavMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, XMFLOAT3 xmf3Scale) : CMesh(pd3dDevice, pd3dCommandList)
{
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

	float fHeight = 0.0f, fMinHeight = +FLT_MAX, fMaxHeight = -FLT_MAX;

	float x, y, z;
	for (int i = 0; i < m_nVertices; ++i) {
		meshInfo >> s >> x >> y >> z;
		if (s.compare("v") != 0) break;
		m_pxmf3Positions[i] = XMFLOAT3((x * m_xmf3Scale.x * 220.0f / 150.0f) + 1000.0f, 100, (z * m_xmf3Scale.z * 220.0f / 150.0f) + 1000.0f);
		//m_pxmf3Positions[i] = XMFLOAT3(x, y, z);
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
			cells = new CCell[m_pnSubSetIndices[0]];
			m_pnSubSetIndices[0] *= 3;
			m_ppnSubSetIndices[0] = new UINT[m_pnSubSetIndices[0]];
			break;
		}
	}

	int k = 0;
	for (int i = 0; i < m_pnSubSetIndices[0];) {
		meshInfo >> s >> x >> y >> z;
		if (s.compare("f") != 0) break;
		m_ppnSubSetIndices[0][i++] = x - 1;
		m_ppnSubSetIndices[0][i++] = y - 1;
		m_ppnSubSetIndices[0][i++] = z - 1;
		cells[k].lines[0].startIndex = x - 1;
		cells[k].lines[0].endIndex = y - 1;
		cells[k].lines[1].startIndex = y - 1;
		cells[k].lines[1].endIndex = z - 1;
		cells[k].lines[2].startIndex = z - 1;
		cells[k].lines[2].endIndex = x - 1;
		k++;
	}

	MakeLink(cells, k);

	m_ppd3dSubSetIndexBuffers[0] = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_ppnSubSetIndices[0], sizeof(UINT) * m_pnSubSetIndices[0], D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER, &m_ppd3dSubSetIndexUploadBuffers[0]);

	m_pd3dSubSetIndexBufferViews[0].BufferLocation = m_ppd3dSubSetIndexBuffers[0]->GetGPUVirtualAddress();
	m_pd3dSubSetIndexBufferViews[0].Format = DXGI_FORMAT_R32_UINT;
	m_pd3dSubSetIndexBufferViews[0].SizeInBytes = sizeof(UINT) * m_pnSubSetIndices[0];
}

CNavMesh::~CNavMesh()
{
}

void CNavMesh::MakeLink(CCell* cells, int n)
{

}
