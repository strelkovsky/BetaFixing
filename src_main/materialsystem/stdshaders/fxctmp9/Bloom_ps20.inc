class bloom_ps20_Index
{
public:
	bloom_ps20_Index()
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
//   fxc /Dmain=main /Emain /Tps_2_0 /nologo /Fhtmpshader.h Bloom_ps20.fxc
//
//
// Parameters:
//
//   sampler2D BlurSampler;
//   sampler2D FBSampler;
//
//
// Registers:
//
//   Name         Reg   Size
//   ------------ ----- ----
//   FBSampler    s0       1
//   BlurSampler  s1       1
//


static unsigned int pixelShader_Bloom_ps20_0[] =
{
    0xffff0200, 0x0027fffe, 0x42415443, 0x0000001c, 0x00000071, 0xffff0200, 
    0x00000002, 0x0000001c, 0x00000000, 0x0000006a, 0x00000044, 0x00010003, 
    0x00000001, 0x00000050, 0x00000000, 0x00000060, 0x00000003, 0x00000001, 
    0x00000050, 0x00000000, 0x72756c42, 0x706d6153, 0x0072656c, 0x000c0004, 
    0x00010001, 0x00000001, 0x00000000, 0x61534246, 0x656c706d, 0x73700072, 
    0x305f325f, 0x63694d00, 0x6f736f72, 0x28207466, 0x44202952, 0x39584433, 
    0x61685320, 0x20726564, 0x706d6f43, 0x72656c69, 0xababab00, 0x05000051, 
    0xa00f0000, 0x41800000, 0x3f800000, 0x00000000, 0x00000000, 0x0200001f, 
    0x80000000, 0xb0030000, 0x0200001f, 0x90000000, 0xa00f0800, 0x0200001f, 
    0x90000000, 0xa00f0801, 0x03000042, 0x800f0001, 0xb0e40000, 0xa0e40801, 
    0x03000042, 0x800f0000, 0xb0e40000, 0xa0e40800, 0x03000005, 0x80070001, 
    0x80ff0001, 0x80e40001, 0x04000004, 0x80070000, 0x80e40001, 0xa0000000, 
    0x80e40000, 0x02000001, 0x80080000, 0xa0550000, 0x02000001, 0x800f0800, 
    0x80e40000, 0x0000ffff
};
static PrecompiledShaderByteCode_t Bloom_ps20_pixel_shaders[1] = 
{
	{ pixelShader_Bloom_ps20_0, sizeof( pixelShader_Bloom_ps20_0 ) },
};
struct Bloom_ps20PixelShader_t : public PrecompiledShader_t
{
	Bloom_ps20PixelShader_t()
	{
		m_nFlags = SHADER_CUSTOM_ENUMERATION;
		m_nCentroidMask = 0x0;
		m_pByteCode = Bloom_ps20_pixel_shaders;
		m_nShaderCount = 1;
		m_pName = "Bloom_ps20";
		m_nDynamicCombos = 1;
		GetShaderDLL()->InsertPrecompiledShader( PRECOMPILED_PIXEL_SHADER, this );
	}
};
static Bloom_ps20PixelShader_t Bloom_ps20_PixelShaderInstance;
#endif // DEFINE_SHADERS