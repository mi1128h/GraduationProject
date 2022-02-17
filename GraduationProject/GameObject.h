#pragma once
#include "Mesh.h"
#include "Camera.h"
#include "Animation.h"

#define RESOURCE_TEXTURE2D			0x01
#define RESOURCE_TEXTURE2D_ARRAY	0x02	//[]
#define RESOURCE_TEXTURE2DARRAY		0x03
#define RESOURCE_TEXTURE_CUBE		0x04
#define RESOURCE_BUFFER				0x05
#define RESOURCE_STRUCTURED_BUFFER	0x06

class CShader;

//게임 객체의 정보를 셰이더에게 넘겨주기 위한 구조체(상수 버퍼)이다.
struct CB_GAMEOBJECT_INFO
{
	XMFLOAT4X4 m_xmf4x4World;
	UINT m_nMaterial;
};

class CTexture
{
public:
	CTexture(int nTextureResources, UINT nResourceType, int nSamplers, int nGraphicsSrvRootParameters, int nComputeUavRootParameters, int nComputeSrvRootParameters);
	virtual ~CTexture();

private:
	int								m_nReferences = 0;

	UINT							m_nTextureType = RESOURCE_TEXTURE2D;

	int								m_nTextures = 0;
	_TCHAR(*m_ppstrTextureNames)[64] = NULL;
	ID3D12Resource** m_ppd3dTextures = NULL;
	ID3D12Resource** m_ppd3dTextureUploadBuffers;

	UINT* m_pnResourceTypes = NULL;

	DXGI_FORMAT* m_pdxgiBufferFormats = NULL;
	int* m_pnBufferElements = NULL;
	int* m_pnBufferStrides = NULL;

	D3D12_GPU_DESCRIPTOR_HANDLE* m_pd3dSrvGpuDescriptorHandles = NULL;
	D3D12_GPU_DESCRIPTOR_HANDLE* m_pd3dUavGpuDescriptorHandles = NULL;

	int								m_nGraphicsSrvRootParameters = 0;
	int* m_pnGraphicsSrvRootParameterIndices = NULL;
	D3D12_GPU_DESCRIPTOR_HANDLE* m_pd3dGraphicsRootParameterSrvGpuDescriptorHandles = NULL;

	int								m_nComputeUavRootParameters = 0;
	int* m_pnComputeUavRootParameterIndices = NULL;
	D3D12_GPU_DESCRIPTOR_HANDLE* m_pd3dComputeRootParameterUavGpuDescriptorHandles = NULL;

	int								m_nComputeSrvRootParameters = 0;
	int* m_pnComputeSrvRootParameterIndices = NULL;
	D3D12_GPU_DESCRIPTOR_HANDLE* m_pd3dComputeRootParameterSrvGpuDescriptorHandles = NULL;

	int								m_nSamplers = 0;
	D3D12_GPU_DESCRIPTOR_HANDLE* m_pd3dSamplerGpuDescriptorHandles = NULL;

public:
	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }

	void SetSampler(int nIndex, D3D12_GPU_DESCRIPTOR_HANDLE d3dSamplerGpuDescriptorHandle);

	void UpdateComputeShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
	void UpdateGraphicsShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
	void UpdateShaderVariable(ID3D12GraphicsCommandList* pd3dCommandList, int nParameterIndex, int nTextureIndex);
	void ReleaseShaderVariables();

	void LoadTextureFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, wchar_t* pszFileName, UINT nResourceType, UINT nIndex);
	void LoadBuffer(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, void* pData, UINT nElements, UINT nStride, DXGI_FORMAT ndxgiFormat, UINT nIndex);
	void CreateBuffer(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, void* pData, UINT nElements, UINT nStride, DXGI_FORMAT dxgiFormat, D3D12_HEAP_TYPE d3dHeapType, D3D12_RESOURCE_STATES d3dResourceStates, UINT nIndex);
	ID3D12Resource* CreateTexture(ID3D12Device* pd3dDevice, UINT nWidth, UINT nHeight, DXGI_FORMAT dxgiFormat, D3D12_RESOURCE_FLAGS d3dResourceFlags, D3D12_RESOURCE_STATES d3dResourceStates, D3D12_CLEAR_VALUE* pd3dClearValue, UINT nResourceType, UINT nIndex);

	int GetGraphicsSrvRootParameters() { return(m_nGraphicsSrvRootParameters); }
	void SetGraphicsSrvRootParameters(UINT nRootParameterStartIndex);
	void SetGraphicsSrvRootParameterIndex(int nIndex, int nRootParameterIndex, int nGpuHandleIndex);
	int GetGraphicsSrvRootParameterIndex(int nIndex) { return(m_pnGraphicsSrvRootParameterIndices[nIndex]); }

	void SetSrvGpuDescriptorHandle(int nIndex, D3D12_GPU_DESCRIPTOR_HANDLE d3dSrvGpuDescriptorHandle);
	D3D12_GPU_DESCRIPTOR_HANDLE GetSrvGpuDescriptorHandle(int nIndex) { return(m_pd3dSrvGpuDescriptorHandles[nIndex]); }

	int GetComputeSrvRootParameters() { return(m_nComputeSrvRootParameters); }
	void SetComputeSrvRootParameters(UINT nRootParameterStartIndex);
	void SetComputeSrvRootParameterIndex(int nIndex, int nRootParameterIndex, int nGpuHandleIndex);
	int GetComputeSrvRootParameterIndex(int nIndex) { return(m_pnComputeSrvRootParameterIndices[nIndex]); }

	int GetComputeUavRootParameters() { return(m_nComputeUavRootParameters); }
	void SetComputeUavRootParameters(UINT nRootParameterStartIndex);
	void SetComputeUavRootParameterIndex(int nIndex, int nRootParameterIndex, int nGpuHandleIndex);
	int GetComputeUavRootParameterIndex(int nIndex) { return(m_pnComputeUavRootParameterIndices[nIndex]); }

	void SetUavGpuDescriptorHandle(int nIndex, D3D12_GPU_DESCRIPTOR_HANDLE d3dUavGpuDescriptorHandle);
	D3D12_GPU_DESCRIPTOR_HANDLE GetUavGpuDescriptorHandle(int nIndex) { return(m_pd3dUavGpuDescriptorHandles[nIndex]); }

	int GetTextures() { return(m_nTextures); }
	ID3D12Resource* GetResource(int nIndex) { return(m_ppd3dTextures[nIndex]); }
	_TCHAR* GetTextureName(int nIndex) { return(m_ppstrTextureNames[nIndex]); }

	UINT GetTextureType() { return(m_nTextureType); }
	UINT GetTextureType(int nIndex) { return(m_pnResourceTypes[nIndex]); }
	DXGI_FORMAT GetBufferFormat(int nIndex) { return(m_pdxgiBufferFormats[nIndex]); }
	int GetBufferElements(int nIndex) { return(m_pnBufferElements[nIndex]); }

	D3D12_SHADER_RESOURCE_VIEW_DESC GetShaderResourceViewDesc(int nIndex);
	D3D12_UNORDERED_ACCESS_VIEW_DESC GetUnorderedAccessViewDesc(int nIndex);

	void ReleaseUploadBuffers();
};

class CMaterial
{
public:
	CMaterial();
	virtual ~CMaterial();

private:
	int								m_nReferences = 0;

public:
	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }

	XMFLOAT4						m_xmf4Albedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	UINT							m_nReflection = 0;
	CTexture* m_pTexture = NULL;
	CShader* m_pShader = NULL;

	void SetAlbedo(XMFLOAT4 xmf4Albedo) { m_xmf4Albedo = xmf4Albedo; }
	void SetReflection(UINT nReflection) { m_nReflection = nReflection; }
	void SetTexture(CTexture* pTexture);
	void SetShader(CShader* pShader);

	void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
	void ReleaseShaderVariables();

	void ReleaseUploadBuffers();
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class CGameObject
{
private:
	int								m_nReferences = 0;

public:
	void AddRef();
	void Release();

public:
	CGameObject();
	CGameObject(int nMaterials);
	virtual ~CGameObject();

public:
	char m_pstrFrameName[64];

	bool m_bActive = true;

	XMFLOAT4X4 m_xmf4x4ToParent;
	XMFLOAT4X4 m_xmf4x4World;

	XMFLOAT3 m_xmf3Scale;
	XMFLOAT3 m_xmf3Rotation;
	XMFLOAT3 m_xmf3Translation;

	CGameObject* m_pParent = NULL;
	CGameObject* m_pChild = NULL;
	CGameObject* m_pSibling = NULL;

	CMesh* m_pMesh = NULL;

	int	m_nMaterials = 0;
	CMaterial** m_ppMaterials = NULL;

	BoundingBox						m_xmBoundingBox;


	D3D12_GPU_DESCRIPTOR_HANDLE		m_d3dCbvGPUDescriptorHandle;
	ID3D12Resource* m_pd3dcbGameObject = NULL;
	CB_GAMEOBJECT_INFO* m_pcbMappedGameObject = NULL;

public:
	void SetMesh(CMesh* pMesh);
	void SetShader(CShader* pShader);
	void SetShader(int nMaterial, CShader* pShader);
	void SetWireFrameShader();
	void SetSkinnedAnimationWireFrameShader();
	void SetMaterial(int nMaterial, CMaterial* pMaterial);

	void SetChild(CGameObject* pChild, bool bReferenceUpdate = false);

	virtual void BuildMaterials(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList) { }

	virtual void OnPrepareAnimate() { }
	virtual void Animate(float fTimeElapsed, CCamera* pCamrea = NULL);

	virtual void OnPrepareRender() {};
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);

	void SetCbvGPUDescriptorHandle(D3D12_GPU_DESCRIPTOR_HANDLE d3dCbvGPUDescriptorHandle) { m_d3dCbvGPUDescriptorHandle = d3dCbvGPUDescriptorHandle; }
	void SetCbvGPUDescriptorHandlePtr(UINT64 nCbvGPUDescriptorHandlePtr) { m_d3dCbvGPUDescriptorHandle.ptr = nCbvGPUDescriptorHandlePtr; }
	D3D12_GPU_DESCRIPTOR_HANDLE GetCbvGPUDescriptorHandle() { return(m_d3dCbvGPUDescriptorHandle); }

	// 상수 버퍼를 생성한다. 
	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void ReleaseShaderVariables();

	virtual void UpdateShaderVariable(ID3D12GraphicsCommandList* pd3dCommandList, XMFLOAT4X4* pxmf4x4World);
	virtual void UpdateShaderVariable(ID3D12GraphicsCommandList* pd3dCommandList, CMaterial* pMaterial);

	void ReleaseUploadBuffers();

	XMFLOAT3 GetPosition();
	XMFLOAT3 GetLook();
	XMFLOAT3 GetUp();
	XMFLOAT3 GetRight();

	void SetPosition(float x, float y, float z);
	void SetPosition(XMFLOAT3 xmf3Position);
	void SetScale(float x, float y, float z);

	void MoveStrafe(float fDistance = 1.0f);
	void MoveUp(float fDistance = 1.0f);
	void MoveForward(float fDistance = 1.0f);

	void Rotate(float fPitch = 10.0f, float fYaw = 10.0f, float fRoll = 10.0f);
	void Rotate(XMFLOAT3* pxmf3Axis, float fAngle);
	void Rotate(XMFLOAT4* pxmf4Quaternion);

	void SetActive(bool bActive) { m_bActive = bActive; }
	void CalculateBoundingBox();

	CGameObject* GetParent() { return(m_pParent); }
	void UpdateTransform(XMFLOAT4X4* pxmf4x4Parent = NULL);

	CGameObject* FindFrame(char* pstrFrameName);
	void SetActive(char* pstrFrameName, bool bActive);
	UINT GetMeshType(int n) { return((m_ppMesh) ? m_pMesh->GetType() : 0x00); }

public:
	CAnimationController* m_pSkinnedAnimationController = NULL;

	CSkinnedMesh* FindSkinnedMesh(char* pstrSkinnedMeshName);
	void FindAndSetSkinnedMesh(CSkinnedMesh** ppSkinnedMeshes, int* pnSkinnedMesh);

	void SetTrackAnimationSet(int nAnimationTrack, int nAnimationSet);
	void SetTrackAnimationPosition(int nAnimationTrack, float fPosition);

	static void LoadAnimationFromFile(FILE* pInFile, CLoadedModelInfo* pLoadedModel);
	static CGameObject* LoadFrameHierarchyFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, CGameObject* pParent, FILE* pInFile, CShader* pShader, int* pnSkinnedMeshes);

	static CLoadedModelInfo* LoadGeometryAndAnimationFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, char* pstrFileName, CShader* pShader);

	static void PrintFrameInfo(CGameObject* pGameObject, CGameObject* pParent);
};

class CRotatingObject : public CGameObject
{
public:
	CRotatingObject(int nMeshes = 1);
	virtual ~CRotatingObject();

private:
	XMFLOAT3 m_xmf3RotationAxis;
	float m_fRotationSpeed;

public:
	void SetRotationSpeed(float fRotationSpeed) { m_fRotationSpeed = fRotationSpeed; }
	void SetRotationAxis(XMFLOAT3 xmf3RotationAxis) {
		m_xmf3RotationAxis = xmf3RotationAxis;
	}
	virtual void Animate(float fTimeElapsed);
};
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// 높이 맵을 사용한 실외 지형을 표현하는 게임 객체
class CHeightMapTerrain : public CGameObject
{
public:
	CHeightMapTerrain(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, LPCTSTR pFileName, int nWidth, int nLength, int nBlockWidth, int nBlockLength, XMFLOAT3 xmf3Scale, XMFLOAT4 xmf4Color);
	virtual ~CHeightMapTerrain();

private:
	CHeightMapImage* m_pHeightMapImage;

	int							m_nWidth; //Width of HeightMapImage
	int							m_nLength; //Height of HeightMapImage

	XMFLOAT3					m_xmf3Scale; //Scaler for World Coordinates

public:
	float GetHeight(float x, float z);
	XMFLOAT3 GetNormal(float x, float z) { return(m_pHeightMapImage->GetHeightMapNormal(int(x / m_xmf3Scale.x), int(z / m_xmf3Scale.z), m_xmf3Scale)); }

	int GetRawImageWidth() { return(m_pHeightMapImage->GetRawImageWidth()); }
	int GetRawImageLength() { return(m_pHeightMapImage->GetRawImageLength()); }

	XMFLOAT3 GetScale() { return(m_xmf3Scale); }
#ifdef _WITH_VERTICES_AS_SCALING
	float GetWidth() { return(float(m_nWidth)); }
	float GetLength() { return(float(m_nLength)); }
#else
	int GetWidth() { return(m_nWidth * m_xmf3Scale.x); }
	int GetLength() { return(m_nLength * m_xmf3Scale.z); }
#endif
};


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
class CSkyBox : public CGameObject
{
public:
	CSkyBox(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual ~CSkyBox();

	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera = NULL);
};