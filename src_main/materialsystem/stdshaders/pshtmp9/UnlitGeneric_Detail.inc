//
// Generated by Microsoft (R) D3DX9 Shader Assembler
//
//  Source: UnlitGeneric_Detail.psh
//  Flags: /Zi 
//


static unsigned int pixelShader_UnlitGeneric_Detail_0[] =
{
    0xffff0101, 0x0031fffe, 0x47554244, 0x00000028, 0x000000a8, 0x00000000, 
    0x00000001, 0x00000084, 0x00000004, 0x00000088, 0x00000000, 0x00000000, 
    0x00000000, 0x505c3a43, 0x656a6f72, 0x5c737463, 0x61746542, 0x65786946, 
    0x65425c72, 0x69466174, 0x676e6978, 0x6372735c, 0x69616d5f, 0x616d5c6e, 
    0x69726574, 0x79736c61, 0x6d657473, 0x6474735c, 0x64616873, 0x5c737265, 
    0x696c6e55, 0x6e654774, 0x63697265, 0x7465445f, 0x2e6c6961, 0x00687370, 
    0x00000028, 0x0000000b, 0x000000cc, 0x0000000c, 0x000000d4, 0x0000000e, 
    0x000000dc, 0x0000000f, 0x000000ec, 0x58443344, 0x68532039, 0x72656461, 
    0x73734120, 0x6c626d65, 0xab007265, 0x00000042, 0xb00f0000, 0x00000042, 
    0xb00f0003, 0x00000005, 0x800f0000, 0xb0e40000, 0x90e40000, 0x00000005, 
    0x81070000, 0x80e40000, 0xb0e40003, 0x0000ffff
};
static PrecompiledShaderByteCode_t UnlitGeneric_Detail_pixel_shaders[1] = 
{
	{ pixelShader_UnlitGeneric_Detail_0, sizeof( pixelShader_UnlitGeneric_Detail_0 ) },
};
struct UnlitGeneric_DetailPixelShader_t : public PrecompiledShader_t
{
	UnlitGeneric_DetailPixelShader_t()
	{
		m_nFlags = SHADER_CUSTOM_ENUMERATION;
		m_pByteCode = UnlitGeneric_Detail_pixel_shaders;
		m_nShaderCount = 1;
		m_pName = "UnlitGeneric_Detail";
		GetShaderDLL()->InsertPrecompiledShader( PRECOMPILED_PIXEL_SHADER, this );
	}
};
static UnlitGeneric_DetailPixelShader_t UnlitGeneric_Detail_PixelShaderInstance;
