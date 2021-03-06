class sky_vs20_Index
{
public:
	sky_vs20_Index()
	{
	}
	int GetIndex()
	{
		// Asserts to make sure that we aren't using any skipped combinations.
		// Asserts to make sure that we are setting all of the combination vars.
#ifdef _DEBUG
#endif // _DEBUG
		return 0;
	}
};
#ifdef DEFINE_SHADERS
//
// Generated by Microsoft (R) D3DX9 Shader Compiler
//
//   fxc /Dmain=main /Emain /Tvs_2_0 /nologo /Fhtmpshader.h sky_vs20.fxc
//
//
// Parameters:
//
//   float4 cBaseTexCoordTransform[2];
//   float4x4 cModelViewProj;
//
//
// Registers:
//
//   Name                   Reg   Size
//   ---------------------- ----- ----
//   cModelViewProj         c4       4
//   cBaseTexCoordTransform c90      2
//


static unsigned int pixelShader_sky_vs20_0[] =
{
    0xfffe0200, 0x002ffffe, 0x42415443, 0x0000001c, 0x00000093, 0xfffe0200, 
    0x00000002, 0x0000001c, 0x00000000, 0x0000008c, 0x00000044, 0x005a0002, 
    0x00000002, 0x0000005c, 0x00000000, 0x0000006c, 0x00040002, 0x00000004, 
    0x0000007c, 0x00000000, 0x73614263, 0x78655465, 0x726f6f43, 0x61725464, 
    0x6f66736e, 0xab006d72, 0x00030001, 0x00040001, 0x00000002, 0x00000000, 
    0x646f4d63, 0x69566c65, 0x72507765, 0xab006a6f, 0x00030003, 0x00040004, 
    0x00000001, 0x00000000, 0x325f7376, 0x4d00305f, 0x6f726369, 0x74666f73, 
    0x29522820, 0x44334420, 0x53203958, 0x65646168, 0x6f432072, 0x6c69706d, 
    0xab007265, 0x0200001f, 0x80000000, 0x900f0000, 0x0200001f, 0x80000005, 
    0x900f0001, 0x03000009, 0xc0010000, 0x90e40000, 0xa0e40004, 0x03000009, 
    0xc0020000, 0x90e40000, 0xa0e40005, 0x03000009, 0xc0040000, 0x90e40000, 
    0xa0e40006, 0x03000009, 0xc0080000, 0x90e40000, 0xa0e40007, 0x03000009, 
    0xe0010000, 0x90e40001, 0xa0e4005a, 0x03000009, 0xe0020000, 0x90e40001, 
    0xa0e4005b, 0x0000ffff
};
static PrecompiledShaderByteCode_t sky_vs20_pixel_shaders[1] = 
{
	{ pixelShader_sky_vs20_0, sizeof( pixelShader_sky_vs20_0 ) },
};
struct sky_vs20PixelShader_t : public PrecompiledShader_t
{
	sky_vs20PixelShader_t()
	{
		m_nFlags = SHADER_CUSTOM_ENUMERATION;
		m_nCentroidMask = 0x0;
		m_pByteCode = sky_vs20_pixel_shaders;
		m_nShaderCount = 1;
		m_pName = "sky_vs20";
		m_nDynamicCombos = 1;
		GetShaderDLL()->InsertPrecompiledShader( PRECOMPILED_VERTEX_SHADER, this );
	}
};
static sky_vs20PixelShader_t sky_vs20_PixelShaderInstance;
#endif // DEFINE_SHADERS
