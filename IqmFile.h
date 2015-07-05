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
	// Forwards
public:
	enum class IQM_T : uint32_t;
private:
	struct iqmheader;

	// Class members
	// Byte buffer of data
	std::vector<char> m_Data;
	// Header struct
	iqmheader * m_Header;
	// Maps for file ptr offsets and counts
	std::map<IQM_T, uint32_t> ofs, num;

public:
	// Source constructor
	IqmFile(std::string src);

	// Get string from file (his format)
	std::string getStr(uint32_t ofs_str);

	// Protected accessor methods
protected:
	inline uint32_t getNum(IQM_T c) const { return num.at(c); };
	template <typename T = uint8_t>
	inline T * getPtr(IQM_T c) const { return (T *)&m_Data[ofs.at(c)]; }

	// Public accessor class
	friend class IqmAttr;
public:
	template <typename N, IqmFile::IQM_T C, typename T = N>
	class IqmAttr{
		friend class IqmFile;
		const IqmFile& m_File;
	protected:
		IqmAttr(IqmFile& file)
			: m_File(file)
		{
			static_assert(!(sizeof(T) % sizeof(N)) || !(sizeof(N) % sizeof(T)), "IQM Error: In order to create an IqmAttr, the Native Type must divide evenly into sizeof(N) / sizeof(T) units, or vice versa.");
		}
		inline uint32_t ratio() const{
			return (sizeof(T) > sizeof(N) ? sizeof(T) / sizeof(N) : sizeof(N) / sizeof(T));
		}
	public:
		inline uint32_t count() const{
			return ratio() * m_File.getNum(C);
		}
		inline uint32_t numBytes() const{
			return count() * sizeof(T);
		}
		inline size_t size() const{
			return sizeof(T);
		}
		inline size_t nativeSize() const{
			return sizeof(N);
		}
		inline T * ptr() const{
			return m_File.getPtr<T>(C);
		}
		inline std::vector<T> toVec() const{
			std::vector<T> ret;
			T * buf = ptr();
			for (int i = 0; i < count(); i++)
				ret.push_back(buf[i]);
			ret.shrink_to_fit();
			return ret;
		}
	};
	// Get Attr; can return attr of different type than native, provided the strides work out
	template <typename N, IqmFile::IQM_T C, typename T = N>
	inline IqmAttr<N, C, T> getAttr(){
		return IqmAttr<N, C, T>(*this);
	}

public:
	// Various structs used in IQM, left public
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

	// structs and enums needed when loading the file
private:
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

public:
	// I think I can declare these with a template rather than a macro...
#define IQMATTRFNGENMACRO(N,C,fn) template <typename T = N> inline IqmAttr<N, C, T> fn(){ return getAttr<N, C, T>(); }
	// Returns Position Attr
	IQMATTRFNGENMACRO(iqmposition, IQM_T::POSITION, Positions);
	// Returns Tex Coord Attr
	IQMATTRFNGENMACRO(iqmtexcoord, IQM_T::TEXCOORD, TexCoords);
	// Returns Normals Attr
	IQMATTRFNGENMACRO(iqmnormal, IQM_T::NORMAL, Normals);
	// Returns Tangents Attr
	IQMATTRFNGENMACRO(iqmnormal, IQM_T::TANGENT, Tangents);
	// Returns Blend Indices Attr
	IQMATTRFNGENMACRO(iqmnormal, IQM_T::BLENDINDEXES, BlendIndices);
	// Returns Blend Weights Attr
	IQMATTRFNGENMACRO(iqmnormal, IQM_T::BLENDWEIGHTS, BlendWeights);
	// Returns Meshes
	IQMATTRFNGENMACRO(iqmmesh, IQM_T::MESH, Meshes);
	// Returns Triangles (Geometry Indices)
	IQMATTRFNGENMACRO(iqmtriangle, IQM_T::TRI, Triangles);
	// Returns Joints
	IQMATTRFNGENMACRO(iqmjoint, IQM_T::JOINT, Joints);
	// Returns anims
	IQMATTRFNGENMACRO(iqmanim, IQM_T::ANIM, Anims);
	// Returns frames
	IQMATTRFNGENMACRO(uint16_t, IQM_T::FRAME, Frames);
	// Returns triangles as uint32_t rather than iqmtriangle
	auto Indices()->decltype(Triangles<uint32_t>()){ return Triangles<uint32_t>(); }
	// Get # of frames
	inline uint32_t getNumFrames() { return m_Header->num_frames; }
};

