//
// Generated by Microsoft (R) D3DX9 Shader Assembler
//
//  Source: white.psh
//  Flags: /Zi 
//


static unsigned int pixelShader_white_0[] =
{
    0xffff0101, 0x002afffe, 0x47554244, 0x00000028, 0x0000008c, 0x00000000, 
    0x00000001, 0x00000078, 0x00000002, 0x0000007c, 0x00000000, 0x00000000, 
    0x00000000, 0x505c3a43, 0x656a6f72, 0x5c737463, 0x61746542, 0x65786946, 
    0x65425c72, 0x69466174, 0x676e6978, 0x6372735c, 0x69616d5f, 0x616d5c6e, 
    0x69726574, 0x79736c61, 0x6d657473, 0x6474735c, 0x64616873, 0x5c737265, 
    0x74696877, 0x73702e65, 0xabab0068, 0x00000028, 0x00000007, 0x000000b0, 
    0x00000008, 0x000000c8, 0x58443344, 0x68532039, 0x72656461, 0x73734120, 
    0x6c626d65, 0xab007265, 0x00000051, 0xa00f0000, 0x3f800000, 0x3f800000, 
    0x3f800000, 0x3f800000, 0x00000001, 0x800f0000, 0xa0e40000, 0x0000ffff
};
static PrecompiledShaderByteCode_t white_pixel_shaders[1] = 
{
	{ pixelShader_white_0, sizeof( pixelShader_white_0 ) },
};
struct whitePixelShader_t : public PrecompiledShader_t
{
	whitePixelShader_t()
	{
		m_nFlags = SHADER_CUSTOM_ENUMERATION;
		m_pByteCode = white_pixel_shaders;
		m_nShaderCount = 1;
		m_pName = "white";
		GetShaderDLL()->InsertPrecompiledShader( PRECOMPILED_PIXEL_SHADER, this );
	}
};
static whitePixelShader_t white_PixelShaderInstance;
