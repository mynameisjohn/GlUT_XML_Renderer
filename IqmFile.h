#pragma once

/**********************************************
	My crazy object oriented take on IQM
	Loads in a file, gives accessors via IqmData
	Designed to be easy to use with OpenGL
***********************************************/

// Maybe move this to a cpp file...  inlining doesn't seem worth it
#include <string>
#include <map>
#include <vector>
#include <iostream>
#include <stdint.h>
#include <fstream>
#include <stdlib.h>

class IqmFile
{
	// Assert with message
	static void IQMASSERT(bool cond, std::string msg){
		if (cond == false){
			std::cout << msg << std::endl;
			exit(1);
		}
	}

public:
	// Accessor class
	template <class T = uint8_t>
	//TODO: Make iqmdata iterable somehow 
	class iqmdata{
		friend class IqmFile;
		T * m_Ptr;               // Pointer to data
		uint32_t m_Num;          // Number of elements in file
		uint32_t m_NativeSize;   // Native size of data type
	protected:
		//We've already asserted that sizeof(T)%nativesize == 0, hopefully
		iqmdata(T * ptr, uint32_t num, uint32_t sze)
			: m_Ptr(ptr), m_Num(num), m_NativeSize(sze)
		{
			IQMASSERT(ptr && num && sze && (sze*num) % sizeof(T) == 0, "Error creating IQM Attr");
			if (sizeof(T) != m_NativeSize)
				m_Num = (sze*num) / sizeof(T);
		}
	public:
		inline T * ptr() const { return m_Ptr; }
		inline uint32_t count() const { return m_Num; }
		inline uint32_t numBytes() const { return m_Num*sizeof(T); }
		inline uint32_t nativeSize() const { return m_NativeSize; }
		inline uint32_t numElems() const { return m_NativeSize >= size() ? m_NativeSize / size() : 0; }
		inline T & operator [](uint32_t idx) const { return m_Ptr[idx]; }
		inline std::vector<T> toVec() const{
			std::vector<T> ret;
			for (int i = 0; i < count(); i++)
				ret.push_back(m_Ptr[i]);
			ret.shrink_to_fit();
			return ret;
		}
		//I'm so cool
		inline auto size() const -> decltype(sizeof(T)){ return sizeof(T); }
	};

	// All of these structs are his; the binary file consists of these
	struct iqmmesh
	{
		unsigned int name;
		unsigned int material;
		unsigned int first_vertex, num_vertexes;
		unsigned int first_triangle, num_triangles;
	};

	struct iqmtriangle
	{
		uint32_t a, b, c;
	};

	struct iqmjoint
	{
		unsigned int name;
		int parent;
		float translate[3], rotate[4], scale[3];
	};

	struct iqmpose
	{
		int parent;
		unsigned int mask;
		float channeloffset[10];
		float channelscale[10];
	};

	struct iqmanim
	{
		unsigned int name;
		unsigned int first_frame, num_frames;
		float framerate;
		unsigned int flags;
	};


	struct iqmbounds
	{
		float bbmin[3], bbmax[3];
		float xyradius, radius;
	};

	struct iqmposition{
		float x, y, z;
	};

	struct iqmtexcoord{
		float u, v;
	};

	struct iqmnormal{
		float nX, nY, nZ;
	};

	struct iqmtangent{
		float tX, tY, tZ, tW;
	};

	struct iqmblendidx{
		uint8_t bI[4];
	};

	struct iqmblendweight{
		uint8_t bW[4];
	};

	// I turned these into enums
	enum class IQM_T : uint32_t
	{
		POSITION = 0,
		TEXCOORD = 1,
		NORMAL = 2,
		TANGENT = 3,
		BLENDINDEXES = 4,
		BLENDWEIGHTS = 5,
		COLOR = 6,
		VARRAY = 7,
		MESH = 8,
		TRI = 9,
		JOINT = 10,
		POSE = 11,
		ANIM = 12,
		FRAME = 13,
		CUSTOM = 0x10
	};

private:
	// Same with these
	enum class IQMPRIM : uint32_t
	{
		IQM_BYTE = 0,
		IQM_UBYTE = 1,
		IQM_SHORT = 2,
		IQM_USHORT = 3,
		IQM_INT = 4,
		IQM_UINT = 5,
		IQM_HALF = 6,
		IQM_FLOAT = 7,
		IQM_DOUBLE = 8
	};

	// Header struct
	struct iqmheader
	{
		char magic[16];
		uint32_t version;
		uint32_t filesize;
		uint32_t flags;
		uint32_t num_text, ofs_text;
		uint32_t num_meshes, ofs_meshes;
		uint32_t num_vertexarrays, num_vertexes, ofs_vertexarrays;
		uint32_t num_triangles, ofs_triangles, ofs_adjacency;
		uint32_t num_joints, ofs_joints;
		uint32_t num_poses, ofs_poses;
		uint32_t num_anims, ofs_anims;
		uint32_t num_frames, num_framechannels, ofs_frames, ofs_bounds;
		uint32_t num_comment, ofs_comment;
		uint32_t num_extensions, ofs_extensions;
	};

	// Handle to a vertex array in the file
	struct iqmvertexarray
	{
		uint32_t type;
		uint32_t flags;
		uint32_t format;
		uint32_t size;
		uint32_t offset;
	};

	// Byte buffer of data
	std::vector<char> m_Data;
	// Header struct
	iqmheader * m_Header;
	// Maps for file offsets, counts, and native sizes
	std::map<IQM_T, uint32_t> ofs, num;
	std::map<IQM_T, size_t> sze;

public:
	IqmFile(std::string src) {
		const uint32_t IQM_VERSION = 2;
		const std::string IQM_MAGIC = "INTERQUAKEMODEL";

		// Read the binary file, make sure we actually get data
		std::ifstream vIn(src, std::ios::binary);
		m_Data = std::vector<char>(std::istreambuf_iterator<char>(vIn), std::istreambuf_iterator<char>());
		IQMASSERT(m_Data.size() > IQM_MAGIC.size(), "Error: No data loaded");

		// No more data needed, save / grab what we can
		m_Data.shrink_to_fit();
		m_Header = (iqmheader *)(m_Data.data());

		// Checks
		IQMASSERT(m_Header != nullptr, "No IQM Header loaded");
		IQMASSERT(m_Header->version == IQM_VERSION, "IQM file version incorrect");
		IQMASSERT(std::string(m_Header->magic) == IQM_MAGIC, "IQM File contained wrong magic number");

		// Check if lil endian (not handled yet)
		auto isLittleEndian = []() {
			union { int i; uint8_t b[sizeof(int)]; } conv;
			conv.i = 1;
			return conv.b[0] != 0;
		};

		//What do I do with this?
		bool littleEndian(isLittleEndian());

		// Insert into the map
		auto add = [&](IQM_T c, uint32_t n, uint32_t o, uint32_t s){
			num[c] = n;
			ofs[c] = o;
			sze[c] = s;
		};

		// Grab these things
		add(IQM_T::MESH, m_Header->num_meshes, m_Header->ofs_meshes, sizeof(iqmmesh));
		add(IQM_T::TRI, m_Header->num_triangles, m_Header->ofs_triangles, sizeof(iqmtriangle));
		add(IQM_T::JOINT, m_Header->num_joints, m_Header->ofs_joints, sizeof(iqmjoint));
		add(IQM_T::POSE, m_Header->num_poses, m_Header->ofs_poses, sizeof(iqmpose));
		add(IQM_T::ANIM, m_Header->num_anims, m_Header->ofs_anims, sizeof(iqmanim));
		add(IQM_T::VARRAY, m_Header->num_vertexarrays, m_Header->ofs_vertexarrays, sizeof(iqmvertexarray));
		add(IQM_T::FRAME, m_Header->num_frames*m_Header->num_framechannels, m_Header->ofs_frames, sizeof(uint16_t));

		// Loop through all vertex arrays
		iqmvertexarray * vArrs((iqmvertexarray *)&m_Data[m_Header->ofs_vertexarrays]);
		for (uint32_t i = 0; i < m_Header->num_vertexarrays; i++){
			iqmvertexarray & va(vArrs[i]);
			uint32_t size;
			switch ((IQM_T)va.type){
			case IQM_T::POSITION:
				IQMASSERT((IQMPRIM)va.format == IQMPRIM::IQM_FLOAT, "Error: Type of vertex attribute incorrect, expected a float");
				size = sizeof(iqmposition);
				break;
			case IQM_T::NORMAL:
				IQMASSERT((IQMPRIM)va.format == IQMPRIM::IQM_FLOAT, "Error: Type of vertex attribute incorrect, expected a float");
				size = sizeof(iqmnormal);
				break;
			case IQM_T::TANGENT:
				IQMASSERT((IQMPRIM)va.format == IQMPRIM::IQM_FLOAT, "Error: Type of vertex attribute incorrect, expected a float");
				size = sizeof(iqmtangent);
				break;
			case IQM_T::TEXCOORD:
				IQMASSERT((IQMPRIM)va.format == IQMPRIM::IQM_FLOAT, "Error: Type of vertex attribute incorrect, expected a float");
				size = sizeof(iqmtexcoord);
				break;
			case IQM_T::BLENDINDEXES:
				IQMASSERT((IQMPRIM)va.format == IQMPRIM::IQM_UBYTE, "Error: Type of vertex attribute incorrect, expected a byte");
				size = sizeof(iqmblendidx);
				break;
			case IQM_T::BLENDWEIGHTS:
				IQMASSERT((IQMPRIM)va.format == IQMPRIM::IQM_UBYTE, "Error: Type of vertex attribute incorrect, expected a byte");
				size = sizeof(iqmblendweight);
				break;
			}
			add((IQM_T)va.type, m_Header->num_vertexes, va.offset, size);
		}
	}

	// Get string from file (his format)
	inline std::string getStr(uint32_t ofs_str){
		return (ofs_str < m_Header->num_text) ? std::string(&m_Data[m_Header->ofs_text + ofs_str]) : std::string();
	}

	// Get Attr; can return attr of different type than native, provided the strides work out
	template <typename T = uint8_t>
	inline iqmdata<T> getAttr(IQM_T code){
		// TODO: static assert
		IQMASSERT((sze[code] * num[code]) % sizeof(T) == 0 && m_Header,
			"Error: Type requested in getAttr does not evenly span the range of data.");
		return iqmdata<T>((T *)(&m_Data[ofs.at(code)]), num.at(code), sze.at(code));
	}

	// Useful getters
	inline iqmdata<iqmposition> Positions(){ return getAttr<iqmposition>(IQM_T::POSITION); }
	inline iqmdata<iqmtexcoord> TexCoords(){ return getAttr<iqmtexcoord>(IQM_T::TEXCOORD); }
	inline iqmdata<iqmnormal> Normals(){ return getAttr<iqmnormal>(IQM_T::NORMAL); }
	inline iqmdata<iqmtangent> Tangents(){ return getAttr<iqmtangent>(IQM_T::TANGENT); }
	inline iqmdata<iqmblendidx> BlendIndices(){ return getAttr<iqmblendidx>(IQM_T::BLENDINDEXES); }
	inline iqmdata<iqmblendweight> BlendWeights(){ return getAttr<iqmblendweight>(IQM_T::BLENDWEIGHTS); }
	inline iqmdata<iqmmesh> Meshes(){ return getAttr<iqmmesh>(IQM_T::MESH); }
	inline iqmdata<uint32_t> Indices(){ return getAttr<uint32_t>(IQM_T::TRI); }
	inline iqmdata<iqmtriangle> Triangles(){ return getAttr<iqmtriangle>(IQM_T::TRI); }
	inline iqmdata<iqmjoint> Joints(){ return getAttr<iqmjoint>(IQM_T::JOINT); }
	inline iqmdata<iqmpose> Poses(){ return getAttr<iqmpose>(IQM_T::POSE); }
	inline iqmdata<iqmanim> Anims(){ return getAttr<iqmanim>(IQM_T::ANIM); }
	inline iqmdata<uint16_t> Frames(){ return getAttr<uint16_t>(IQM_T::FRAME); }
	inline uint32_t getNumFrames() { return m_Header->num_frames; }
};

