//
// Generated by Microsoft (R) D3DX9 Shader Assembler
//
//  Source: LightmappedGeneric_BaseTexture.psh
//  Flags: /Zi 
//


static unsigned int pixelShader_LightmappedGeneric_BaseTexture_0[] =
{
    0xffff0101, 0x0030fffe, 0x47554244, 0x00000028, 0x000000a4, 0x00000000, 
    0x00000001, 0x00000090, 0x00000002, 0x00000094, 0x00000000, 0x00000000, 
    0x00000000, 0x505c3a43, 0x656a6f72, 0x5c737463, 0x61746542, 0x65786946, 
    0x65425c72, 0x69466174, 0x676e6978, 0x6372735c, 0x69616d5f, 0x616d5c6e, 
    0x69726574, 0x79736c61, 0x6d657473, 0x6474735c, 0x64616873, 0x5c737265, 
    0x6867694c, 0x70616d74, 0x47646570, 0x72656e65, 0x425f6369, 0x54657361, 
    0x75747865, 0x702e6572, 0xab006873, 0x00000028, 0x0000000c, 0x000000c8, 
    0x0000000d, 0x000000d0, 0x58443344, 0x68532039, 0x72656461, 0x73734120, 
    0x6c626d65, 0xab007265, 0x00000042, 0xb00f0000, 0x00000001, 0x800f0000, 
    0xb0e40000, 0x0000ffff
};
static PrecompiledShaderByteCode_t LightmappedGeneric_BaseTexture_pixel_shaders[1] = 
{
	{ pixelShader_LightmappedGeneric_BaseTexture_0, sizeof( pixelShader_LightmappedGeneric_BaseTexture_0 ) },
};
struct LightmappedGeneric_BaseTexturePixelShader_t : public PrecompiledShader_t
{
	LightmappedGeneric_BaseTexturePixelShader_t()
	{
		m_nFlags = SHADER_CUSTOM_ENUMERATION;
		m_pByteCode = LightmappedGeneric_BaseTexture_pixel_shaders;
		m_nShaderCount = 1;
		m_pName = "LightmappedGeneric_BaseTexture";
		GetShaderDLL()->InsertPrecompiledShader( PRECOMPILED_PIXEL_SHADER, this );
	}
};
static LightmappedGeneric_BaseTexturePixelShader_t LightmappedGeneric_BaseTexture_PixelShaderInstance;
