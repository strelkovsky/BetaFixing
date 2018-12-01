//========= Copyright � 1996-2001, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================
#include "cbase.h"
#include "beamdraw.h"
#include "enginesprite.h"
#include "IViewRender_Beams.h"
#include "view.h"
#include "iviewrender.h"
#include "engine/ivmodelinfo.h"
#include "fx_line.h"

ConVar r_DrawBeams( "r_DrawBeams", "1" );
extern ConVar mat_wireframe;


// ------------------------------------------------------------------------------------------ //
// CBeamSegDraw implementation.
// ------------------------------------------------------------------------------------------ //
void CBeamSegDraw::Start( int nSegs, IMaterial *pMaterial )
{
	m_nSegsDrawn = 0;
	m_nTotalSegs = nSegs;

	Assert( nSegs >= 2 );

	IMesh *pMesh = materials->GetDynamicMesh( true, NULL, NULL, pMaterial );
	m_Mesh.Begin( pMesh, MATERIAL_TRIANGLE_STRIP, (nSegs-1) * 2 );
}

inline void CBeamSegDraw::ComputeNormal( const Vector &vStartPos, const Vector &vNextPos, Vector *pNormal )
{
	// vTangentY = line vector for beam
	Vector vTangentY;
	VectorSubtract( vStartPos, vNextPos, vTangentY );
	
	// vDirToBeam = vector from viewer origin to beam
	Vector vDirToBeam;
	VectorSubtract( vStartPos, CurrentViewOrigin(), vDirToBeam );

	// Get a vector that is perpendicular to us and perpendicular to the beam.
	// This is used to fatten the beam.
	CrossProduct( vTangentY, vDirToBeam, *pNormal );
	VectorNormalizeFast( *pNormal );
}

inline void CBeamSegDraw::SpecifySeg( const Vector &vNormal )
{
	// SUCKY: Need to do a fair amount more work to get the tangent owing to the averaged normal
	Vector vDirToBeam, vTangentY;
	VectorSubtract( m_Seg.m_vPos, CurrentViewOrigin(), vDirToBeam );
	CrossProduct( vDirToBeam, vNormal, vTangentY );
	VectorNormalizeFast( vTangentY );

	// Build the endpoints.
	Vector vPoint1, vPoint2;
	VectorMA( m_Seg.m_vPos,  m_Seg.m_flWidth*0.5f, vNormal, vPoint1 );
	VectorMA( m_Seg.m_vPos, -m_Seg.m_flWidth*0.5f, vNormal, vPoint2 );

	// Specify the points.
	m_Mesh.Position3fv( vPoint1.Base() );
	m_Mesh.Color4f( VectorExpand( m_Seg.m_vColor ), m_Seg.m_flAlpha );
	m_Mesh.TexCoord2f( 0, 0, m_Seg.m_flTexCoord );
	m_Mesh.TexCoord2f( 1, 0, m_Seg.m_flTexCoord );
	m_Mesh.TangentS3fv( vNormal.Base() );
	m_Mesh.TangentT3fv( vTangentY.Base() );
	m_Mesh.AdvanceVertex();
	
	m_Mesh.Position3fv( vPoint2.Base() );
	m_Mesh.Color4f( VectorExpand( m_Seg.m_vColor ), m_Seg.m_flAlpha );
	m_Mesh.TexCoord2f( 0, 1, m_Seg.m_flTexCoord );
	m_Mesh.TexCoord2f( 1, 1, m_Seg.m_flTexCoord );
	m_Mesh.TangentS3fv( vNormal.Base() );
	m_Mesh.TangentT3fv( vTangentY.Base() );
	m_Mesh.AdvanceVertex();
}


void CBeamSegDraw::NextSeg( CBeamSeg *pSeg )
{
 	if ( m_nSegsDrawn > 0 )
	{
		// Get a vector that is perpendicular to us and perpendicular to the beam.
		// This is used to fatten the beam.
		Vector vNormal, vAveNormal;
		ComputeNormal( m_Seg.m_vPos, pSeg->m_vPos, &vNormal );

		if ( m_nSegsDrawn > 1 )
		{
			// Average this with the previous normal
			VectorAdd( vNormal, m_vNormalLast, vAveNormal );
			vAveNormal *= 0.5f;
			VectorNormalizeFast( vAveNormal );
		}
		else
		{
			vAveNormal = vNormal;
		}

		m_vNormalLast = vNormal;
		SpecifySeg( vAveNormal );
	}

	m_Seg = *pSeg;
	++m_nSegsDrawn;

 	if( m_nSegsDrawn == m_nTotalSegs )
	{
		SpecifySeg( m_vNormalLast );
	}
}

void CBeamSegDraw::End()
{
	m_Mesh.End( false, true );
}




//-----------------------------------------------------------------------------
// Purpose: Retrieve sprite object and set it up for rendering
// Input  : *pSpriteModel - 
//			frame - 
//			rendermode - 
// Output : CEngineSprite
//-----------------------------------------------------------------------------
CEngineSprite *Draw_SetSpriteTexture( const model_t *pSpriteModel, int frame, int rendermode )
{
	CEngineSprite			*psprite;
	IMaterial		*material;

	psprite = ( CEngineSprite * )modelinfo->GetModelExtraData( pSpriteModel );
	Assert( psprite );

	material = psprite->GetMaterial();
	if( !material )
		return NULL;
	
	if ( mat_wireframe.GetBool() )
	{
		IMaterial *pWireframe = materials->FindMaterial("shadertest/wireframevertexcolor", NULL);
		materials->Bind( pWireframe, NULL );
		return psprite;
	}
	
	psprite->SetFrame( frame );
	psprite->SetRenderMode( rendermode );

	materials->Bind( material );
	return psprite;
}



//-----------------------------------------------------------------------------
// Purpose: 
// Input  : pMaterial - 
//			source - 
//			color - 
//-----------------------------------------------------------------------------
void DrawHalo(IMaterial* pMaterial, const Vector& source, float scale, float const* color )
{
	Vector		point, screen;
	
	IMesh* pMesh = materials->GetDynamicMesh( );

	CMeshBuilder meshBuilder;
	meshBuilder.Begin( pMesh, MATERIAL_QUADS, 1 );

	// Transform source into screen space
	ScreenTransform( source, screen );

	meshBuilder.Color3fv (color);
	meshBuilder.TexCoord2f (0, 0, 1);
	VectorMA (source, -scale, CurrentViewUp(), point);
	VectorMA (point, -scale, CurrentViewRight(), point);
	meshBuilder.Position3fv (point.Base());
	meshBuilder.AdvanceVertex();

	meshBuilder.Color3fv (color);
	meshBuilder.TexCoord2f (0, 0, 0);
	VectorMA (source, scale, CurrentViewUp(), point);
	VectorMA (point, -scale, CurrentViewRight(), point);
	meshBuilder.Position3fv (point.Base());
	meshBuilder.AdvanceVertex();

	meshBuilder.Color3fv (color);
	meshBuilder.TexCoord2f (0, 1, 0);
	VectorMA (source, scale, CurrentViewUp(), point);
	VectorMA (point, scale, CurrentViewRight(), point);
	meshBuilder.Position3fv (point.Base());
	meshBuilder.AdvanceVertex();

	meshBuilder.Color3fv (color);
	meshBuilder.TexCoord2f (0, 1, 1);
	VectorMA (source, -scale, CurrentViewUp(), point);
	VectorMA (point, scale, CurrentViewRight(), point);
	meshBuilder.Position3fv (point.Base());
	meshBuilder.AdvanceVertex();
	
	meshBuilder.End();
	pMesh->Draw();
}

static inline void DrawSpriteModel_Helper( CEngineSprite *psprite, const Vector &origin, float scale, int r, int g, int b, int a, const Vector& forward, const Vector& right, const Vector& up )
{
	Vector point;
	IMesh* pMesh = materials->GetDynamicMesh( );

	CMeshBuilder meshBuilder;
	meshBuilder.Begin( pMesh, MATERIAL_QUADS, 1 );

	unsigned char color[4];
	color[0] = r;
	color[1] = g;
	color[2] = b;
	color[3] = a;

	meshBuilder.Color4ubv (color);
	meshBuilder.TexCoord2f (0, 0, 1);
	VectorMA (origin, psprite->GetDown() * scale, up, point);
	VectorMA (point, psprite->GetLeft() * scale, right, point);
	meshBuilder.Position3fv (point.Base());
	meshBuilder.AdvanceVertex();

	meshBuilder.Color4ubv (color);
	meshBuilder.TexCoord2f (0, 0, 0);
	VectorMA (origin, psprite->GetUp() * scale, up, point);
	VectorMA (point, psprite->GetLeft() * scale, right, point);
	meshBuilder.Position3fv (point.Base());
	meshBuilder.AdvanceVertex();

	meshBuilder.Color4ubv (color);
	meshBuilder.TexCoord2f (0, 1, 0);
	VectorMA (origin, psprite->GetUp() * scale, up, point);
	VectorMA (point, psprite->GetRight() * scale, right, point);
	meshBuilder.Position3fv (point.Base());
	meshBuilder.AdvanceVertex();

	meshBuilder.Color4ubv (color);
	meshBuilder.TexCoord2f (0, 1, 1);
	VectorMA (origin, psprite->GetDown() * scale, up, point);
	VectorMA (point, psprite->GetRight() * scale, right, point);
	meshBuilder.Position3fv (point.Base());
	meshBuilder.AdvanceVertex();
	
	meshBuilder.End();
	pMesh->Draw();
}

//-----------------------------------------------------------------------------
// Purpose: Generic sprite model renderer
// Input  : *baseentity - 
//			*psprite - 
//			fscale - 
//			frame - 
//			rendermode - 
//			r - 
//			g - 
//			b - 
//			a - 
//			forward - 
//			right - 
//			up - 
//-----------------------------------------------------------------------------
void DrawSpriteModel( IClientEntity *baseentity, CEngineSprite *psprite, const Vector &origin, float fscale, float frame, 
	int rendermode, int r, int g, int b, int a, const Vector& forward, const Vector& right, const Vector& up )
{
	float			scale;
	IMaterial	*material;
	
	// don't even bother culling, because it's just a single
	// polygon without a surface cache
	//	frame = R_GetSpriteFrame( psprite, frame );
	if ( fscale > 0 )
		scale = fscale;
	else
		scale = 1.0;
	
	if( rendermode == kRenderNormal )
		render->SetBlend( 1.0 );
	
	material = psprite->GetMaterial();
	if( !material )
	{
		return;
	}
	psprite->SetRenderMode( rendermode );
	psprite->SetFrame( frame );

	materials->Bind( material, (IClientRenderable*)baseentity );

	DrawSpriteModel_Helper( psprite, origin, scale, r, g, b, a, forward, right, up );
	
	extern ConVar mat_wireframe;
	if( mat_wireframe.GetBool() )
	{
		IMaterial *pMaterial = materials->FindMaterial( "debug/debugspritewireframe", NULL );
		materials->Bind( pMaterial, NULL );
		DrawSpriteModel_Helper( psprite, origin, scale, r, g, b, a, forward, right, up );
	}
}


//-----------------------------------------------------------------------------
// Assumes the material has already been bound
//-----------------------------------------------------------------------------
void DrawSprite( const Vector &vecOrigin, float flWidth, float flHeight, color32 color )
{
	unsigned char pColor[4] = { color.r, color.g, color.b, color.a };

	// Generate half-widths
	flWidth *= 0.5f;
	flHeight *= 0.5f;

	// Compute direction vectors for the sprite
	Vector fwd, right( 1, 0, 0 ), up( 0, 1, 0 );
	VectorSubtract( CurrentViewOrigin(), vecOrigin, fwd );
	float flDist = VectorNormalize( fwd );
	if (flDist >= 1e-3)
	{
		CrossProduct( CurrentViewUp(), fwd, right );
		flDist = VectorNormalize( right );
		if (flDist >= 1e-3)
		{
			CrossProduct( fwd, right, up );
		}
		else
		{
			// In this case, fwd == g_vecVUp, it's right above or 
			// below us in screen space
			CrossProduct( fwd, CurrentViewRight(), up );
			VectorNormalize( up );
			CrossProduct( up, fwd, right );
		}
	}

	CMeshBuilder meshBuilder;
	Vector point;
	IMesh* pMesh = materials->GetDynamicMesh( );

	meshBuilder.Begin( pMesh, MATERIAL_QUADS, 1 );

	meshBuilder.Color4ubv (pColor);
	meshBuilder.TexCoord2f (0, 0, 1);
	VectorMA (vecOrigin, -flHeight, up, point);
	VectorMA (point, -flWidth, right, point);
	meshBuilder.Position3fv (point.Base());
	meshBuilder.AdvanceVertex();

	meshBuilder.Color4ubv (pColor);
	meshBuilder.TexCoord2f (0, 0, 0);
	VectorMA (vecOrigin, flHeight, up, point);
	VectorMA (point, -flWidth, right, point);
	meshBuilder.Position3fv (point.Base());
	meshBuilder.AdvanceVertex();

	meshBuilder.Color4ubv (pColor);
	meshBuilder.TexCoord2f (0, 1, 0);
	VectorMA (vecOrigin, flHeight, up, point);
	VectorMA (point, flWidth, right, point);
	meshBuilder.Position3fv (point.Base());
	meshBuilder.AdvanceVertex();

	meshBuilder.Color4ubv (pColor);
	meshBuilder.TexCoord2f (0, 1, 1);
	VectorMA (vecOrigin, -flHeight, up, point);
	VectorMA (point, flWidth, right, point);
	meshBuilder.Position3fv (point.Base());
	meshBuilder.AdvanceVertex();
	
	meshBuilder.End();
	pMesh->Draw();
}


//-----------------------------------------------------------------------------
// Compute vectors perpendicular to the beam
//-----------------------------------------------------------------------------
static void ComputeBeamPerpendicular( const Vector &vecBeamDelta, Vector *pPerp )
{
	// Direction in worldspace of the center of the beam
	Vector vecBeamCenter = vecBeamDelta;
	VectorNormalize( vecBeamCenter );

	CrossProduct( CurrentViewForward(), vecBeamCenter, *pPerp );
	VectorNormalize( *pPerp );
}



//-----------------------------------------------------------------------------
// Purpose: 
// Input  : noise_divisions - 
//			*prgNoise - 
//			*spritemodel - 
//			frame - 
//			rendermode - 
//			source - 
//			delta - 
//			flags - 
//			*color - 
//			fadescale - 
//-----------------------------------------------------------------------------
void DrawSegs( int noise_divisions, float *prgNoise, const model_t* spritemodel,
				float frame, int rendermode, const Vector& source, const Vector& delta, 
				float startWidth, float endWidth, float scale, float freq, float speed, int segments,
				int flags, float* color, float fadeLength )
{
	int				i, noiseIndex, noiseStep;
	float			div, length, fraction, factor, vLast, vStep, brightness;

	if( !r_DrawBeams.GetInt() )
		return;
	
	Assert( fadeLength >= 0.0f );
	CEngineSprite *pSprite = Draw_SetSpriteTexture( spritemodel, frame, rendermode );
	if ( !pSprite )
		return;

	if ( segments < 2 )
		return;
	
	length = VectorLength( delta );
	float flMaxWidth = max(startWidth, endWidth) * 0.5f;
	div = 1.0 / (segments-1);

	if ( length*div < flMaxWidth * 1.414 )
	{
		// Here, we have too many segments; we could get overlap... so lets have less segments
		segments = (int)(length / (flMaxWidth * 1.414)) + 1;
		if ( segments < 2 )
		{
			segments = 2;
		}
	}

	if ( segments > noise_divisions )		// UNDONE: Allow more segments?
	{
		segments = noise_divisions;
	}

	div = 1.0 / (segments-1);
	length *= 0.01;

	// UNDONE: Expose texture length scale factor to control "fuzziness"
	vStep = length*div;	// Texture length texels per space pixel

	// UNDONE: Expose this paramter as well(3.5)?  Texture scroll rate along beam
	vLast = fmod(freq*speed,1);	// Scroll speed 3.5 -- initial texture position, scrolls 3.5/sec (1.0 is entire texture)

	if ( flags & FBEAM_SINENOISE )
	{
		if ( segments < 16 )
		{
			segments = 16;
			div = 1.0 / (segments-1);
		}
		scale *= 100;
		length = segments * (1.0/10);
	}
	else
	{
		scale *= length;
	}

	// Iterator to resample noise waveform (it needs to be generated in powers of 2)
	noiseStep = (int)((float)(noise_divisions-1) * div * 65536.0f);
	noiseIndex = 0;
	
	if ( flags & FBEAM_SINENOISE )
	{
		noiseIndex = 0;
	}

	brightness = 1.0;
	if ( flags & FBEAM_SHADEIN )
	{
		brightness = 0;
	}

	// What fraction of beam should be faded
	Assert( fadeLength >= 0.0f );
	float fadeFraction = fadeLength/ delta.Length();
	
	// BUGBUG: This code generates NANs when fadeFraction is zero! REVIST!
	fadeFraction = clamp(fadeFraction,1e-6,1);

	// Choose two vectors that are perpendicular to the beam
	Vector perp1;
	ComputeBeamPerpendicular( delta, &perp1 );

	// Specify all the segments.
	CBeamSegDraw segDraw;
	segDraw.Start( segments, NULL );

	for ( i = 0; i < segments; i++ )
	{
		Assert( noiseIndex < (noise_divisions<<16) );
		CBeamSeg curSeg;
		curSeg.m_flAlpha = 1;

		fraction = i * div;

		// Fade in our out beam to fadeLength

		if ( (flags & FBEAM_SHADEIN) && (flags & FBEAM_SHADEOUT) )
		{
			if (fraction < 0.5)
			{
				brightness = 2*(fraction/fadeFraction);
			}
			else
			{
				brightness = 2*(1.0 - (fraction/fadeFraction));
			}
		}
		else if ( flags & FBEAM_SHADEIN )
		{
			brightness = fraction/fadeFraction;
		}
		else if ( flags & FBEAM_SHADEOUT )
		{
			brightness = 1.0 - (fraction/fadeFraction);
		}

		// clamps
		if (brightness < 0 )		
		{
			brightness = 0;
		}
		else if (brightness > 1)		
		{
			brightness = 1;
		}

		VectorScale( *((Vector*)color), brightness, curSeg.m_vColor );

		// UNDONE: Make this a spline instead of just a line?
		VectorMA( source, fraction, delta, curSeg.m_vPos );
 
		// Distort using noise
		if ( scale != 0 )
		{
			factor = prgNoise[noiseIndex>>16] * scale;
			if ( flags & FBEAM_SINENOISE )
			{
				float	s, c;
				SinCos( fraction*M_PI*length + freq, &s, &c );
				VectorMA( curSeg.m_vPos, factor * s, CurrentViewUp(), curSeg.m_vPos );
				// Rotate the noise along the perpendicluar axis a bit to keep the bolt from looking diagonal
				VectorMA( curSeg.m_vPos, factor * c, CurrentViewRight(), curSeg.m_vPos );
			}
			else
			{
				VectorMA( curSeg.m_vPos, factor, perp1, curSeg.m_vPos );
			}
		}

		// Specify the next segment.
		if( endWidth == startWidth )
		{
			curSeg.m_flWidth = startWidth * 2;
		}
		else
		{
			curSeg.m_flWidth = ((fraction*(endWidth-startWidth))+startWidth) * 2;
		}
		
		curSeg.m_flTexCoord = vLast;
		segDraw.NextSeg( &curSeg );


		vLast += vStep;	// Advance texture scroll (v axis only)
		noiseIndex += noiseStep;
	}

	segDraw.End();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CalcSegOrigin( Vector *vecOut, int iPoint, int noise_divisions, float *prgNoise, 
	const Vector &source, const Vector& delta, const Vector &perp, int segments, 
	float freq, float scale, float fraction, int flags )
{
	Assert( segments > 1 );

	float factor;
	float length = VectorLength( delta ) * 0.01;
	float div = 1.0 / (segments-1);

	// Iterator to resample noise waveform (it needs to be generated in powers of 2)
	int noiseStep = (int)((float)(noise_divisions-1) * div * 65536.0f);
	int noiseIndex = (iPoint+1) * noiseStep;

	// Sine noise beams have different length calculations
	if ( flags & FBEAM_SINENOISE )
	{
		length = segments * (1.0/10);
		noiseIndex = 0;
	}

	// UNDONE: Make this a spline instead of just a line?
	VectorMA( source, fraction, delta, *vecOut );

	// Distort using noise
	if ( scale != 0 )
	{
		factor = prgNoise[noiseIndex>>16] * scale;
		if ( flags & FBEAM_SINENOISE )
		{
			float	s, c;
			SinCos( fraction*M_PI*length + freq, &s, &c );
			VectorMA( *vecOut, factor * s, MainViewUp(), *vecOut );
			// Rotate the noise along the perpendicular axis a bit to keep the bolt from looking diagonal
			VectorMA( *vecOut, factor * c, MainViewRight(), *vecOut );
		}
		else
		{
			VectorMA( *vecOut, factor, perp, *vecOut );
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : noise_divisions - 
//			*prgNoise - 
//			*spritemodel - 
//			frame - 
//			rendermode - 
//			source - 
//			delta - 
//			flags - 
//			*color - 
//			fadescale - 
//-----------------------------------------------------------------------------
void DrawTeslaSegs( int noise_divisions, float *prgNoise, const model_t* spritemodel,
				float frame, int rendermode, const Vector&  source, const Vector&  delta, 
				float startWidth, float endWidth, float scale, float freq, float speed, int segments,
				int flags, float* color, float fadeLength )
{
	int				i;
	float			div, length, fraction, vLast, vStep, brightness;

	if( !r_DrawBeams.GetInt() )
		return;
	
	Assert( fadeLength >= 0.0f );
	CEngineSprite *pSprite = Draw_SetSpriteTexture( spritemodel, frame, rendermode );
	if ( !pSprite )
		return;

	if ( segments < 2 )
		return;
	
	if ( segments > noise_divisions )		// UNDONE: Allow more segments?
		segments = noise_divisions;

	length = VectorLength( delta ) * 0.01;
	div = 1.0 / (segments-1);

	// UNDONE: Expose texture length scale factor to control "fuzziness"
	vStep = length*div;	// Texture length texels per space pixel

	// UNDONE: Expose this paramter as well(3.5)?  Texture scroll rate along beam
	vLast = fmod(freq*speed,1);	// Scroll speed 3.5 -- initial texture position, scrolls 3.5/sec (1.0 is entire texture)

	brightness = 1.0;
	if ( flags & FBEAM_SHADEIN )
		brightness = 0;

	// What fraction of beam should be faded
	Assert( fadeLength >= 0.0f );
	float fadeFraction = fadeLength/ delta.Length();
	
	// BUGBUG: This code generates NANs when fadeFraction is zero! REVIST!
	fadeFraction = clamp(fadeFraction,1e-6,1);

	Vector perp;
	ComputeBeamPerpendicular( delta, &perp );

	// Specify all the segments.
	CBeamSegDraw segDraw;
	segDraw.Start( segments, NULL );

	// Keep track of how many times we've branched
	int iBranches = 0;

	Vector vecStart, vecEnd;
	float flWidth = 0;
	float flEndWidth = 0;

	for ( i = 0; i < segments; i++ )
	{
		CBeamSeg curSeg;
		curSeg.m_flAlpha = 1;

		fraction = i * div;

		// Fade in our out beam to fadeLength

		if ( (flags & FBEAM_SHADEIN) && (flags & FBEAM_SHADEOUT) )
		{
			if (fraction < 0.5)
			{
				brightness = 2*(fraction/fadeFraction);
			}
			else
			{
				brightness = 2*(1.0 - (fraction/fadeFraction));
			}
		}
		else if ( flags & FBEAM_SHADEIN )
		{
			brightness = fraction/fadeFraction;
		}
		else if ( flags & FBEAM_SHADEOUT )
		{
			brightness = 1.0 - (fraction/fadeFraction);
		}

		// clamps
		if (brightness < 0 )		
		{
			brightness = 0;
		}
		else if (brightness > 1)		
		{
			brightness = 1;
		}

		VectorScale( *((Vector*)color), brightness, curSeg.m_vColor );

		CalcSegOrigin( &curSeg.m_vPos, i, noise_divisions, prgNoise, source, delta, perp, segments, freq, scale, fraction, flags );

		// Specify the next segment.
		if( endWidth == startWidth )
			curSeg.m_flWidth = startWidth * 2;
		else
			curSeg.m_flWidth = ((fraction*(endWidth-startWidth))+startWidth) * 2;

		// Reduce the width by the current number of branches we've had
		for ( int j = 0; i < iBranches; j++ )
		{
			curSeg.m_flWidth *= 0.5;
		}
		
		curSeg.m_flTexCoord = vLast;

		segDraw.NextSeg( &curSeg );

		vLast += vStep;	// Advance texture scroll (v axis only)

		// Now see if we'd like to branch here
		// For now, always branch at the midpoint.
		// We could branch randomly, and multiple times per beam
		if ( i == (segments * 0.5) )
		{
			// Figure out what the new width would be
			// Halve the width because the beam is breaking in two, and halve it again because width is doubled above
			flWidth = curSeg.m_flWidth * 0.25;
			if ( flWidth > 1 )
			{
				iBranches++;

				// Get an endpoint for the new branch
				vecStart = curSeg.m_vPos;
				vecEnd = source + delta + (MainViewUp() * 32) + (MainViewRight() * 32);
				vecEnd -= vecStart;

				// Reduce the end width by the current number of branches we've had
				flEndWidth = endWidth;
				for ( int j = 0; i < iBranches; j++ )
				{
					flEndWidth *= 0.5;
				}
			}
		}
	}

	segDraw.End();

	// If we branched, draw the new beam too
	if ( iBranches )
	{
		DrawTeslaSegs( noise_divisions, prgNoise, spritemodel, frame, rendermode, 
			vecStart, vecEnd, flWidth, flEndWidth, scale, freq, speed, segments,
			flags, color, fadeLength );
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : noise_divisions - 
//			*prgNoise - 
//			*beammodel - 
//			*halomodel - 
//			flHaloScale - 
//			startWidth - 
//			endWidth - 
//			scale - 
//			freq - 
//			speed - 
//			segments - 
//			* - 
//-----------------------------------------------------------------------------

void DrawSplineSegs( int noise_divisions, float *prgNoise, 
				const model_t* beammodel, const model_t* halomodel, float flHaloScale,
				float frame, int rendermode, int numAttachments, Vector* attachment, 
				float startWidth, float endWidth, float scale, float freq, float speed, int segments,
				int flags, float* color, float fadeLength )
{
	int				noiseIndex, noiseStep;
	float			div, length, fraction, factor, vLast, vStep, brightness;
	float			scaledColor[3];

	model_t *beamsprite = ( model_t *)beammodel;
	model_t *halosprite = ( model_t *)halomodel;

	CEngineSprite *pBeamSprite = Draw_SetSpriteTexture( beamsprite, frame, rendermode );
	if ( !pBeamSprite )
		return;

	
	// Figure out the number of segments.
	if ( segments < 2 )
		return;
	
	if ( segments > noise_divisions )		// UNDONE: Allow more segments?
		segments = noise_divisions;

	if ( flags & FBEAM_SINENOISE )
	{
		if ( segments < 16 )
			segments = 16;
	}
	

	IMaterial *pBeamMaterial = pBeamSprite->GetMaterial();
	CBeamSegDraw segDraw;
	segDraw.Start( (segments-1)*(numAttachments-1), pBeamMaterial );

	CEngineSprite *pHaloSprite = (CEngineSprite *)modelinfo->GetModelExtraData( halosprite );
	IMaterial *pHaloMaterial = NULL;
	if ( pHaloSprite )
	{
		pHaloSprite->SetRenderMode( rendermode );
		pHaloMaterial = pHaloSprite->GetMaterial();
	}
	
	//-----------------------------------------------------------
	//  Calculate widthStep if start and end width are different
	//-----------------------------------------------------------
	float widthStep;
	if (startWidth != endWidth)
	{
		widthStep		= (endWidth - startWidth)/numAttachments;
	}
	else
	{
		widthStep		= 0;
	}
	
	// Calculate total length of beam 
	float flBeamLength = (attachment[0]-attachment[numAttachments-1]).Length();

	// What fraction of beam should be faded
	float fadeFraction = fadeLength/flBeamLength;
	if (fadeFraction > 1) 
	{
		fadeFraction = 1;
	}
	//---------------------------------------------------------------
	// Go through each attachment drawing spline beams between them
	//---------------------------------------------------------------
	Vector vLastPoint(0,0,0);
	Vector pPre;	// attachment point before the current beam
	Vector pStart;	// start of current beam
	Vector pEnd;	// end of current beam
	Vector pNext;	// attachment point after the current beam



	for (int j=0;j<numAttachments-1;j++)
	{
		if (j==0)
		{
			VectorCopy(attachment[0],pPre);
			VectorCopy(pPre,vLastPoint);
		}
		else
		{
			VectorCopy(attachment[j-1],pPre);
		}

		VectorCopy(attachment[j],	pStart);
		VectorCopy(attachment[j+1],	pEnd);

		if (j+2 >= numAttachments-1)
		{
			VectorCopy(attachment[j+1],pNext);
		}
		else
		{
			VectorCopy(attachment[j+2],pNext);
		}

		Vector vDelta;
		VectorSubtract(pEnd,pStart,vDelta);
		length = VectorLength( vDelta ) * 0.01;
		if ( length < 0.5 )	// Don't lose all of the noise/texture on short beams
			length = 0.5;
		div = 1.0 / (segments-1);

		// UNDONE: Expose texture length scale factor to control "fuzziness"
		vStep = length*div;	// Texture length texels per space pixel

		// UNDONE: Expose this paramter as well(3.5)?  Texture scroll rate along beam
		vLast = fmod(freq*speed,1);	// Scroll speed 3.5 -- initial texture position, scrolls 3.5/sec (1.0 is entire texture)

		if ( flags & FBEAM_SINENOISE )
		{
			scale = scale * 100;
			length = segments * (1.0/10);
		}
		else
			scale = scale * length;
		
		// -----------------------------------------------------------------------------
		// Iterator to resample noise waveform (it needs to be generated in powers of 2)
		// -----------------------------------------------------------------------------
		noiseStep = (int)((float)(noise_divisions-1) * div * 65536.0f);
		noiseIndex = noiseStep;
		
		if ( flags & FBEAM_SINENOISE )
			noiseIndex = 0;

		brightness = 1.0;
		if ( flags & FBEAM_SHADEIN )
			brightness = 0;

		CBeamSeg seg;
		seg.m_flAlpha = 1;

		VectorScale( color, brightness, scaledColor );
		seg.m_vColor.Init( scaledColor[0], scaledColor[1], scaledColor[2] );
		

		// -------------------------------------------------
		//  Calc start and end widths for this segment
		// -------------------------------------------------
		float startSegWidth = startWidth + (widthStep*j);
		float endSegWidth	= startWidth + (widthStep*(j+1));

		// -------------------------------------------------
		//  Now draw each segment
		// -------------------------------------------------
		float	fBestFraction	= -1;
		float	bestDot			= 0;
		for (int i = 1; i < segments; i++ )
		{
			fraction = i * div;

			// Fade in our out beam to fadeLength
			// BUG BUG: should be based on total lengh of beam not this particular fraction
			if ( flags & FBEAM_SHADEIN )
			{
				brightness = fraction/fadeFraction;
				if (brightness > 1)
				{
					brightness = 1;
				}
			}
			else if ( flags & FBEAM_SHADEOUT )
			{
				float fadeFraction = fadeLength/length;
				brightness = 1.0 - (fraction/fadeFraction);
				if (brightness < 0)
				{
					brightness = 0;
				}
			}

			// -----------------------------------------------------------
			//  Calculate spline position
			// -----------------------------------------------------------
			Vector vTarget(0,0,0);
			
			Catmull_Rom_Spline(pPre, pStart, pEnd, pNext, fraction, vTarget );
			
			seg.m_vPos[0] = vTarget.x;
			seg.m_vPos[1] = vTarget.y;
			seg.m_vPos[2] = vTarget.z;

			// --------------------------------------------------------------
			//  Keep track of segment most facing the player for halo effect
			// --------------------------------------------------------------
			if (pHaloMaterial)
			{
				Vector vBeamDir1;
				VectorSubtract(seg.m_vPos,vLastPoint,vBeamDir1);
				VectorNormalize(vBeamDir1);

				Vector vLookDir;
				VectorSubtract(CurrentViewOrigin(),seg.m_vPos,vLookDir);
				VectorNormalize(vLookDir);

				float	dotpr		= fabs(DotProduct(vBeamDir1,vLookDir));
				static float thresh = 0.85;
				if (dotpr > thresh && dotpr > bestDot)
				{
					bestDot		  = dotpr;
					fBestFraction = fraction;
				}	
				VectorCopy(seg.m_vPos,vLastPoint);
			}


			// ----------------------
			// Distort using noise
			// ----------------------
			if ( scale != 0 )
			{
				factor = prgNoise[noiseIndex>>16] * scale;
				if ( flags & FBEAM_SINENOISE )
				{
					float	s, c;
					SinCos( fraction*M_PI*length + freq, &s, &c );
					VectorMA( seg.m_vPos, factor * s, CurrentViewUp(), seg.m_vPos );
					// Rotate the noise along the perpendicluar axis a bit to keep the bolt from looking diagonal
					VectorMA( seg.m_vPos, factor * c, CurrentViewRight(), seg.m_vPos );
				}
				else
				{
					VectorMA( seg.m_vPos, factor, CurrentViewUp(), seg.m_vPos );
					// Rotate the noise along the perpendicluar axis a bit to keep the bolt from looking diagonal
					factor = prgNoise[noiseIndex>>16] * scale * cos(fraction*M_PI*3+freq);
					VectorMA( seg.m_vPos, factor, CurrentViewRight(), seg.m_vPos );
				}
			}


			// Scale width if non-zero spread
			if (startWidth != endWidth)
				seg.m_flWidth = ((fraction*(endSegWidth-startSegWidth))+startSegWidth)*2;
			else
				seg.m_flWidth = startWidth*2;

			seg.m_flTexCoord = vLast;
			segDraw.NextSeg( &seg );

			vLast += vStep;	// Advance texture scroll (v axis only)
			noiseIndex += noiseStep;
		}


		// --------------------------------------------------------------
		//  Draw halo on segment most facing the player 
		// --------------------------------------------------------------
		if (false&&pHaloMaterial)
		{
			Vector vHaloPos(0,0,0);
			if (bestDot != 0)
			{
				Catmull_Rom_Spline(pPre, pStart, pEnd, pNext, fBestFraction, vHaloPos);
			}
			else 
			{
				Vector vBeamDir1;
				VectorSubtract(pStart,pEnd,vBeamDir1);
				VectorNormalize(vBeamDir1);

				Vector vLookDir;
				VectorSubtract(CurrentViewOrigin(),pStart,vLookDir);
				VectorNormalize(vLookDir);

				bestDot		= fabs(DotProduct(vBeamDir1,vLookDir));
				static float thresh = 0.85;
				if (bestDot > thresh)
				{
					fBestFraction = 0.5;
					VectorAdd(pStart,pEnd,vHaloPos);
					VectorScale(vHaloPos,0.5,vHaloPos);
				}	
			}
			if (fBestFraction > 0)
			{
				float	fade	= pow(bestDot,60);
				if (fade > 1.0) fade = 1.0;
				float haloColor[3];
				VectorScale( color, fade, haloColor );
				materials->Bind(pHaloMaterial);
				float curWidth = (fBestFraction*(endSegWidth-startSegWidth))+startSegWidth;
				DrawHalo(pHaloMaterial,vHaloPos,flHaloScale*curWidth/endWidth,haloColor);
			}
		}
	}
	
	segDraw.End();

	// ------------------------
	// Draw halo at end of beam
	// ------------------------
	if (pHaloMaterial)
	{
		materials->Bind(pHaloMaterial);
		DrawHalo(pHaloMaterial,pEnd,flHaloScale,scaledColor);
	}
}


//-----------------------------------------------------------------------------
// Purpose: 
// Input  : *spritemodel - 
//			frame - 
//			rendermode - 
//			source - 
//			scale - 
//			*color - 
//-----------------------------------------------------------------------------
void BeamDrawHalo( const model_t* spritemodel, float frame, int rendermode, 
				  const Vector& source, float scale, float* color )
{
	CEngineSprite *pSprite = Draw_SetSpriteTexture( spritemodel, frame, rendermode );
	if ( !pSprite )
		return;

	DrawHalo( pSprite->GetMaterial(), source, scale, color );
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : noise_divisions - 
//			*prgNoise - 
//			*spritemodel - 
//			frame - 
//			rendermode - 
//			source - 
//			delta - 
//			width - 
//			scale - 
//			freq - 
//			speed - 
//			segments - 
//			*color - 
//-----------------------------------------------------------------------------
void DrawDisk( int noise_divisions, float *prgNoise, const model_t* spritemodel, 
			  float frame, int rendermode, const Vector&  source, const Vector& delta, 
			  float width, float scale, float freq, float speed, int segments, float* color )
{
	int				i;
	float			div, length, fraction, vLast, vStep;
	Vector			point;
	float			w;

	CEngineSprite *pSprite = Draw_SetSpriteTexture( spritemodel, frame, rendermode );
	if ( !pSprite )
		return;

	if ( segments < 2 )
		return;
	
	if ( segments > noise_divisions )		// UNDONE: Allow more segments?
		segments = noise_divisions;

	length = VectorLength( delta ) * 0.01;
	if ( length < 0.5 )	// Don't lose all of the noise/texture on short beams
		length = 0.5;
	div = 1.0 / (segments-1);

	// UNDONE: Expose texture length scale factor to control "fuzziness"
	vStep = length*div;	// Texture length texels per space pixel
	
	// UNDONE: Expose this paramter as well(3.5)?  Texture scroll rate along beam
	vLast = fmod(freq*speed,1);	// Scroll speed 3.5 -- initial texture position, scrolls 3.5/sec (1.0 is entire texture)
	scale = scale * length;

	w = freq * delta[2];

	IMesh* pMesh = materials->GetDynamicMesh( );

	CMeshBuilder meshBuilder;
	meshBuilder.Begin( pMesh, MATERIAL_TRIANGLE_STRIP, (segments - 1) * 2 );

	// NOTE: We must force the degenerate triangles to be on the edge
	for ( i = 0; i < segments; i++ )
	{
		float	s, c;
		fraction = i * div;

		point[0] = source[0];
		point[1] = source[1];
		point[2] = source[2];

		meshBuilder.Color3fv( color );
		meshBuilder.TexCoord2f( 0, 1.0, vLast );
		meshBuilder.Position3fv( point.Base() );
		meshBuilder.AdvanceVertex();

		SinCos( fraction * 2 * M_PI, &s, &c );
		point[0] = s * w + source[0];
		point[1] = c * w + source[1];
		point[2] = source[2];

		meshBuilder.Color3fv( color );
		meshBuilder.TexCoord2f( 0, 0.0, vLast );
		meshBuilder.Position3fv( point.Base() );
		meshBuilder.AdvanceVertex();

		vLast += vStep;	// Advance texture scroll (v axis only)
	}

	meshBuilder.End( );
	pMesh->Draw();
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : noise_divisions - 
//			*prgNoise - 
//			*spritemodel - 
//			frame - 
//			rendermode - 
//			source - 
//			delta - 
//			width - 
//			scale - 
//			freq - 
//			speed - 
//			segments - 
//			*color - 
//-----------------------------------------------------------------------------
void DrawCylinder( int noise_divisions, float *prgNoise, const model_t* spritemodel,
				  float frame, int rendermode, const Vector&  source, const Vector& delta, 
				  float width, float scale, float freq, float speed, int segments, 
				  float* color )
{
	int				i;
	float			div, length, fraction, vLast, vStep;
	Vector			point;

	CEngineSprite *pSprite = Draw_SetSpriteTexture( spritemodel, frame, rendermode );
	if ( !pSprite )
		return;

	if ( segments < 2 )
		return;
	
	if ( segments > noise_divisions )		// UNDONE: Allow more segments?
		segments = noise_divisions;

	length = VectorLength( delta ) * 0.01;
	if ( length < 0.5 )	// Don't lose all of the noise/texture on short beams
		length = 0.5;
	div = 1.0 / (segments-1);

	// UNDONE: Expose texture length scale factor to control "fuzziness"
	vStep = length*div;	// Texture length texels per space pixel
	
	// UNDONE: Expose this paramter as well(3.5)?  Texture scroll rate along beam
	vLast = fmod(freq*speed,1);	// Scroll speed 3.5 -- initial texture position, scrolls 3.5/sec (1.0 is entire texture)
	scale = scale * length;
	
	IMesh* pMesh = materials->GetDynamicMesh( );

	CMeshBuilder meshBuilder;
	meshBuilder.Begin( pMesh, MATERIAL_TRIANGLE_STRIP, (segments - 1) * 2 );

	float radius = delta[2];
	for ( i = 0; i < segments; i++ )
	{
		float	s, c;
		fraction = i * div;
		SinCos( fraction * 2 * M_PI, &s, &c );

		point[0] = s * freq * radius + source[0];
		point[1] = c * freq * radius + source[1];
		point[2] = source[2] + width;

		meshBuilder.Color3f( 0.0f, 0.0f, 0.0f );
		meshBuilder.TexCoord2f( 0, 1.0f, vLast );
		meshBuilder.Position3fv( point.Base() );
		meshBuilder.AdvanceVertex();

		point[0] = s * freq * (radius + width) + source[0];
		point[1] = c * freq * (radius + width) + source[1];
		point[2] = source[2] - width;

		meshBuilder.Color3fv( color );
		meshBuilder.TexCoord2f( 0, 0.0f, vLast );
		meshBuilder.Position3fv( point.Base() );
		meshBuilder.AdvanceVertex();

		vLast += vStep;	// Advance texture scroll (v axis only)
	}
	
	meshBuilder.End();
	pMesh->Draw();
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : noise_divisions - 
//			*prgNoise - 
//			(*pfnNoise - 
//-----------------------------------------------------------------------------
void DrawRing( int noise_divisions, float *prgNoise, void (*pfnNoise)( float *noise, int divs, float scale ), 
			  const model_t* spritemodel, float frame, int rendermode,
			  const Vector& source, const Vector& delta, float width, 
			  float amplitude, float freq, float speed, int segments, float *color )
{
	int				i, j, noiseIndex, noiseStep;
	float			div, length, fraction, factor, vLast, vStep;
	Vector			last1, last2, point, screen, screenLast(0,0,0), tmp, normal;
	Vector			center, xaxis, yaxis, zaxis;
	float			radius, x, y, scale;
	Vector			d;

	CEngineSprite *pSprite = Draw_SetSpriteTexture( spritemodel, frame, rendermode );
	if ( !pSprite )
		return;

	VectorCopy( delta, d );

	if ( segments < 2 )
		return;

	segments = segments * M_PI;
	
	if ( segments > noise_divisions * 8 )		// UNDONE: Allow more segments?
		segments = noise_divisions * 8;

	length = VectorLength( d ) * 0.01 * M_PI;
	if ( length < 0.5 )	// Don't lose all of the noise/texture on short beams
		length = 0.5;
	div = 1.0 / (segments-1);

	// UNDONE: Expose texture length scale factor to control "fuzziness"
	vStep = length*div/8.0;	// Texture length texels per space pixel
	
	// UNDONE: Expose this paramter as well(3.5)?  Texture scroll rate along beam
	vLast = fmod(freq*speed,1);	// Scroll speed 3.5 -- initial texture position, scrolls 3.5/sec (1.0 is entire texture)
	scale = amplitude * length / 8.0;

	// Iterator to resample noise waveform (it needs to be generated in powers of 2)
	noiseStep = (int)((noise_divisions-1) * div * 65536.0) * 8;
	noiseIndex = 0;

	VectorScale( d, 0.5, d );
	VectorAdd( source, d, center );
	zaxis[0] = 0; zaxis[1] = 0; zaxis[2] = 1;

	VectorCopy( d, xaxis );
	radius = VectorLength( xaxis );
	
	// cull beamring
	// --------------------------------
	// Compute box center +/- radius
	last1[0] = radius;
	last1[1] = radius;
	last1[2] = scale;
	VectorAdd( center, last1, tmp );	// maxs
	VectorSubtract( center, last1, screen ); // mins

	// Is that box in PVS && frustum?
	if ( !engine->IsBoxVisible( screen, tmp ) || engine->CullBox( screen, tmp ) )	
	{
		return;
	}

	yaxis[0] = xaxis[1]; yaxis[1] = -xaxis[0]; yaxis[2] = 0;
	VectorNormalize( yaxis );
	VectorScale( yaxis, radius, yaxis );

	j = segments / 8;

	IMesh* pMesh = materials->GetDynamicMesh( );

	CMeshBuilder meshBuilder;
	meshBuilder.Begin( pMesh, MATERIAL_TRIANGLE_STRIP, (segments) * 2 );

	for ( i = 0; i < segments + 1; i++ )
	{
		fraction = i * div;
		SinCos( fraction * 2 * M_PI, &x, &y );

		point[0] = xaxis[0] * x + yaxis[0] * y + center[0];
		point[1] = xaxis[1] * x + yaxis[1] * y + center[1];
		point[2] = xaxis[2] * x + yaxis[2] * y + center[2];

		// Distort using noise
		factor = prgNoise[(noiseIndex>>16) & 0x7F] * scale;
		VectorMA( point, factor, CurrentViewUp(), point );

		// Rotate the noise along the perpendicluar axis a bit to keep the bolt from looking diagonal
		factor = prgNoise[(noiseIndex>>16) & 0x7F] * scale * cos(fraction*M_PI*3*8+freq);
		VectorMA( point, factor, CurrentViewRight(), point );
		
		// Transform point into screen space
		ScreenTransform( point, screen );

		if (i != 0)
		{
			// Build world-space normal to screen-space direction vector
			VectorSubtract( screen, screenLast, tmp );
			// We don't need Z, we're in screen space
			tmp[2] = 0;
			VectorNormalize( tmp );
			VectorScale( CurrentViewUp(), tmp[0], normal );	// Build point along noraml line (normal is -y, x)
			VectorMA( normal, -tmp[1], CurrentViewRight(), normal );
			
			// Make a wide line
			VectorMA( point, width, normal, last1 );
			VectorMA( point, -width, normal, last2 );

			vLast += vStep;	// Advance texture scroll (v axis only)
			meshBuilder.Color3fv( color );
			meshBuilder.TexCoord2f( 0, 1.0f, vLast );
			meshBuilder.Position3fv( last2.Base() );
			meshBuilder.AdvanceVertex();

			meshBuilder.Color3fv( color );
			meshBuilder.TexCoord2f( 0, 0.0f, vLast );
			meshBuilder.Position3fv( last1.Base() );
			meshBuilder.AdvanceVertex();
		}
		VectorCopy( screen, screenLast );
		noiseIndex += noiseStep;

		j--;
		if (j == 0 && amplitude != 0 )
		{
			j = segments / 8;
			(*pfnNoise)( prgNoise, noise_divisions, 1.0f );
		}
	}

	meshBuilder.End();
	pMesh->Draw();
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : spritemodel - 
//			*pHead - 
//			delta - 
//			*screen - 
//			*screenLast - 
//			die - 
//			source - 
//			flags - 
//			width - 
//			amplitude - 
//			freq - 
//			*color - 
//-----------------------------------------------------------------------------
void DrawBeamFollow( const model_t* spritemodel, BeamTrail_t* pHead, int frame, int rendermode, 
					Vector& delta, Vector& screen, Vector& screenLast, float die,
					const Vector& source, int flags, float width, float amplitude, 
					float freq, float* color )
{
	float			fraction;
	float			div;
	float			vLast = 0.0;
	float			vStep = 1.0;
	Vector			last1, last2, tmp, normal;
	float			scaledColor[3];

	CEngineSprite *pSprite = Draw_SetSpriteTexture( spritemodel, frame, rendermode );
	if ( !pSprite )
		return;

	// UNDONE: This won't work, screen and screenLast must be extrapolated here to fix the
	// first beam segment for this trail

	// Build world-space normal to screen-space direction vector
	VectorSubtract( screen, screenLast, tmp );
	// We don't need Z, we're in screen space
	tmp[2] = 0;
	VectorNormalize( tmp );
	VectorScale( CurrentViewUp(), tmp[0], normal );	// Build point along noraml line (normal is -y, x)
	VectorMA( normal, -tmp[1], CurrentViewRight(), normal );
	
	// Make a wide line
	VectorMA( delta, width, normal, last1 );
	VectorMA( delta, -width, normal, last2 );

	div = 1.0 / amplitude;
	fraction = ( die - gpGlobals->curtime ) * div;
	unsigned char nColor[3];

	VectorScale( color, fraction, scaledColor );
	nColor[0] = (unsigned char)clamp( (int)(scaledColor[0] * 255.0f), 0, 255 );
	nColor[1] = (unsigned char)clamp( (int)(scaledColor[1] * 255.0f), 0, 255 );
	nColor[2] = (unsigned char)clamp( (int)(scaledColor[2] * 255.0f), 0, 255 );
	
	// need to count the segments
	int count = 0;
	BeamTrail_t* pTraverse = pHead;
	while ( pTraverse )
	{
		++count;
		pTraverse = pTraverse->next;
	}

	IMesh* pMesh = materials->GetDynamicMesh( );

	CMeshBuilder meshBuilder;
	meshBuilder.Begin( pMesh, MATERIAL_QUADS, count );

	while (pHead)
	{
		// Msg("%.2f ", fraction );
		meshBuilder.Position3fv( last1.Base() );
		meshBuilder.Color3ubv( nColor );
		meshBuilder.TexCoord2f( 0, 0.0f, 0.0f );
		meshBuilder.AdvanceVertex();

		meshBuilder.Position3fv( last2.Base() );
		meshBuilder.Color3ubv( nColor );
		meshBuilder.TexCoord2f( 0, 1.0f, 0.0f );
		meshBuilder.AdvanceVertex();

		// Transform point into screen space
		ScreenTransform( pHead->org, screen );
		// Build world-space normal to screen-space direction vector
		VectorSubtract( screen, screenLast, tmp );
		// We don't need Z, we're in screen space
		tmp[2] = 0;
		VectorNormalize( tmp );
		VectorScale( CurrentViewUp(), tmp[0], normal );	// Build point along noraml line (normal is -y, x)
		VectorMA( normal, -tmp[1], CurrentViewRight(), normal );
		
		// Make a wide line
		VectorMA( pHead->org, width, normal, last1 );
		VectorMA( pHead->org, -width, normal, last2 );

		vLast += vStep;	// Advance texture scroll (v axis only)

		if (pHead->next != NULL)
		{
			fraction = (pHead->die - gpGlobals->curtime) * div;
			VectorScale( color, fraction, scaledColor );
			nColor[0] = (unsigned char)clamp( (int)(scaledColor[0] * 255.0f), 0, 255 );
			nColor[1] = (unsigned char)clamp( (int)(scaledColor[1] * 255.0f), 0, 255 );
			nColor[2] = (unsigned char)clamp( (int)(scaledColor[2] * 255.0f), 0, 255 );
		}
		else
		{
			fraction = 0.0;
			nColor[0] = nColor[1] = nColor[2] = 0;
		}

		meshBuilder.Position3fv( last2.Base() );
		meshBuilder.Color3ubv( nColor );
		meshBuilder.TexCoord2f( 0, 1.0f, 1.0f );
		meshBuilder.AdvanceVertex();

		meshBuilder.Position3fv( last1.Base() );
		meshBuilder.Color3ubv( nColor );
		meshBuilder.TexCoord2f( 0, 0.0f, 1.0f );
		meshBuilder.AdvanceVertex();

		VectorCopy( screen, screenLast );

		pHead = pHead->next;
	}

	meshBuilder.End();
	pMesh->Draw();
}