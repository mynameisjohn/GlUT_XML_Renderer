#include "IqmFile.h"

using namespace std;

IqmFile::IqmFile(string src){
	// Useful lambdas
	// Check if lil endian (not handled yet)
//	auto isLittleEndian = []() {
//		union { int i; uint8_t b[sizeof(int)]; } conv;
//		conv.i = 1;
//		return conv.b[0] != 0;
//	};
	// Insert into the map
	auto add = [&](IQM_T c, uint32_t n, uint32_t o){
		num[c] = n;
		ofs[c] = o;
	};
	auto IQMASSERT = [](bool cond, string msg){
		if (cond == false){
			cout << msg << endl;
			exit(-11);
		}
	};

	const uint32_t IQM_VERSION = 2;
	const string IQM_MAGIC = "INTERQUAKEMODEL";

	// Read the binary file, make sure we actually get data
	ifstream vIn(src, ios::binary);
	m_Data = vector<char>(istreambuf_iterator<char>(vIn), istreambuf_iterator<char>());
	IQMASSERT(m_Data.size() > IQM_MAGIC.size(), "Error: No data loaded");

	// No more data needed, save / grab what we can
	m_Data.shrink_to_fit();
	m_Header = (iqmheader *)(m_Data.data());

	// Checks
	IQMASSERT(m_Header != nullptr, "No IQM Header loaded");
	IQMASSERT(m_Header->version == IQM_VERSION, "IQM file version incorrect");
	IQMASSERT(string(m_Header->magic) == IQM_MAGIC, "IQM File contained wrong magic number");

	// Grab these things
	add(IQM_T::MESH, m_Header->num_meshes, m_Header->ofs_meshes);
	add(IQM_T::TRI, m_Header->num_triangles, m_Header->ofs_triangles);
	add(IQM_T::JOINT, m_Header->num_joints, m_Header->ofs_joints);
	add(IQM_T::POSE, m_Header->num_poses, m_Header->ofs_poses);
	add(IQM_T::ANIM, m_Header->num_anims, m_Header->ofs_anims);
	add(IQM_T::VARRAY, m_Header->num_vertexarrays, m_Header->ofs_vertexarrays);
	add(IQM_T::FRAME, m_Header->num_frames*m_Header->num_framechannels, m_Header->ofs_frames);

	// Loop through all vertex arrays
	iqmvertexarray * vArrs((iqmvertexarray *)&m_Data[m_Header->ofs_vertexarrays]);
	for (uint32_t i = 0; i < m_Header->num_vertexarrays; i++){
		iqmvertexarray & va(vArrs[i]);
		switch ((IQM_T)va.type){
		case IQM_T::POSITION:
			IQMASSERT((IQMPRIM)va.format == IQMPRIM::IQM_FLOAT, "Error: Type of vertex attribute incorrect, expected a float");
			break;
		case IQM_T::NORMAL:
			IQMASSERT((IQMPRIM)va.format == IQMPRIM::IQM_FLOAT, "Error: Type of vertex attribute incorrect, expected a float");
			break;
		case IQM_T::TANGENT:
			IQMASSERT((IQMPRIM)va.format == IQMPRIM::IQM_FLOAT, "Error: Type of vertex attribute incorrect, expected a float");
			break;
		case IQM_T::TEXCOORD:
			IQMASSERT((IQMPRIM)va.format == IQMPRIM::IQM_FLOAT, "Error: Type of vertex attribute incorrect, expected a float");
			break;
		case IQM_T::BLENDINDEXES:
			IQMASSERT((IQMPRIM)va.format == IQMPRIM::IQM_UBYTE, "Error: Type of vertex attribute incorrect, expected a byte");
			break;
		case IQM_T::BLENDWEIGHTS:
			IQMASSERT((IQMPRIM)va.format == IQMPRIM::IQM_UBYTE, "Error: Type of vertex attribute incorrect, expected a byte");
		default:
			break;
			
		}
		add((IQM_T)va.type, m_Header->num_vertexes, va.offset);
	}
}

string IqmFile::getStr(uint32_t ofs_str){
	return (ofs_str < m_Header->num_text) ? string(&m_Data[m_Header->ofs_text + ofs_str]) : string();
}
