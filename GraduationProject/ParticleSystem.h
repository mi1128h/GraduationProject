#pragma once
#include "GameObject.h"

struct VS_VB_PARTICLE_INFO
{
	XMFLOAT3 m_xmf3Position;
	XMFLOAT2 m_xmf2Age;
};

struct particle_info
{
	bool m_bActive = true;
	XMFLOAT3 m_xmf3Position;
	float m_fVelocity;
	XMFLOAT3 m_xmf3Accel;
	XMFLOAT3 m_xmf3Vectors;
	float m_fParticleAge = 0.0f;
};

#define EMIT_MODE 0xff
class CParticleSystem : public CGameObject
{
public:
	CParticleSystem() :CGameObject(1) {}
	CParticleSystem(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
		XMFLOAT3 xmf3Position, XMFLOAT3 xmf3Range, int nParticles, float fEmitTime = 100000.0f);
	virtual ~CParticleSystem();

	virtual void Animate(float fElapsedTime, CCamera* pCamrea = NULL);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);

	virtual void KillParticles();
	virtual void EmitParticles(float fElapsedTime);
	virtual void UpdateParticles(float fElapsedTime);
	virtual void InitParticleSystem(XMFLOAT3 xmf3Position, XMFLOAT3 xmf3Range, float fEmitTime);

	virtual void CreateParticles();
	virtual void InitParticles();
public:
	vector<particle_info> m_pParticles;
	int m_nParticles;
	int m_nDeadParticles;
	float m_fParticleVelocity = 15.0f;
	float m_fParticleVelocityVariation = 100.0f;
	XMFLOAT3 m_xmf3ParticleRange;
	XMFLOAT3 m_xmf3MaxRange;

protected:
	float m_fMaxLife = 5.0f;

	ID3D12Resource* m_pd3dInstancesBuffer = NULL;
	D3D12_VERTEX_BUFFER_VIEW		m_d3dInstancingBufferView;

	ID3D12Resource* m_pd3dcbParticles = NULL;
	VS_VB_PARTICLE_INFO* m_pcbMappedParticles = NULL;
};

bool compare(particle_info& p1, particle_info& p2);

///

class CExplosiveParticle : public CParticleSystem
{
public:
	CExplosiveParticle(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
		XMFLOAT3 xmf3Position, XMFLOAT3 xmf3Range, int nParticles, float fEmitTime = 200.0f);
	virtual ~CExplosiveParticle();

	virtual void CreateParticles();
	virtual void KillParticles();
	virtual void EmitParticles(float fElapsedTime);
	virtual void InitParticles();
};


class CBreathParticle : public CParticleSystem
{
public:
	CBreathParticle(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
		XMFLOAT3 xmf3Position, XMFLOAT3 xmf3Range, int nParticles, float fEmitTime = 2000.0f);
	virtual ~CBreathParticle();

	virtual void CreateParticles();
	virtual void EmitParticles(float fElapsedTime);
	virtual void UpdateParticles(float fElapsedTime);
	virtual void KillParticles();

protected:
	float _sinValue = 0.0f;
	CBossMonster* m_pBoss = NULL;

public:
	void SetBoss(CBossMonster* pBoss) { m_pBoss = pBoss; }
};