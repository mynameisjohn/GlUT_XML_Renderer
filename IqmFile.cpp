#include "IqmFile.h"

#include <algorithm>
using namespace std;

/*************************************************
	Source Constructor
	Takes in binary file and parses it, storing the
	raw bytes as well as useful byte offsets and
	data type counts within the file.
	**************************************************/
IqmFile::IqmFile( string src )
{
	// Useful lambdas
	auto isLittleEndian = [] ()
	{	// Check if lil endian (not handled yet)
		union { int i; uint8_t b[sizeof( int )]; } conv;
		conv.i = 1;
		return conv.b[0] != 0;
	};
	
	auto IQMASSERT = [] ( bool cond, string msg )
	{	// Assert with message
		if ( cond == false )
		{
			cout << msg << endl;
			exit( -11 );
		}
	};

	// Version/Magic at the time of writing this
	const uint32_t IQM_VERSION = 2;
	const string IQM_MAGIC = "INTERQUAKEMODEL";

	// Read the binary file, make sure we actually get data
	ifstream vIn( src, ios::binary );
	m_Data = vector<char>( istreambuf_iterator<char>( vIn ), istreambuf_iterator<char>() );
	IQMASSERT( m_Data.size() > IQM_MAGIC.size(), "Error: No data loaded" );

	// If !littleEndian, reverse all bytes read in (NOT TESTED)
	if ( isLittleEndian() == false )
		reverse( m_Data.begin(), m_Data.end() );

	// No more data needed, save / grab what we can
	m_Data.shrink_to_fit();
	m_Header = (iqmheader *) ( m_Data.data() );

	// Checks
	IQMASSERT( m_Header != nullptr, "No IQM Header loaded" );
	IQMASSERT( m_Header->version == IQM_VERSION, "IQM file version incorrect" );
	IQMASSERT( string( m_Header->magic ) == IQM_MAGIC, "IQM File contained wrong magic number" );

	// Grab these things
	m_WayPoints[IQM_T::MESH] = { m_Header->num_meshes, m_Header->ofs_meshes };
	m_WayPoints[IQM_T::TRI] = { m_Header->num_meshes, m_Header->ofs_meshes };
	m_WayPoints[IQM_T::JOINT] = { m_Header->num_meshes, m_Header->ofs_meshes };
	m_WayPoints[IQM_T::POSE] = { m_Header->num_meshes, m_Header->ofs_meshes };
	m_WayPoints[IQM_T::ANIM] = { m_Header->num_meshes, m_Header->ofs_meshes };
	m_WayPoints[IQM_T::VARRAY] = { m_Header->num_meshes, m_Header->ofs_meshes };
	m_WayPoints[IQM_T::FRAME] = { m_Header->num_meshes, m_Header->ofs_meshes };

	// Loop through all vertex arrays
	iqmvertexarray * vArrs( (iqmvertexarray *) &m_Data[m_Header->ofs_vertexarrays] );
	for ( uint32_t i = 0; i < m_Header->num_vertexarrays; i++ )
	{	// Check array type, cache info
		iqmvertexarray & va( vArrs[i] );
		switch ( (IQM_T) va.type )
		{
			case IQM_T::POSITION:
			case IQM_T::NORMAL:
			case IQM_T::TANGENT:
			case IQM_T::TEXCOORD:
				IQMASSERT( (IQM_P) va.format == IQM_P::FLOAT, "Error: Type of vertex attribute incorrect, expected a float" );
				break;
			case IQM_T::BLENDINDEXES:
			case IQM_T::BLENDWEIGHTS:
				IQMASSERT( (IQM_P) va.format == IQM_P::UBYTE, "Error: Type of vertex attribute incorrect, expected a byte" );
			default:
				// Is this OK?
				continue;
		}
		m_WayPoints[(IQM_T) va.type] = { m_Header->num_vertexes, va.offset };
	}
}

// Get text string from IQM File
string IqmFile::getStr( uint32_t ofs_str )
{
	return ( ofs_str < m_Header->num_text ) ? string( &m_Data[m_Header->ofs_text + ofs_str] ) : string();
}

// Return waypoint from map, if it exists
IqmFile::IqmWaypoint IqmFile::getWayPoint( IQM_T c ) const
{
	auto it = m_WayPoints.find( c );
	if ( it == m_WayPoints.end() )
	{
		cout << "Unable to find IQM type with code " << c << endl;
		return{ 0, 0 };
	}
	return it->second;
}

// Get number of IQM_T mapped types
uint32_t IqmFile::getNum( IQM_T c ) const
{
	IqmWaypoint wp = getWayPoint( c );
	return wp.num;
}

// Get ptr to IQM_T mapped types
template <typename T>
T * IqmFile::getPtr( IQM_T c ) const
{
	IqmWaypoint wp = getWaypoint( c );
	return wp.ofs ? (T *) &m_Data[wp.ofs] : nullptr;
}