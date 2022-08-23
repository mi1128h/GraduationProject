#pragma once
#include "stdafx.h"
#include "Camera.h"
#include "GameObject.h"
#include "ParticleSystem.h"
#include "ParticleShader.h"

class CFactory
{
public:
	CFactory() {};
	~CFactory() {};

	virtual void BuildObjects(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature, 
		ID3D12GraphicsCommandList* pd3dCommandList, void* pContext) = 0;
	virtual void AnimateObjects(float fTimeElapsed, CCamera* pCamrea = NULL);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);
	virtual void SetObjectCollision(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	vector<CGameObject*> GetGameObjects() { return _gameObjects; }

protected:
	vector<CGameObject*> _gameObjects;
};

/////////////////////

class CObjectFactory : public CFactory
{
public:
	CObjectFactory() {};
	~CObjectFactory() {};

	virtual void BuildObjects(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature, ID3D12GraphicsCommandList* pd3dCommandList, void* pContext);
};

class CCannonFactory : public CFactory
{
public:
	CCannonFactory() {};
	~CCannonFactory() {};

	CCannonObject* m_pInteractedCannon = NULL;

	virtual void BuildObjects(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature, ID3D12GraphicsCommandList* pd3dCommandList, void* pContext);
	void ActiveCannon();
	void RotateCannon(WPARAM wParam);
	XMFLOAT3& GetCannonPosition();
	CGameObject* GetCannonBall();
};

class CMonsterFactory : public CFactory
{
public:
	CMonsterFactory() {};
	~CMonsterFactory() {};

	virtual void BuildObjects(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature, ID3D12GraphicsCommandList* pd3dCommandList, void* pContext);
	virtual void SetObjectCollision(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	void FindTarget(CGameObject* pObject);
	void SetNavMesh(CNavMesh* pNavMesh);
	void InitMonsters();
};

class CUIFactory : public CFactory
{
public:
	CUIFactory() {};
	~CUIFactory() {};

	virtual void BuildObjects(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature, ID3D12GraphicsCommandList* pd3dCommandList, void* pContext) {};
	virtual void BuildObjects(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature, ID3D12GraphicsCommandList* pd3dCommandList, void* pContext, CPlayer* pPlayer);
	virtual void SetObjectCollision(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature) {};
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);
	virtual void AnimateObjects(float fTimeElapsed, CCamera* pCamrea = NULL);
	void SetTargetMonster(CGameObject* pObject);
	void SetGameState(int n) { m_gameState = n; }
	void SetStartSelected(bool b) { m_StartSelected = b; }
	int GetGameState() { return m_gameState; }
	bool GetSelectedMenu() { return m_StartSelected; }
	void MonsterDeadCount();
	void BossDeadCount();
	bool IsClear();
	void initQuest();

private:
	CCamera* m_pCamera = NULL;
	CGameObject* m_pTitleUi = NULL;
	CGameObject* m_pOverUi = NULL;
	CGameObject* m_pClearUi = NULL;
	CGameObject* m_pMenuPointerUi = NULL;
	CMonsterQusetUIObject* m_pMonsterQuestUi = NULL;
	CMonsterQusetUIObject* m_pBossQuestUi = NULL;
	int m_gameState = 0;	// 0: title, 1: play, 2: over
	bool m_StartSelected = true;
};

class CParticleFactory : public CFactory
{
public:
	CParticleFactory() {};
	~CParticleFactory() {};

	virtual void BuildObjects(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature, ID3D12GraphicsCommandList* pd3dCommandList, void* pContext);
	virtual void SetObjectCollision(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature) {};
	virtual void AnimateObjects(float fTimeElapsed, CCamera* pCamrea = NULL);
	
	void SetBreathPosition(XMFLOAT4X4* pos);
	void SetBombParticlePosition(XMFLOAT3& pos);
	void BombParticleController(int index = 0);

protected:
	XMFLOAT4X4* m_xmf4x4BreathWorld;

};

class CEffectFactory : public CFactory
{
public:
	CEffectFactory() {};
	~CEffectFactory() {};

	virtual void BuildObjects(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature, ID3D12GraphicsCommandList* pd3dCommandList, void* pContext);

	void printEffect(XMFLOAT3 pos);
};