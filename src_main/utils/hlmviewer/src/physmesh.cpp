#include "cmdlib.h"
#include "vphysics/constraints.h"
#include "phyfile.h"
#include "physdll.h"
#include "physmesh.h"
#include "mathlib.h"
#include <stddef.h>
#include "utlvector.h"
#include "commonmacros.h"
#include "studiomodel.h"
#include "vstdlib/strtools.h"
#include "bone_setup.h"

int FindPhysprop( const char *pPropname );

IPhysicsSurfaceProps *physprop = NULL;
IPhysicsCollision *physcollision = NULL;
bool LoadPhysicsProperties( void );
extern int FindBoneIndex( studiohdr_t *pstudiohdr, const char *pName );

#include "vcollide_parse.h"

class CStudioPhysics : public IStudioPhysics
{
public:
	CStudioPhysics( void )
	{
		m_pList = NULL;
		m_listCount = 0;
		m_mass = 0;
		memset( &m_edit, 0, sizeof(editparams_t) );
	}

	~CStudioPhysics( void ) 
	{
		if ( physcollision )
		{
			for ( int i = 0; i < m_listCount; i++ )
			{
				physcollision->DestroyDebugMesh( m_pList[i].m_vertCount, m_pList[i].m_pVerts );
				physcollision->DestroyQueryModel( m_pList[i].m_pCollisionModel );
			}
			physcollision->VCollideUnload( &m_vcollide );
		}
		delete[] m_pList;
	}

	int	Count( void )
	{
		return m_listCount;
	}

	CPhysmesh *GetMesh( int index )
	{
		if ( index < m_listCount )
			return m_pList + index;

		return NULL;
	}

	float	GetMass( void ) { return m_mass; }

	void	Load( studiohdr_t *pstudiohdr, const char *pFilename );
	char	*DumpQC( void );
	void	ParseKeydata( void );


	CPhysmesh		*m_pList;
	vcollide_t		m_vcollide;
	int				m_listCount;

	float			m_mass;
	editparams_t	m_edit;
};


void CPhysmesh::Clear( void )
{
	memset( this, 0, sizeof(*this) );
	memset( &m_constraint, 0, sizeof(m_constraint) );
	m_constraint.parentIndex = -1;
	m_constraint.childIndex = -1;
}


IStudioPhysics *LoadPhysics( studiohdr_t *pstudiohdr, const char *pFilename )
{
	CStudioPhysics *pPhysics = new CStudioPhysics;

	pPhysics->Load( pstudiohdr, pFilename );
	return pPhysics;
}

void CStudioPhysics::Load( studiohdr_t *pstudiohdr, const char *pFilename )
{
	char phyName[512];
	SetExtension( phyName, pFilename, ".PHY" );
	FILE *fp = fopen( phyName, "rb" );

	if ( !fp )
		return;

	CmdLib_InitFileSystem( pFilename, true );
	char dirname[512];
	strcpy( dirname, basegamedir );
	StripLastDir( dirname );

	strcat( dirname, "VPHYSICS.DLL" );
	PhysicsDLLPath( dirname );
	CreateInterfaceFn physicsFactory = GetPhysicsFactory();
	if ( physicsFactory )
	{
		physcollision = (IPhysicsCollision *)physicsFactory( VPHYSICS_COLLISION_INTERFACE_VERSION, NULL );
	}
	if ( !physcollision )
		return;

	LoadPhysicsProperties();

	phyheader_t header;
	
	fread( &header, sizeof(header), 1, fp );
	if ( header.size != sizeof(header) || header.solidCount <= 0 )
		return;

	int pos = ftell( fp );
	fseek( fp, 0, SEEK_END );
	int fileSize = ftell(fp) - pos;
	fseek( fp, pos, SEEK_SET );

	char *buf = (char *)_alloca( fileSize );
	fread( buf, fileSize, 1, fp );
	fclose( fp );

	physcollision->VCollideLoad( &m_vcollide, header.solidCount, (const char *)buf, fileSize );

	m_pList = new CPhysmesh[header.solidCount];
	m_listCount = header.solidCount;

	int i;

	for ( i = 0; i < header.solidCount; i++ )
	{
		m_pList[i].Clear();
		m_pList[i].m_vertCount = physcollision->CreateDebugMesh( m_vcollide.solids[i], &m_pList[i].m_pVerts );
		m_pList[i].m_pCollisionModel = physcollision->CreateQueryModel( m_vcollide.solids[i] );
	}

	ParseKeydata();

	for ( i = 0; i < header.solidCount; i++ )
	{
		CPhysmesh *pmesh = m_pList + i;
		int boneIndex = FindBoneIndex( pstudiohdr, pmesh->m_boneName );
		if ( pmesh->m_constraint.parentIndex >= 0 )
		{
			CPhysmesh *pparent = m_pList + pmesh->m_constraint.parentIndex;
			int parentIndex = FindBoneIndex( pstudiohdr, pparent->m_boneName );
			Studio_CalcBoneToBoneTransform( pstudiohdr, boneIndex, parentIndex, pmesh->m_matrix );
		}
		else
		{
			MatrixInvert( pstudiohdr->pBone(boneIndex)->poseToBone, pmesh->m_matrix );
		}
	}

	// doesn't have a root bone?  Make it the first bone
	if ( !m_edit.rootName[0] )
	{
		strcpy( m_edit.rootName, m_pList[0].m_boneName );
	}
}


class CEditParse : public IVPhysicsKeyHandler
{
public:
	virtual void ParseKeyValue( void *pCustom, const char *pKey, const char *pValue )
	{
		editparams_t *pEdit = (editparams_t *)pCustom;
		if ( !strcmpi( pKey, "rootname" ) )
		{
			strncpy( pEdit->rootName, pValue, sizeof(pEdit->rootName) );
		}
		else if ( !strcmpi( pKey, "totalmass" ) )
		{
			pEdit->totalMass = atof( pValue );
		}
		else if ( !strcmpi( pKey, "concave" ) )
		{
			pEdit->concave = atoi( pValue );
		}
		else if ( !strcmpi( pKey, "jointmerge" ) )
		{
			char tmp[1024];
			char parentName[512], childName[512];
			Q_strncpy( tmp, pValue, 1024 );
			char *pWord = strtok( tmp, "," );
			Q_strncpy( parentName, pWord, sizeof(parentName) );
			pWord = strtok( NULL, "," );
			Q_strncpy( childName, pWord, sizeof(childName) );
			if ( pEdit->mergeCount < ARRAYSIZE(pEdit->mergeList) )
			{
				merge_t *pMerge = &pEdit->mergeList[pEdit->mergeCount];
				pEdit->mergeCount++;
				pMerge->parent = g_pStudioModel->FindBone(parentName);
				pMerge->child = g_pStudioModel->FindBone(childName);
			}
		}
	}
	virtual void SetDefaults( void *pCustom )
	{
		editparams_t *pEdit = (editparams_t *)pCustom;
		memset( pEdit, 0, sizeof(*pEdit) );
	}
};

class CSolidParse : public IVPhysicsKeyHandler
{
public:
	virtual void ParseKeyValue( void *pCustom, const char *pKey, const char *pValue )
	{
		hlmvsolid_t *pSolid = (hlmvsolid_t *)pCustom;
		if ( !strcmpi( pKey, "massbias" ) )
		{
			pSolid->massBias = atof( pValue );
		}
		else
		{
			printf("Bad key %s!!\n");
		}
	}
	virtual void SetDefaults( void *pCustom )
	{
		hlmvsolid_t *pSolid = (hlmvsolid_t *)pCustom;
		pSolid->massBias = 1.0;
	}
};

void CStudioPhysics::ParseKeydata( void )
{
	IVPhysicsKeyParser *pParser = physcollision->VPhysicsKeyParserCreate( m_vcollide.pKeyValues );

	while ( !pParser->Finished() )
	{
		const char *pBlock = pParser->GetCurrentBlockName();
		if ( !stricmp( pBlock, "solid" ) )
		{
			hlmvsolid_t solid;
			CSolidParse solidParse;

			pParser->ParseSolid( &solid, &solidParse );
			solid.surfacePropIndex = FindPhysprop( solid.surfaceprop );

			if ( solid.index >= 0 && solid.index < m_listCount )
			{
				strcpy( m_pList[solid.index].m_boneName, solid.name );
				memcpy( &m_pList[solid.index].m_solid, &solid, sizeof(solid) );
			}
		}
		else if ( !stricmp( pBlock, "ragdollconstraint" ) )
		{
			constraint_ragdollparams_t constraint;
			pParser->ParseRagdollConstraint( &constraint, NULL );
			if ( constraint.childIndex >= 0 && constraint.childIndex < m_listCount )
			{
				m_pList[constraint.childIndex].m_constraint = constraint;
			}
		}
		else if ( !stricmp( pBlock, "editparams" ) )
		{
			CEditParse editParse;
			pParser->ParseCustom( &m_edit, &editParse );
			m_mass = m_edit.totalMass;
		}
		else
		{
			pParser->SkipBlock();
		}
	}
	physcollision->VPhysicsKeyParserDestroy( pParser );
}


int FindPhysprop( const char *pPropname )
{
	if ( physprop )
	{
		int count = physprop->SurfacePropCount();
		for ( int i = 0; i < count; i++ )
		{
			if ( !strcmpi( pPropname, physprop->GetPropName(i) ) )
				return i;
		}
	}
	return 0;
}



class CTextBuffer
{
public:
	CTextBuffer( void ) {}
	~CTextBuffer( void ) {}

	inline int GetSize( void ) { return m_buffer.Size(); }
	inline char *GetData( void ) { return m_buffer.Base(); }
	
	void WriteText( const char *pText )
	{
		int len = strlen( pText );
		CopyData( pText, len );
	}

	void Terminate( void ) { CopyData( "\0", 1 ); }

	void CopyData( const char *pData, int len )
	{
		int offset = m_buffer.AddMultipleToTail( len );
		memcpy( m_buffer.Base() + offset, pData, len );
	}

private:
	CUtlVector<char> m_buffer;
};


struct physdefaults_t
{
	int   surfacePropIndex;
	float inertia;
	float damping;
	float rotdamping;
};

//-----------------------------------------------------------------------------
// Purpose: Nasty little routine (that was easy to code) to find the most common
//			value in an array of structs containing that as a member
// Input  : *pStructArray - pointer to head of struct array
//			arrayCount - number of elements in the array
//			structSize - size of each element
//			fieldOffset - offset to the float we're finding
// Output : static T - most common value
//-----------------------------------------------------------------------------
template< class T >
static T FindCommonValue( void *pStructArray, int arrayCount, int structSize, int fieldOffset )
{
	int maxCount = 0;
	T maxVal = 0;

	// BUGBUG: This is O(n^2), but n is really small
	for ( int i = 0; i < arrayCount; i++ )
	{
		// current = struct[i].offset
		T current = *(T *)((char *)pStructArray + (i*structSize) + fieldOffset);
		int currentCount = 0;

		// if everything is set to the default, this is almost O(n)
		if ( current == maxVal )
			continue;

		for ( int j = 0; j < arrayCount; j++ )
		{
			// value = struct[j].offset
			T value = *(T *)((char *)pStructArray + (j*structSize) + fieldOffset);
			if ( value == current )
				currentCount++;
		}

		if ( currentCount > maxCount )
		{
			maxVal = current;
			maxCount = currentCount;
		}
	}

	return maxVal;
}

static void CalcDefaultProperties( CPhysmesh *pList, int listCount, physdefaults_t &defs )
{
	defs.surfacePropIndex = FindCommonValue<int>( pList, listCount, sizeof(CPhysmesh), offsetof(CPhysmesh, m_solid.surfacePropIndex) );
	defs.inertia = FindCommonValue<float>( pList, listCount, sizeof(CPhysmesh), offsetof(CPhysmesh, m_solid.params.inertia) );
	defs.damping = FindCommonValue<float>( pList, listCount, sizeof(CPhysmesh), offsetof(CPhysmesh, m_solid.params.damping) );
	defs.rotdamping = FindCommonValue<float>( pList, listCount, sizeof(CPhysmesh), offsetof(CPhysmesh, m_solid.params.rotdamping) );
}

static void DumpModelProperties( CTextBuffer &out, float mass, physdefaults_t &defs )
{
	char tmpbuf[1024];
	sprintf( tmpbuf, "\t$mass %.1f\r\n", mass );
	out.WriteText( tmpbuf );
	sprintf( tmpbuf, "\t$inertia %.2f\r\n", defs.inertia );
	out.WriteText( tmpbuf );
	sprintf( tmpbuf, "\t$damping %.2f\r\n", defs.damping );
	out.WriteText( tmpbuf );
	sprintf( tmpbuf, "\t$rotdamping %.2f\r\n", defs.rotdamping );
	out.WriteText( tmpbuf );
}

char *CStudioPhysics::DumpQC( void )
{
	if ( !m_listCount )
		return NULL;

	CTextBuffer out;
	physdefaults_t defs;

	CalcDefaultProperties( m_pList, m_listCount, defs );

	if ( m_listCount == 1 )
	{
		out.WriteText( "$collisionmodel ragdoll {\r\n\r\n" );
		if ( m_edit.concave )
		{
			out.WriteText( "\t$concave\r\n" );
		}
		DumpModelProperties( out, m_mass, defs );
	}
	else
	{
		int i;

		out.WriteText( "$collisionjoints ragdoll {\r\n\r\n" );
		DumpModelProperties( out, m_mass, defs );

		// write out the root bone
		if ( m_edit.rootName[0] )
		{
			char tmp[128];
			sprintf( tmp, "\t$rootbone \"%s\"\r\n", m_edit.rootName );
			out.WriteText( tmp );
		}

		for ( i = 0; i < m_edit.mergeCount; i++ )
		{
			char tmp[1024];
			char const *pParentName = g_pStudioModel->getStudioHeader()->pBone(m_edit.mergeList[i].parent)->pszName();
			char const *pChildName = g_pStudioModel->getStudioHeader()->pBone(m_edit.mergeList[i].child)->pszName();
			Q_snprintf( tmp, sizeof(tmp), "\t$jointmerge \"%s\" \"%s\"\r\n", pParentName, pChildName );
			out.WriteText( tmp );
		}
		char tmpbuf[1024];
		for ( i = 0; i < m_listCount; i++ )
		{
			CPhysmesh *pmesh = m_pList + i;
			char jointname[256];
			sprintf( jointname, "\"%s\"", pmesh->m_boneName );
			if ( pmesh->m_solid.massBias != 1.0 )
			{
				sprintf( tmpbuf, "\t$jointmassbias %s %.2f\r\n", jointname, pmesh->m_solid.massBias );
				out.WriteText( tmpbuf );
			}
			if ( pmesh->m_solid.params.inertia != defs.inertia )
			{
				sprintf( tmpbuf, "\t$jointinertia %s %.2f\r\n", jointname, pmesh->m_solid.params.inertia );
				out.WriteText( tmpbuf );
			}
			if ( pmesh->m_solid.params.damping != defs.damping )
			{
				sprintf( tmpbuf, "\t$jointdamping %s %.2f\r\n", jointname, pmesh->m_solid.params.damping );
				out.WriteText( tmpbuf );
			}
			if ( pmesh->m_solid.params.rotdamping != defs.rotdamping )
			{
				sprintf( tmpbuf, "\t$jointrotdamping %s %.2f\r\n", jointname, pmesh->m_solid.params.rotdamping );
				out.WriteText( tmpbuf );
			}

			if ( pmesh->m_constraint.parentIndex >= 0 )
			{
				for ( int j = 0; j < 3; j++ )
				{
					char *pAxis[] = { "x", "y", "z" };
					sprintf( tmpbuf, "\t$jointconstrain %s %s limit %.2f %.2f %.2f\r\n", jointname, pAxis[j], pmesh->m_constraint.axes[j].minRotation, pmesh->m_constraint.axes[j].maxRotation, pmesh->m_constraint.axes[j].torque );
					out.WriteText( tmpbuf );
				}
			}
			if ( i != m_listCount-1 )
				out.WriteText( "\r\n" );
		}
	}

	out.WriteText( "}\r\n" );
	
	// only need the pose for ragdolls
	if ( m_listCount != 1 )
	{
		out.WriteText( "$sequence ragdoll \t\t\"ragdoll_pose\" \t\tFPS 30 \t\tactivity ACT_DIERAGDOLL 1\r\n" );
	}

	out.Terminate();

	if ( out.GetSize() )
	{
		char *pOutput = new char[out.GetSize()];
		memcpy( pOutput, out.GetData(), out.GetSize() );
		return pOutput;
	}

	return NULL;
}

static const char *pMaterialFilename = "scripts/surfaceproperties.txt";

bool LoadPhysicsProperties( void )
{
	CreateInterfaceFn physicsFactory = GetPhysicsFactory();

	if ( physicsFactory )
		physprop = (IPhysicsSurfaceProps *)physicsFactory( VPHYSICS_SURFACEPROPS_INTERFACE_VERSION, NULL );

	if ( !physprop )
		return false;

	// already loaded
	if ( physprop->SurfacePropCount() )
		return false;

	char buf[1024];
	strcpy( buf, gamedir );
	strcat( buf, pMaterialFilename );
	FILE *fp = fopen( buf, "rb" );

	if ( !fp )
	{
		// try base game dir
		strcpy( buf, basegamedir );
		strcat( buf, pMaterialFilename );
		fp = fopen( buf, "rb" );
		if ( !fp )
			return false;
	}

	fseek( fp, 0, SEEK_END );

	int len = ftell(fp);
	fseek( fp, 0, SEEK_SET );

	char *pText = new char[len+1];
	fread( pText, len, 1, fp );
	fclose( fp );
	pText[len]=0;

	physprop->ParseSurfaceData( pMaterialFilename, pText );

	delete[] pText;
	return true;
}
