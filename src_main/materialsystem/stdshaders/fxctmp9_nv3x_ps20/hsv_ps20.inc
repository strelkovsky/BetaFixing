class hsv_ps20_Index
{
public:
	hsv_ps20_Index()
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
//   fxc /Dmain=main /Emain /Tps_2_0 /nologo /Fhtmpshader.h hsv_ps20.fxc
//
//
// Parameters:
//
//   sampler2D BaseTextureSampler;
//
//
// Registers:
//
//   Name               Reg   Size
//   ------------------ ----- ----
//   BaseTextureSampler s0       1
//


static unsigned int pixelShader_hsv_ps20_0[] =
{
    0xffff0200, 0x0021fffe, 0x42415443, 0x0000001c, 0x0000005b, 0xffff0200, 
    0x00000001, 0x0000001c, 0x00000000, 0x00000054, 0x00000030, 0x00000003, 
    0x00000001, 0x00000044, 0x00000000, 0x65736142, 0x74786554, 0x53657275, 
    0x6c706d61, 0xab007265, 0x000c0004, 0x00010001, 0x00000001, 0x00000000, 
    0x325f7370, 0x4d00305f, 0x6f726369, 0x74666f73, 0x29522820, 0x44334420, 
    0x53203958, 0x65646168, 0x6f432072, 0x6c69706d, 0xab007265, 0x05000051, 
    0xa00f0000, 0x3f800000, 0x00000000, 0x00000000, 0x00000000, 0x0200001f, 
    0x80000000, 0xb0030000, 0x0200001f, 0x90000000, 0xa00f0800, 0x03000042, 
    0x800f0001, 0xb0e40000, 0xa0e40800, 0x0300000b, 0x80080002, 0x80000001, 
    0x80550001, 0x0300000b, 0x80070000, 0x80aa0001, 0x80ff0002, 0x02000001, 
    0x80080000, 0xa0000000, 0x02000001, 0x800f0800, 0x80e40000, 0x0000ffff
};
static PrecompiledShaderByteCode_t hsv_ps20_pixel_shaders[1] = 
{
	{ pixelShader_hsv_ps20_0, sizeof( pixelShader_hsv_ps20_0 ) },
};
struct hsv_ps20PixelShader_t : public PrecompiledShader_t
{
	hsv_ps20PixelShader_t()
	{
		m_nFlags = SHADER_CUSTOM_ENUMERATION;
		m_nCentroidMask = 0x0;
		m_pByteCode = hsv_ps20_pixel_shaders;
		m_nShaderCount = 1;
		m_pName = "hsv_ps20";
		m_nDynamicCombos = 1;
		GetShaderDLL()->InsertPrecompiledShader( PRECOMPILED_PIXEL_SHADER, this );
	}
};
static hsv_ps20PixelShader_t hsv_ps20_PixelShaderInstance;
#endif // DEFINE_SHADERS
