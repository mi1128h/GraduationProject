#pragma once
#include "Player.h"

enum track_name
{
	idle,
	run,
	slash,
	impact,
	length
};

class CTerrainPlayer : public CPlayer
{
public:
	CTerrainPlayer(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, void* pContext = NULL, int nMeshes = 1);
	virtual ~CTerrainPlayer();

	void SetResource(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	void SetAnimationTracks(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CLoadedModelInfo* pAngrybotModel);
	void InitPlayerMatrics(void* pContext);

	virtual void OnPrepareRender();
	virtual CCamera* ChangeCamera(DWORD nNewCameraMode, float fTimeElapsed);

	virtual void OnPlayerUpdateCallback(float fTimeElapsed);
	virtual void OnCameraUpdateCallback(float fTimeElapsed);

	virtual void Move(ULONG nDirection, float fDistance, bool bVelocity = false);
	virtual void Update(float fTimeElapsed);

private:
	int m_nTracks = track_name::length;
};
