//
// Generated by Microsoft (R) D3DX9 Shader Assembler
//
//  Source: VertexLitGeneric_NoTexture.psh
//  Flags: /Zi /DTEXKILL="0" 
//


static unsigned int pixelShader_VertexLitGeneric_NoTexture_0[] =
{
    0xffff0101, 0x002ffffe, 0x47554244, 0x00000028, 0x000000a0, 0x00000000, 
    0x00000001, 0x0000008c, 0x00000002, 0x00000090, 0x00000000, 0x00000000, 
    0x00000000, 0x505c3a43, 0x656a6f72, 0x5c737463, 0x61746542, 0x65786946, 
    0x65425c72, 0x69466174, 0x676e6978, 0x6372735c, 0x69616d5f, 0x616d5c6e, 
    0x69726574, 0x79736c61, 0x6d657473, 0x6474735c, 0x64616873, 0x5c737265, 
    0x74726556, 0x694c7865, 0x6e654774, 0x63697265, 0x546f4e5f, 0x75747865, 
    0x702e6572, 0xab006873, 0x00000028, 0x0000000f, 0x000000c4, 0x00000010, 
    0x000000d4, 0x58443344, 0x68532039, 0x72656461, 0x73734120, 0x6c626d65, 
    0xab007265, 0x00000005, 0x800f0000, 0x90e40000, 0xa0e40003, 0x00000005, 
    0x81070000, 0xa0e40000, 0x80e40000, 0x0000ffff
};
//
// Generated by Microsoft (R) D3DX9 Shader Assembler
//
//  Source: VertexLitGeneric_NoTexture.psh
//  Flags: /Zi /DTEXKILL="1" 
//


static unsigned int pixelShader_VertexLitGeneric_NoTexture_1[] =
{
    0xffff0101, 0x0031fffe, 0x47554244, 0x00000028, 0x000000a8, 0x00000000, 
    0x00000001, 0x0000008c, 0x00000003, 0x00000090, 0x00000000, 0x00000000, 
    0x00000000, 0x505c3a43, 0x656a6f72, 0x5c737463, 0x61746542, 0x65786946, 
    0x65425c72, 0x69466174, 0x676e6978, 0x6372735c, 0x69616d5f, 0x616d5c6e, 
    0x69726574, 0x79736c61, 0x6d657473, 0x6474735c, 0x64616873, 0x5c737265, 
    0x74726556, 0x694c7865, 0x6e654774, 0x63697265, 0x546f4e5f, 0x75747865, 
    0x702e6572, 0xab006873, 0x00000028, 0x0000000d, 0x000000cc, 0x0000000f, 
    0x000000d4, 0x00000010, 0x000000e4, 0x58443344, 0x68532039, 0x72656461, 
    0x73734120, 0x6c626d65, 0xab007265, 0x00000041, 0xb00f0003, 0x00000005, 
    0x800f0000, 0x90e40000, 0xa0e40003, 0x00000005, 0x81070000, 0xa0e40000, 
    0x80e40000, 0x0000ffff
};
static PrecompiledShaderByteCode_t VertexLitGeneric_NoTexture_pixel_shaders[2] = 
{
	{ pixelShader_VertexLitGeneric_NoTexture_0, sizeof( pixelShader_VertexLitGeneric_NoTexture_0 ) },
	{ pixelShader_VertexLitGeneric_NoTexture_1, sizeof( pixelShader_VertexLitGeneric_NoTexture_1 ) },
};
struct VertexLitGeneric_NoTexturePixelShader_t : public PrecompiledShader_t
{
	VertexLitGeneric_NoTexturePixelShader_t()
	{
		m_nFlags = SHADER_CUSTOM_ENUMERATION;
		m_pByteCode = VertexLitGeneric_NoTexture_pixel_shaders;
		m_nShaderCount = 2;
		m_pName = "VertexLitGeneric_NoTexture";
		GetShaderDLL()->InsertPrecompiledShader( PRECOMPILED_PIXEL_SHADER, this );
	}
};
static VertexLitGeneric_NoTexturePixelShader_t VertexLitGeneric_NoTexture_PixelShaderInstance;
