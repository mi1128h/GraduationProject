cbuffer cbPlayerInfo : register(b0)
{
	matrix		gmtxPlayerWorld : packoffset(c0);
	float3		f3PlayerVelocity : packoffset(c4);
};

cbuffer cbCameraInfo : register(b1)
{
	matrix		gmtxView : packoffset(c0);
	matrix		gmtxProjection : packoffset(c4);
	matrix		gmtxViewProjection : packoffset(c8);
	float3		gvCameraPosition : packoffset(c12);
};

cbuffer cbGameObjectInfo : register(b2)
{
	matrix		gmtxGameObject : packoffset(c0);
};

cbuffer cbBulletObjectsInfo : register(b3)
{
	int		TextureIndex;
};

cbuffer cbFrameworkInfo : register(b4)
{
	float		gfCurrentTime : packoffset(c0.x);
	float		gfElapsedTime : packoffset(c0.y);
	uint		gnRenderMode : packoffset(c0.z);
	uint		gnBlurMode : packoffset(c0.w);
};

#define DEBUG_TESSELLATION			0x20
#define DEBUG_BLURRING				0x21

struct PS_MULTIPLE_RENDER_TARGETS_OUTPUT
{
	float4 f4Scene : SV_TARGET0; //Swap Chain Back Buffer
	float4 f4Color : SV_TARGET1;
	float4 fDepth : SV_TARGET2;
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
struct VS_DIFFUSED_INPUT
{
	float3 position : POSITION;
	float4 color : COLOR;
};

struct VS_DIFFUSED_OUTPUT
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
};

VS_DIFFUSED_OUTPUT VSPlayer(VS_DIFFUSED_INPUT input)
{
	VS_DIFFUSED_OUTPUT output;

	output.position = mul(mul(mul(float4(input.position, 1.0f), gmtxPlayerWorld), gmtxView), gmtxProjection);
	output.color = input.color;

	return(output);
}

PS_MULTIPLE_RENDER_TARGETS_OUTPUT PSPlayer(VS_DIFFUSED_OUTPUT input) : SV_TARGET
{
	PS_MULTIPLE_RENDER_TARGETS_OUTPUT output;
	output.f4Scene = output.f4Color = input.color;
	output.fDepth = 1.0f - input.position.z;
	return(output);
}


VS_DIFFUSED_OUTPUT VSMirror(VS_DIFFUSED_INPUT input)
{
	VS_DIFFUSED_OUTPUT output;

	output.position = mul(mul(mul(float4(input.position, 1.0f), gmtxGameObject), gmtxView), gmtxProjection);
	output.color = input.color;

	return(output);
}

PS_MULTIPLE_RENDER_TARGETS_OUTPUT PSMirror(VS_DIFFUSED_OUTPUT input) : SV_TARGET
{
	input.color.a = 0.5f;

	float4 cColor = input.color;

	PS_MULTIPLE_RENDER_TARGETS_OUTPUT output;
	output.f4Scene = output.f4Color = cColor;
	output.fDepth = 1.0f - input.position.z;
	return(output);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
Texture2D gtxtTexture : register(t0);
SamplerState gSamplerState : register(s0);
SamplerState gClampSamplerState : register(s1);
SamplerState gMirrorSamplerState : register(s2);

struct VS_TEXTURED_INPUT
{
	float3 position : POSITION;
	float2 uv : TEXCOORD;
};

struct VS_TEXTURED_OUTPUT
{
	float4 position : SV_POSITION;
	float2 uv : TEXCOORD;
};

VS_TEXTURED_OUTPUT VSTextured(VS_TEXTURED_INPUT input)
{
	VS_TEXTURED_OUTPUT output;

	output.position = mul(mul(mul(float4(input.position, 1.0f), gmtxGameObject), gmtxView), gmtxProjection);
	output.uv = input.uv;

	return(output);
}

PS_MULTIPLE_RENDER_TARGETS_OUTPUT PSTextured(VS_TEXTURED_OUTPUT input) : SV_TARGET
{
	float4 cColor = gtxtTexture.Sample(gSamplerState, input.uv);

	PS_MULTIPLE_RENDER_TARGETS_OUTPUT output;
	output.f4Scene = output.f4Color = cColor;
	output.fDepth = 1.0f - input.position.z;

	return(output);
}

/////////////////////////////////////////////////////////////////////////////////////////////////
Texture2D<float4> gtxtTerrainBaseTexture : register(t1);
Texture2D<float4> gtxtTerrainDetailTextures[3] : register(t2); //t2, t3, t4
Texture2D<float4> gtxtTerrainAlphaTexture[2] : register(t5); // t5, t6

struct VS_TERRAIN_INPUT
{
	float3 position : POSITION;
	float4 color : COLOR;
	float2 uv0 : TEXCOORD0;
	float2 uv1 : TEXCOORD1;
};

struct VS_TERRAIN_OUTPUT
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
	float2 uv0 : TEXCOORD0;
	float2 uv1 : TEXCOORD1;
};

VS_TERRAIN_OUTPUT VSTerrain(VS_TERRAIN_INPUT input)
{
	VS_TERRAIN_OUTPUT output;

	output.position = mul(mul(mul(float4(input.position, 1.0f), gmtxGameObject), gmtxView), gmtxProjection);
	output.color = input.color;
	output.uv0 = input.uv0;
	output.uv1 = input.uv1;

	return(output);
}

float4 PSTerrain(VS_TERRAIN_OUTPUT input) : SV_TARGET
{
	float4 cBaseTexColor = gtxtTerrainBaseTexture.Sample(gSamplerState, input.uv0);
	float fAlphas[2];
	fAlphas[0] = gtxtTerrainAlphaTexture[0].Sample(gSamplerState, input.uv0).w;
	fAlphas[1] = gtxtTerrainAlphaTexture[1].Sample(gSamplerState, input.uv0).w;

	float4 cDetailTexColors[3];
	cDetailTexColors[0] = gtxtTerrainDetailTextures[0].Sample(gSamplerState, input.uv1 * 2.0f);
	cDetailTexColors[1] = gtxtTerrainDetailTextures[1].Sample(gSamplerState, input.uv1 * 2.0f);
	cDetailTexColors[2] = gtxtTerrainDetailTextures[2].Sample(gSamplerState, input.uv1 * 2.0f);

	float4 cColor = input.color * saturate((cBaseTexColor * 0.6f) + (cDetailTexColors[1]*0.4f));
	cColor += lerp(cBaseTexColor * 0.25f, cDetailTexColors[2], 1.0f - fAlphas[0]);

	if (fAlphas[1] < 0.35f) cColor = saturate((cBaseTexColor * 0.3f) + (cDetailTexColors[0] * 0.7f));

	return(cColor);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
Texture2D gtxtSkyBox : register(t7);

PS_MULTIPLE_RENDER_TARGETS_OUTPUT PSSkyBox(VS_TEXTURED_OUTPUT input) : SV_TARGET
{
	float4 cColor = gtxtSkyBox.Sample(gClampSamplerState, input.uv);


	PS_MULTIPLE_RENDER_TARGETS_OUTPUT output;
	output.f4Scene = output.f4Color = cColor;
	
	output.fDepth = 1.0f - input.position.z;

	return(output);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//#define _WITH_BILLBOARD_ANIMATION

struct VS_BILLBOARD_INSTANCING_INPUT
{
	float3 position : POSITION;
	float2 uv : TEXCOORD;
	float3 instancePosition : INSTANCEPOSITION;
	float4 billboardInfo : BILLBOARDINFO; //(cx, cy, type, texture)
};

struct VS_BILLBOARD_INSTANCING_OUTPUT
{
	float4 position : SV_POSITION;
	float2 uv : TEXCOORD;
	int textureID : TEXTUREID;
};

VS_BILLBOARD_INSTANCING_OUTPUT VSBillboardInstancing(VS_BILLBOARD_INSTANCING_INPUT input)
{
	VS_BILLBOARD_INSTANCING_OUTPUT output;

	input.position.x *= (input.billboardInfo.x * 0.5f);
	input.position.y *= (input.billboardInfo.y * 0.5f);

	float3 f3Look = normalize(gvCameraPosition - input.instancePosition);
	float3 f3Up = float3(0.0f, 1.0f, 0.0f);
	float3 f3Right = normalize(cross(f3Up, f3Look));

	matrix mtxWorld;
	mtxWorld[0] = float4(f3Right, 0.0f);
	mtxWorld[1] = float4(f3Up, 0.0f);
	mtxWorld[2] = float4(f3Look, 0.0f);
	mtxWorld[3] = float4(input.instancePosition, 1.0f);

	output.position = mul(mul(mul(float4(input.position, 1.0f), mtxWorld), gmtxView), gmtxProjection);

	output.uv = input.uv;

	output.textureID = (int)input.billboardInfo.w - 1;

	return(output);
}

Texture2D<float4> gtxtBillInstTextures[7] : register(t8); //  t8, t9, t10, t11, t12, t13, t14

float4 PSBillboardInstancing(VS_BILLBOARD_INSTANCING_OUTPUT input) : SV_TARGET
{
	float4 cColor = gtxtBillInstTextures[NonUniformResourceIndex(input.textureID)].Sample(gSamplerState, input.uv);

	return(cColor);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

VS_TEXTURED_OUTPUT VSBulletBillboard(VS_TEXTURED_INPUT input)
{
	VS_TEXTURED_OUTPUT output;
	output.position = mul(mul(mul(float4(input.position, 1.0f), gmtxGameObject), gmtxView), gmtxProjection);
	output.uv = input.uv;

	return(output);
}

Texture2D<float4> gtxtBulletTextures[7] : register(t15); // t15, 16, 17, 18, 19, 20, 21

float4 PSBulletBillboard(VS_TEXTURED_OUTPUT input) :SV_TARGET
{
	float4 cColor = gtxtBulletTextures[TextureIndex].SampleLevel(gSamplerState, input.uv,0);
	if (cColor.a <= 0.3f) discard;

	return(cColor);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct VS_BILLBOARD_OUT{
	float3 centerW:POSITION;
	float2 sizeW:SIZE;
};

struct VS_BILLBOARD_IN{
	float3 posW:POSITION;
	float2 sizeW:SIZE;
	float3 instancePosition : INSTANCEPOSITION;
};

struct GS_BILLBOARD_OUT {
	float4 posH:SV_POSITION;
	float3 posW:POSITION;
	float3 normalW : NORMAL;
	float2 uv: TEXCOORD;
	uint primID:SV_PrimitiveID;
};

VS_BILLBOARD_OUT VSBillboard(VS_BILLBOARD_IN input)
{
	VS_BILLBOARD_OUT output;
	output.centerW = input.instancePosition;
	output.sizeW = input.sizeW;
	return(output);
}

[maxvertexcount(4)]
void GSBillboard(point VS_BILLBOARD_OUT input[1],uint primID:SV_PrimitiveID, inout TriangleStream<GS_BILLBOARD_OUT> outStream)
{
	float3 vUp = float3(0.0f, 1.0f, 0.0f);
	float3 vLook = gvCameraPosition.xyz - input[0].centerW;
	vLook = normalize(vLook);
	float3 vRight = cross(vUp, vLook);
	float fHalfW = input[0].sizeW.x * 0.5f;
	float fHalfH = input[0].sizeW.y * 0.5f;

	float4 pVertices[4];
	pVertices[0] = float4(input[0].centerW + fHalfW * vRight - fHalfH * vUp, 1.0f);
	pVertices[1] = float4(input[0].centerW + fHalfW * vRight + fHalfH * vUp, 1.0f);
	pVertices[2] = float4(input[0].centerW - fHalfW * vRight - fHalfH * vUp, 1.0f);
	pVertices[3] = float4(input[0].centerW - fHalfW * vRight + fHalfH * vUp, 1.0f);

	float2 pUVs[4] = { float2(0.0f,1.0f),float2(0.0f,0.0f),float2(1.0f,1.0f),float2(1.0f,0.0f) };
	GS_BILLBOARD_OUT output;
	for (int i = 0; i < 4; ++i) {
		output.posW = pVertices[i].xyz;
		output.posH =mul(mul(pVertices[i], gmtxView), gmtxProjection);
		output.normalW = vLook;
		output.uv = pUVs[i];
		output.primID = primID;
		outStream.Append(output);
	}
}

Texture2D<float4> gtxtBillboardTexture[3] : register(t22); // 22,23, 24

PS_MULTIPLE_RENDER_TARGETS_OUTPUT PSBillboard(GS_BILLBOARD_OUT input): SV_TARGET
{
	//float4 cColor0 = gtxtBillboardTexture[0].SampleLevel(gSamplerState, input.uv, 0);
	//float4 cColor1 = gtxtBillboardTexture[1].SampleLevel(gMirrorSamplerState, float2(input.uv.x, frac(input.uv.y + gfCurrentTime)), 0);
	//float4 cColor2 = gtxtBillboardTexture[2].SampleLevel(gMirrorSamplerState, float2(input.uv.x, frac(input.uv.y + gfCurrentTime)), 0);
	//float4 cColor3 = gtxtBillboardTexture[0].SampleLevel(gSamplerState, float2(input.uv.x, saturate(input.uv.y + cColor1.r)), 0);
	//float4 cColor4 = gtxtBillboardTexture[0].SampleLevel(gSamplerState, float2(input.uv.x, saturate(input.uv.y + cColor2.r)), 0);

	////float4 cOutput = float4(lerp(cColor3.rgb, cColor4.rgb, 0.3f), lerp(cColor3.a, cColor4.a, 0.7f));

	//float alpha0 = gtxtBillboardTexture[1].Sample(gSamplerState, input.uv).w;
	//float alpha1 = gtxtBillboardTexture[2].Sample(gSamplerState, input.uv).w;

	//float4 cOutput = cColor0 * alpha0;

	float4 cColor0 = gtxtBillboardTexture[0].SampleLevel(gSamplerState, input.uv, 0);
	float4 cColor1 = gtxtBillboardTexture[1].SampleLevel(gMirrorSamplerState, float2(input.uv.x, input.uv.y + gfCurrentTime), 0);
	//float4 cColor1 = gtxtBillboardTexture[1].SampleLevel(gMirrorSamplerState, float2(input.uv.x, input.uv.y), 0);

	if (cColor0.a > 0.15) cColor0.a = 0.6;
	float4 cOutput = float4(cColor0.rgb + (cColor0.rgb * cColor1.rgb), cColor0.a);

//	float4 cColor = gtxtBillboardTexture.Sample(gSamplerState, input.uv);

	PS_MULTIPLE_RENDER_TARGETS_OUTPUT output;
	output.f4Scene = output.f4Color = cOutput;
	output.fDepth = 1.0f - input.posH.z;
	return(output);
}


//--------------------------------------------------------------------------------------
//
struct VS_TERRAIN_TESSELLATION_OUTPUT
{
	float3 position : POSITION;
	float3 positionW : POSITION1;
	float4 color : COLOR;
	float2 uv0 : TEXCOORD0;
	float2 uv1 : TEXCOORD1;
};

VS_TERRAIN_TESSELLATION_OUTPUT VSTerrainTessellation(VS_TERRAIN_INPUT input)
{
	VS_TERRAIN_TESSELLATION_OUTPUT output;

	output.position = input.position;
	output.positionW = mul(float4(input.position, 1.0f), gmtxGameObject).xyz;
	output.color = input.color;
	output.uv0 = input.uv0;
	output.uv1 = input.uv1;

	return(output);
}

struct HS_TERRAIN_TESSELLATION_CONSTANT
{
	float fTessEdges[4] : SV_TessFactor;
	float fTessInsides[2] : SV_InsideTessFactor;
};

struct HS_TERRAIN_TESSELLATION_OUTPUT
{
	float3 position : POSITION;
	float4 color : COLOR;
	float2 uv0 : TEXCOORD0;
	float2 uv1 : TEXCOORD1;
};

struct DS_TERRAIN_TESSELLATION_OUTPUT
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
	float2 uv0 : TEXCOORD0;
	float2 uv1 : TEXCOORD1;
	float4 tessellation : TEXCOORD2;
};

[domain("quad")]
//[partitioning("fractional_even")]
[partitioning("integer")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(25)]
[patchconstantfunc("HSTerrainTessellationConstant")]
[maxtessfactor(64.0f)]
HS_TERRAIN_TESSELLATION_OUTPUT HSTerrainTessellation(InputPatch<VS_TERRAIN_TESSELLATION_OUTPUT, 25> input, uint i : SV_OutputControlPointID)
{
	HS_TERRAIN_TESSELLATION_OUTPUT output;

	output.position = input[i].position;
	output.color = input[i].color;
	output.uv0 = input[i].uv0;
	output.uv1 = input[i].uv1;

	return(output);
}

float CalculateTessFactor(float3 f3Position)
{
	float fDistToCamera = distance(f3Position, gvCameraPosition);
	float s = saturate((fDistToCamera - 10.0f) / (500.0f - 10.0f));
	return(lerp(64.0f, 1.0f, s));
	//	return(pow(2, lerp(20.0f, 4.0f, s)));
}

HS_TERRAIN_TESSELLATION_CONSTANT HSTerrainTessellationConstant(InputPatch<VS_TERRAIN_TESSELLATION_OUTPUT, 25> input)
{
	HS_TERRAIN_TESSELLATION_CONSTANT output;

	{
		float3 e0 = 0.5f * (input[0].positionW + input[4].positionW);
		float3 e1 = 0.5f * (input[0].positionW + input[20].positionW);
		float3 e2 = 0.5f * (input[4].positionW + input[24].positionW);
		float3 e3 = 0.5f * (input[20].positionW + input[24].positionW);

		output.fTessEdges[0] = CalculateTessFactor(e0);
		output.fTessEdges[1] = CalculateTessFactor(e1);
		output.fTessEdges[2] = CalculateTessFactor(e2);
		output.fTessEdges[3] = CalculateTessFactor(e3);

		float3 f3Sum = float3(0.0f, 0.0f, 0.0f);
		for (int i = 0; i < 25; i++) f3Sum += input[i].positionW;
		float3 f3Center = f3Sum / 25.0f;
		output.fTessInsides[0] = output.fTessInsides[1] = CalculateTessFactor(f3Center);
	}

	return(output);
}

void BernsteinCoeffcient5x5(float t, out float fBernstein[5])
{
	float tInv = 1.0f - t;
	fBernstein[0] = tInv * tInv * tInv * tInv;
	fBernstein[1] = 4.0f * t * tInv * tInv * tInv;
	fBernstein[2] = 6.0f * t * t * tInv * tInv;
	fBernstein[3] = 4.0f * t * t * t * tInv;
	fBernstein[4] = t * t * t * t;
}

float3 CubicBezierSum5x5(OutputPatch<HS_TERRAIN_TESSELLATION_OUTPUT, 25> patch, float uB[5], float vB[5])
{
	float3 f3Sum = float3(0.0f, 0.0f, 0.0f);
	f3Sum = vB[0] * (uB[0] * patch[0].position + uB[1] * patch[1].position + uB[2] * patch[2].position + uB[3] * patch[3].position + uB[4] * patch[4].position);
	f3Sum += vB[1] * (uB[0] * patch[5].position + uB[1] * patch[6].position + uB[2] * patch[7].position + uB[3] * patch[8].position + uB[4] * patch[9].position);
	f3Sum += vB[2] * (uB[0] * patch[10].position + uB[1] * patch[11].position + uB[2] * patch[12].position + uB[3] * patch[13].position + uB[4] * patch[14].position);
	f3Sum += vB[3] * (uB[0] * patch[15].position + uB[1] * patch[16].position + uB[2] * patch[17].position + uB[3] * patch[18].position + uB[4] * patch[19].position);
	f3Sum += vB[4] * (uB[0] * patch[20].position + uB[1] * patch[21].position + uB[2] * patch[22].position + uB[3] * patch[23].position + uB[4] * patch[24].position);

	return(f3Sum);
}

[domain("quad")]
DS_TERRAIN_TESSELLATION_OUTPUT DSTerrainTessellation(HS_TERRAIN_TESSELLATION_CONSTANT patchConstant, float2 uv : SV_DomainLocation, OutputPatch<HS_TERRAIN_TESSELLATION_OUTPUT, 25> patch)
{
	DS_TERRAIN_TESSELLATION_OUTPUT output = (DS_TERRAIN_TESSELLATION_OUTPUT)0;

	float uB[5], vB[5];
	BernsteinCoeffcient5x5(uv.x, uB);
	BernsteinCoeffcient5x5(uv.y, vB);

	output.color = lerp(lerp(patch[0].color, patch[4].color, uv.x), lerp(patch[20].color, patch[24].color, uv.x), uv.y);
	output.uv0 = lerp(lerp(patch[0].uv0, patch[4].uv0, uv.x), lerp(patch[20].uv0, patch[24].uv0, uv.x), uv.y);
	output.uv1 = lerp(lerp(patch[0].uv1, patch[4].uv1, uv.x), lerp(patch[20].uv1, patch[24].uv1, uv.x), uv.y);

	float3 position = CubicBezierSum5x5(patch, uB, vB);
	matrix mtxWorldViewProjection = mul(mul(gmtxGameObject, gmtxView), gmtxProjection);
	output.position = mul(float4(position, 1.0f), mtxWorldViewProjection);

	output.tessellation = float4(patchConstant.fTessEdges[0], patchConstant.fTessEdges[1], patchConstant.fTessEdges[2], patchConstant.fTessEdges[3]);

	return(output);
}

PS_MULTIPLE_RENDER_TARGETS_OUTPUT PSTerrainTessellation(DS_TERRAIN_TESSELLATION_OUTPUT input) : SV_TARGET
{
	float4 cColor = float4(0.0f, 1.0f, 0.0f, 1.0f);

	if (gnRenderMode & (DEBUG_TESSELLATION))
	{
		if (input.tessellation.w <= 5.0f) cColor = float4(1.0f, 0.0f, 0.0f, 1.0f);
		else if (input.tessellation.w <= 10.0f) cColor = float4(0.0f, 1.0f, 0.0f, 1.0f);
		else if (input.tessellation.w <= 20.0f) cColor = float4(0.0f, 0.0f, 1.0f, 1.0f);
		else if (input.tessellation.w <= 30.0f) cColor = float4(1.0f, 0.0f, 1.0f, 1.0f);
		else if (input.tessellation.w <= 40.0f) cColor = float4(1.0f, 1.0f, 0.0f, 1.0f);
		else if (input.tessellation.w <= 50.0f) cColor = float4(1.0f, 1.0f, 1.0f, 1.0f);
		else if (input.tessellation.w <= 55.0f) cColor = float4(0.2f, 0.2f, 0.72f, 1.0f);
		else if (input.tessellation.w <= 60.0f) cColor = float4(0.5f, 0.75f, 0.75f, 1.0f);
		else cColor = float4(0.87f, 0.17f, 1.0f, 1.0f);
	}
	else
	{
		float4 cBaseTexColor = gtxtTerrainBaseTexture.Sample(gSamplerState, input.uv0);
		float fAlphas[2];
		fAlphas[0] = gtxtTerrainAlphaTexture[0].Sample(gSamplerState, input.uv0).w;
		fAlphas[1] = gtxtTerrainAlphaTexture[1].Sample(gSamplerState, input.uv0).w;

		float4 cDetailTexColors[3];
		cDetailTexColors[0] = gtxtTerrainDetailTextures[0].Sample(gSamplerState, input.uv1 * 2.0f);
		cDetailTexColors[1] = gtxtTerrainDetailTextures[1].Sample(gSamplerState, input.uv1 * 2.0f);
		cDetailTexColors[2] = gtxtTerrainDetailTextures[2].Sample(gSamplerState, input.uv1 * 2.0f);

		cColor = input.color * saturate((cBaseTexColor * 0.6f) + (cDetailTexColors[1] * 0.4f));
		cColor += lerp(cBaseTexColor * 0.25f, cDetailTexColors[2], 1.0f - fAlphas[0]);

		if (fAlphas[1] < 0.35f) cColor = saturate((cBaseTexColor * 0.3f) + (cDetailTexColors[0] * 0.7f));
	}

	PS_MULTIPLE_RENDER_TARGETS_OUTPUT output;
	//float grayscale = (cColor.r + cColor.g + cColor.b) / 3.0f;
	//output.f4Scene = output.f4Color = float4(grayscale,grayscale,grayscale,1.0f);

	output.f4Scene = output.f4Color = cColor;
	output.fDepth = 1.0f - input.position.z;

	return(output);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////


RWTexture2D<float4> gtxtRWOutput : register(u0);
Texture2D<float4> gtxtInputTextures[3] : register(t25);
Texture2D gtxtOutput : register(t28);
//Texture2D<float4> gtxtInputTextures[3] : register(t0);

groupshared float4 gTextureCache[(480 + 2 * 5)];
groupshared float4 gHorzCache[(640 + 2 * 5)];
static float gfWeights[11] = { 0.05f,0.05f,0.1f,0.1f,0.1f,0.2f,0.1f,0.1f,0.1f,0.05f,0.05f };

[numthreads(1, 480, 1)]
void VertBlurCS(int3 vGroupThreadID: SV_GroupThreadID, int3 vDispatchThreadID : SV_DispatchThreadID)
{
	if (vGroupThreadID.y < 5)
	{
		int y = max(vDispatchThreadID.y - 5, 0);
		gTextureCache[vGroupThreadID.y] = gtxtInputTextures[0][int2(vDispatchThreadID.x, y)];
	}
	else if (vGroupThreadID.y >= 480 - 5)
	{
		int y = min(vDispatchThreadID.y + 5, gtxtInputTextures[0].Length.y - 1);
		gTextureCache[vGroupThreadID.y + 2 * 5] = gtxtInputTextures[0][int2(vDispatchThreadID.x, y)];
	}
	gTextureCache[vGroupThreadID.y + 5] = gtxtInputTextures[0][min(vDispatchThreadID.xy, gtxtInputTextures[0].Length.xy - 1)];

	GroupMemoryBarrierWithGroupSync();

	float4 cBlurredColor = float4(0, 0, 0, 0);
	for (int i = -5; i <= 5; ++i)
	{
		int k = vGroupThreadID.y + 5 + i;
		cBlurredColor += gfWeights[i + 5] * gTextureCache[k];
	}
	gtxtRWOutput[vDispatchThreadID.xy] = cBlurredColor;
}

[numthreads(640, 1, 1)]
void HorzBlurCS(int3 vGroupThreadID: SV_GroupThreadID, int3 vDispatchThreadID : SV_DispatchThreadID)
{
	if (vGroupThreadID.x < 5)
	{
		int x = max(vDispatchThreadID.x - 5, 0);
		gHorzCache[vGroupThreadID.x] = gtxtInputTextures[0][int2(x, vDispatchThreadID.y)];
	}
	else if (vGroupThreadID.x >= 640 - 5)
	{
		int x = min(vDispatchThreadID.x + 5, gtxtInputTextures[0].Length.x - 1);
		gHorzCache[vGroupThreadID.x + 2 * 5] = gtxtInputTextures[0][int2(x, vDispatchThreadID.y)];
	}
	gHorzCache[vGroupThreadID.x + 5] = gtxtInputTextures[0][min(vDispatchThreadID.xy, gtxtInputTextures[0].Length.xy - 1)];

	GroupMemoryBarrierWithGroupSync();

	float4 cBlurredColor = float4(0, 0, 0, 0);
	for (int i = -5; i <= 5; ++i)
	{
		int k = vGroupThreadID.x + 5 + i;
		cBlurredColor += gfWeights[i + 5] * gHorzCache[k];
	}
	gtxtRWOutput[vDispatchThreadID.xy] = cBlurredColor;
}

float4 GetColorFromDepth(float fDepth)
{
	float4 cColor = float4(0.0f, 0.0f, 0.0f, 1.0f);

	if (fDepth > 1.0f) cColor = float4(1.0f, 1.0f, 1.0f, 1.0f);
	else if (fDepth < 0.00625f) cColor = float4(1.0f, 0.0f, 0.0f, 1.0f);
	else if (fDepth < 0.0125f) cColor = float4(0.0f, 1.0f, 0.0f, 1.0f);
	else if (fDepth < 0.025f) cColor = float4(0.0f, 0.0f, 1.0f, 1.0f);
	else if (fDepth < 0.05f) cColor = float4(1.0f, 1.0f, 0.0f, 1.0f);
	else if (fDepth < 0.075f) cColor = float4(0.0f, 1.0f, 1.0f, 1.0f);
	else if (fDepth < 0.1f) cColor = float4(1.0f, 0.5f, 0.5f, 1.0f);
	else if (fDepth < 0.4f) cColor = float4(0.5f, 1.0f, 1.0f, 1.0f);
	else if (fDepth < 0.6f) cColor = float4(1.0f, 0.0f, 1.0f, 1.0f);
	else if (fDepth < 0.8f) cColor = float4(0.5f, 0.5f, 1.0f, 1.0f);
	else if (fDepth < 0.9f) cColor = float4(0.5f, 1.0f, 0.5f, 1.0f);
	else cColor = float4(0.0f, 0.0f, 0.0f, 1.0f);

	return(cColor);
}

struct VS_SCREEN_RECT_TEXTURED_OUTPUT
{
	float4 position : SV_POSITION;
	float2 uv : TEXCOORD;
};

VS_SCREEN_RECT_TEXTURED_OUTPUT VSPostProcessing(uint nVertexID : SV_VertexID)
{
	VS_SCREEN_RECT_TEXTURED_OUTPUT output = (VS_TEXTURED_OUTPUT)0;

	if (nVertexID == 0) { output.position = float4(-1.0f, +1.0f, 0.0f, 1.0f); output.uv = float2(0.0f, 0.0f); }
	else if (nVertexID == 1) { output.position = float4(+1.0f, +1.0f, 0.0f, 1.0f); output.uv = float2(1.0f, 0.0f); }
	else if (nVertexID == 2) { output.position = float4(+1.0f, -1.0f, 0.0f, 1.0f); output.uv = float2(1.0f, 1.0f); }

	else if (nVertexID == 3) { output.position = float4(-1.0f, +1.0f, 0.0f, 1.0f); output.uv = float2(0.0f, 0.0f); }
	else if (nVertexID == 4) { output.position = float4(+1.0f, -1.0f, 0.0f, 1.0f); output.uv = float2(1.0f, 1.0f); }
	else if (nVertexID == 5) { output.position = float4(-1.0f, -1.0f, 0.0f, 1.0f); output.uv = float2(0.0f, 1.0f); }

	return(output);
}

float4 PSPostProcessing(VS_TEXTURED_OUTPUT input) : SV_Target
{
	float4 cColor;
	if (f3PlayerVelocity.x > 0.02f || f3PlayerVelocity.y > 0.02f) cColor = gtxtOutput.Sample(gSamplerState, input.uv);
	else if (gnBlurMode & (DEBUG_BLURRING)) cColor = gtxtOutput.Sample(gSamplerState, input.uv);
	else cColor = gtxtInputTextures[0].Sample(gSamplerState, input.uv);

	return(cColor);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define PARTICLE_TYPE_EMITTER	0
#define PARTICLE_TYPE_FLARE		0x0ff

struct VS_PARTICLE_INPUT
{
	float3 position : POSITION;
	float3 color : COLOR;
	float3 velocity : VELOCITY;
	float3 acceleration : ACCELERATION;
	float2 size : SIZE;
	float2 age : AGELIFETIME; //(Age, Lifetime)
	uint type : PARTICLETYPE;
};

VS_PARTICLE_INPUT VSParticleStreamOutput(VS_PARTICLE_INPUT input)
{
	return(input);
}

Buffer<float4> gRandomBuffer : register(t30);

float3 GetParticleColor(float fAge, float fLifetime)
{
	float3 cColor = float3(1.0f, 1.0f, 1.0f);

	if (fAge == 0.0f) cColor = float3(0.0f, 1.0f, 0.0f);
	else if (fLifetime == 0.0f)
		cColor = float3(1.0f, 1.0f, 0.0f);
	else
	{
		float t = fAge / fLifetime;
		cColor = lerp(float3(1.0f, 0.0f, 0.0f), float3(0.0f, 0.0f, 1.0f), t * 1.0f);
	}

	return(cColor);
}

void GetBillboardCorners(float3 position, float2 size, out float4 pf4Positions[4])
{
	float3 f3Up = float3(0.0f, 1.0f, 0.0f);
	float3 f3Look = normalize(gvCameraPosition - position);
	float3 f3Right = normalize(cross(f3Up, f3Look));

	pf4Positions[0] = float4(position + size.x * f3Right - size.y * f3Up, 1.0f);
	pf4Positions[1] = float4(position + size.x * f3Right + size.y * f3Up, 1.0f);
	pf4Positions[2] = float4(position - size.x * f3Right - size.y * f3Up, 1.0f);
	pf4Positions[3] = float4(position - size.x * f3Right + size.y * f3Up, 1.0f);
}

void GetPositions(float3 position, float2 f2Size, out float3 pf3Positions[8])
{
	float3 f3Right = float3(1.0f, 0.0f, 0.0f);
	float3 f3Up = float3(0.0f, 1.0f, 0.0f);
	float3 f3Look = float3(0.0f, 0.0f, 1.0f);

	float3 f3Extent = normalize(float3(1.0f, 1.0f, 1.0f));

	pf3Positions[0] = position + float3(-f2Size.x/0.8f, 0.0f, -f2Size.y);
	pf3Positions[1] = position + float3(-f2Size.x, 0.0f, +f2Size.y);
	pf3Positions[2] = position + float3(+f2Size.x, 0.0f, -f2Size.y/0.7f);
	pf3Positions[3] = position + float3(+f2Size.x, 0.0f, +f2Size.y);
	pf3Positions[4] = position + float3(-f2Size.x, 0.0f, 0.0f);
	pf3Positions[5] = position + float3(+f2Size.x/0.9f, 0.0f, 0.0f);
	pf3Positions[6] = position + float3(0.0f, 0.0f, +f2Size.y);
	pf3Positions[7] = position + float3(0.0f, 0.0f, -f2Size.y);
}

[maxvertexcount(9)]
void GSParticleStreamOutput(point VS_PARTICLE_INPUT input[1], inout PointStream<VS_PARTICLE_INPUT> output)
{
	input[0].age.x += gfElapsedTime;
	VS_PARTICLE_INPUT particle = input[0];
	if (particle.type == PARTICLE_TYPE_EMITTER)
	{
		particle.color = float3(1.0f, 0.0f, 0.0f);
		output.Append(particle);

		float4 f4Random = gRandomBuffer.Load(int(fmod(gfCurrentTime - floor(gfCurrentTime) * 1000.0f, 1000.0f)));

		if (particle.age.x > 0.005f)
		{
			float3 pf3Positions[8];
			GetPositions(particle.position, float2(5.5, 6.3f), pf3Positions);

			particle.color = float3(0.0f, 0.0f, 1.0f);
			particle.age.x = 0.0f;

			for (int j = 0; j < 8; j++)
			{
				particle.type = (j >= 4) ? PARTICLE_TYPE_EMITTER : PARTICLE_TYPE_FLARE;
				particle.position = pf3Positions[j].xyz;
				particle.age.y = (particle.type == PARTICLE_TYPE_EMITTER) ? 0.25f : 4.0f;
				output.Append(particle);
			}
		}
	}
	else
	{
		if (particle.age.x < particle.age.y) output.Append(particle);
	}
}

VS_PARTICLE_INPUT VSParticleDraw(VS_PARTICLE_INPUT input)
{
	return(input);
}

struct GS_PARTICLE_OUTPUT
{
	float4 position : SV_Position;
	float3 color : COLOR;
	float2 uv : TEXCOORD;
	float2 age : AGELIFETIME; //(Age, Lifetime)
	uint type : PARTICLETYPE;
};

static float2 gf2QuadUVs[4] = { float2(0.0f,1.0f), float2(0.0f,0.0f), float2(1.0f,1.0f), float2(1.0f,0.0f) };

[maxvertexcount(4)]
void GSParticleDraw(point VS_PARTICLE_INPUT input[1], inout TriangleStream<GS_PARTICLE_OUTPUT> outputStream)
{
	float4 pVertices[4];
	//	GetBillboardCorners(input[0].position, input[0].size * 0.5f, pVertices);
	GetBillboardCorners(mul(float4(input[0].position, 1.0f), gmtxGameObject).xyz, input[0].size * 0.5f, pVertices);

	GS_PARTICLE_OUTPUT output = (GS_PARTICLE_OUTPUT)0;
	output.color = input[0].color;
	output.age = input[0].age;
	output.type = input[0].type;
	for (int i = 0; i < 4; i++)
	{
		output.position = mul(mul(pVertices[i], gmtxView), gmtxProjection);
		output.uv = gf2QuadUVs[i];

		outputStream.Append(output);
	}
}

Texture2D<float4> gtxtParticleTexture : register(t29);

PS_MULTIPLE_RENDER_TARGETS_OUTPUT PSParticleDraw(GS_PARTICLE_OUTPUT input) : SV_TARGET
{
	float4 cColor = gtxtParticleTexture.Sample(gSamplerState, input.uv);

	PS_MULTIPLE_RENDER_TARGETS_OUTPUT output;
	output.f4Scene = output.f4Color = cColor;
	output.fDepth = 1.0f - input.position.z;

	return(output);
}
