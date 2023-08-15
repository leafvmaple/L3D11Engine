struct MaterialPixelParameters
{
    float3  WorldNormal;
    float3  WorldPosition; 
    float3  VertexPosition;
    float3  VertexNormal;
    float4  ScreenPosition;
	float4	OutPosition;
    float4  VertexColor;
    float3  Tangent;
    float3  Binormal;
    float   FogFactor;
    float4  ModelColor;
    float3  ModelColorOffset;
	float4  ParticleVertexColor;
	float4  WeightHeightMapTexcoord;

	float4x4 MatrixWorld;
	float4x4 MatrixWorldInverse;
	float4x4 MatrixWorldInverseTranspose;
	
	float2  ScreenMotion;
	float   MotionPrecision;
	float   IsMoving;
	float   IsFrontFace;

	float	BaseLayerHeight;
	float	CurrentLayerHeight;
	float	PreviousLayerHeight;

    float   CurrentLayerMask;
    float   CurrentLayerInfo;

	float2  SplineUV;
	float3  SplineDir;
	float   SplineT;
};