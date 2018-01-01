#ifndef HEADER_EFFECT_DATABASE_H
#define HEADER_EFFECT_DATABASE_H

#include "stdafx.h"
#include "Lib/audio/Soundtypes.hpp"
#include "Lib/audio/audioloader.hpp"
#include "vector.h"
#include "set.h"
#include "map.h"
#include "Lib/std/CRC.hpp"
#include "crtdbg.h"
#include <fstream.h>
#include <ctype.h>

#pragma warning(disable:4786)

void __cdecl dprintf(char* str,...);
 
#pragma pack(push,1)
//*********************************************************************************************
struct SEffect
// prefix: eff
{
	CString			cstrID;
	CString			cstrFilename;
	uint32			u4Hash;
	SSampleFile*	psfFile;
	float			fPlayTime;
	float			fAttenuation;
	float			fMasterVolume;

	CString			cstrFilenameUsed;			// used only while saving
	bool			bLangFound;
	bool			bProcessed;
};

//*********************************************************************************************
struct SMaterialListElement
// prefix: mle
{
	CString			cstrID;
	uint32			u4Hash;
};


//*********************************************************************************************
struct SCollisionSample
// prefix: csm
{
	TSoundMaterial	u4SampleID;
	SSoundTransfer	stTransfer;
};

//*********************************************************************************************
struct SCollision
// prefix: col
{
	uint64				u8Key;

	uint8				u1Samples;
	float				fMinTimeDelay;
	SCollisionSample	csmList[2];
	SCollisionSample	csmSlide;

	void SetMinimumDelay(float f_delay)
	{
		fMinTimeDelay = f_delay;
	}

	float fGetMinDelay()
	{
		return fMinTimeDelay;
	}
	
	void RemoveSlide()
	{
		u1Samples &= 0x7f;
		csmSlide.u4SampleID = 0;
	}

	void RemoveCollision()
	{
		u1Samples &= 0xfc;
		fMinTimeDelay = 0.0f;
		csmList[0].u4SampleID = 0;
		csmList[1].u4SampleID = 0;
	}

	bool bSlide()
	{
		return ((u1Samples & 0x80) != 0);
	}

	bool bCollision()
	{
		return ((u1Samples & 0x03) != 0);
	}

	void SetNumSamples(uint32 u4_count)
	{
		Assert(u4_count<=2);
		u1Samples &= 0xfc;
		u1Samples |= (uint8)u4_count;
	}

	uint32 u4Samples()
	{
		return (uint32)(u1Samples & 0x03);
	}

	void SetCollisionDefaults(uint32 u4_col, uint32 u4_hash)
	{
		Assert(u4_col<2);
		csmList[u4_col].u4SampleID = u4_hash;

		csmList[u4_col].stTransfer.fVolMin = 0.0f;
		csmList[u4_col].stTransfer.fVolMax = 1.0f;
		csmList[u4_col].stTransfer.fVolSlope = 1.0f;
		csmList[u4_col].stTransfer.fVolInt = 0.0f;
		csmList[u4_col].stTransfer.fPitchMin = -1.0f;
		csmList[u4_col].stTransfer.fPitchMax = 1.0f;
		csmList[u4_col].stTransfer.fPitchSlope = 0.0f;
		csmList[u4_col].stTransfer.fPitchInt = 0.0f;
		csmList[u4_col].stTransfer.fAttenuate = 1.0f;
		csmList[u4_col].stTransfer.fMinVelocity = 0.0f;
	}

	void EnableSlide()
	{
		u1Samples |= 0x80;
	}

	void SetSlideDefaults(uint32 u4_hash)
	{
		csmSlide.u4SampleID = u4_hash;
		csmSlide.stTransfer.fVolMin = 0.0f;
		csmSlide.stTransfer.fVolMax = 1.0f;
		csmSlide.stTransfer.fVolSlope = 1.0f;
		csmSlide.stTransfer.fVolInt = 0.0f;
		csmSlide.stTransfer.fPitchMin = -1.0f;
		csmSlide.stTransfer.fPitchMax = 1.0f;
		csmSlide.stTransfer.fPitchSlope = 0.0f;
		csmSlide.stTransfer.fPitchInt = 0.0f;
		csmSlide.stTransfer.fAttenuate = 1.0f;
		csmSlide.stTransfer.fMinVelocity = 0.0f;
	}
};


#pragma pack (pop)

typedef vector<SEffect> TEffectVector;
// prefix ev

typedef vector<SMaterialListElement> TMaterialVector;
// prefix mv

typedef map<CString, SSampleFile*, less<CString> > TFilePosHash;
// prefix fph

typedef map<uint64, SCollision, less<uint64> > TCollisionMap;
// prefix cm



#define fPACKED_AUDIO_TEXT_VERSION	((float)3.00f)

//*********************************************************************************************
class CEffectDatabase
{
public:
	static CCRC*			pcrcHash;
	TEffectVector			evEffects;
	TMaterialVector			mvMaterials;
	TCollisionMap			cmCollisions;

	//*****************************************************************************************
	CEffectDatabase();
	~CEffectDatabase();

	//*****************************************************************************************
	void SaveBinary(const char* str_filename, int i_lang);

	//*****************************************************************************************
	void SaveText(const char* str_filename);

	//*****************************************************************************************
	bool LoadText(const char* str_filename);

	//*****************************************************************************************
	void Reset();

	//*****************************************************************************************
	void SaveTextTransfer(ofstream& file, SCollisionSample& csm);

	//*****************************************************************************************
	uint32 u4Hash(const char* str)
	{
		char	str_local[1024];
		char*	pstr = str_local;

		Assert(str);
		Assert(strlen(str) > 0);
		Assert(strlen(str) < 1024);

		// Remove case sensitivity and forward/backward slash sensitivity.
		while (*str)
		{
			if (*str == '\\')
				*pstr = '/';
			else
			{
				*pstr = tolower(*str);
			}
			str++;
			pstr++;
		}
		*pstr = 0;
		pstr++;

		// hash the result
		return u4HashBlock(str_local,strlen(str_local));
	}


	//*****************************************************************************************
	static uint32 u4HashBlock(const void* pv, int i_size)
	{
		pcrcHash->Reset();
		pcrcHash->CRCBlock(pv, i_size);

		return (*pcrcHash)();
	}


	//*****************************************************************************************
	bool bNewEffect(CString& cstr_id, CString& cstr_filename, float f_vol = 0.0f, float f_atten = 0.0f)
	{
		SEffect	eff;

		if (cstr_id == "")
			return false;

		if (cstr_filename == "")
			return false;

		eff.cstrID = cstr_id;
		eff.cstrFilename = cstr_filename;
		eff.u4Hash = u4Hash(cstr_id);
		eff.psfFile = NULL;

		eff.fPlayTime = 0.0f;
		eff.fAttenuation = f_atten;
		eff.fMasterVolume = f_vol;

		evEffects.push_back(eff);

		return true;
	}

	//*****************************************************************************************
	bool bFindEffectID(CString& cstr_id)
	{
		Assert(cstr_id != "");

		uint32 u4_hash = u4Hash(cstr_id);

		for (TEffectVector::iterator i = evEffects.begin(); i<evEffects.end(); ++i)
		{
			if ( (*i).u4Hash == u4_hash )
			{
				return true;
			}
		}

		return false;
	}

	//*****************************************************************************************
	SEffect* peffFindEffectID(CString& cstr_id)
	{
		Assert(cstr_id != "");

		uint32 u4_hash = u4Hash(cstr_id);

		return peffFindEffectID(u4_hash);
	}


	//*****************************************************************************************
	SEffect* peffFindEffectID(uint32 u4_hash)
	{
		for (TEffectVector::iterator i = evEffects.begin(); i<evEffects.end(); ++i)
		{
			if ( (*i).u4Hash == u4_hash )
			{
				return &(*i);
			}
		}
		return NULL;
	}


	//*****************************************************************************************
	bool bDeleteEffect(CString& cstr_id)
	{
		Assert(cstr_id != "");

		uint32 u4_hash = u4Hash(cstr_id);

		for (TEffectVector::iterator i = evEffects.begin(); i<evEffects.end(); ++i)
		{
			if ( (*i).u4Hash == u4_hash )
			{
				evEffects.erase(i);
				return true;
			}
		}
		return false;
	}

	//*****************************************************************************************
	bool bEffectIDUsed(CString& cstr_id)
	{
		Assert(cstr_id != "");

		uint32 u4_hash = u4Hash(cstr_id);

		return bEffectIDUsed(u4_hash);
	}

	//*****************************************************************************************
	bool bEffectIDUsed(uint32 u4_hash)
	{
		for (TCollisionMap::iterator k = cmCollisions.begin(); k!=cmCollisions.end(); ++k)
		{
			// get the number of samples...
			uint32 u4_sam = (*k).second.u4Samples();
			uint32 u4;

			for (u4=0; u4<u4_sam; u4++)
			{
				if ( (*k).second.csmList[u4].u4SampleID == u4_hash)
					return true;
			}

			// get the number of slide samples
			u4_sam = (*k).second.bSlide()?1:0;

			for (u4=0; u4<u4_sam; u4++)
			{
				if ( (*k).second.csmSlide.u4SampleID == u4_hash)
					return true;
			}
		}

		return false;
	}

	//*****************************************************************************************
	uint32 u4TotalEffects()
	{
		return evEffects.size();
	}

	//*****************************************************************************************
	bool bNewMaterial(CString& cstr_id)
	{
		SMaterialListElement	mle;

		if (cstr_id == "")
			return false;

		mle.cstrID = cstr_id;
		mle.u4Hash = u4Hash(cstr_id);

		mvMaterials.push_back(mle);

		return true;
	}

	//*****************************************************************************************
	void DuplicateMaterial(uint32 u4_new, uint32 u4_source);

	//*****************************************************************************************
	bool bFindMaterialID(CString& cstr_id)
	{
		Assert(cstr_id != "");

		uint32 u4_hash = u4Hash(cstr_id);

		return bFindMaterialID(u4_hash);
	}

	//*****************************************************************************************
	SMaterialListElement* pmleFindMaterialID(uint32 u4_hash)
	{
		for (TMaterialVector::iterator i = mvMaterials.begin(); i<mvMaterials.end(); ++i)
		{
			if ( (*i).u4Hash == u4_hash )
			{
				return &(*i);
			}
		}
		return NULL;
	}

	//*****************************************************************************************
	bool bFindMaterialID(uint32 u4_hash)
	{
		for (TMaterialVector::iterator i = mvMaterials.begin(); i<mvMaterials.end(); ++i)
		{
			if ( (*i).u4Hash == u4_hash )
			{
				return true;
			}
		}

		return false;
	}

	//*****************************************************************************************
	void DeleteMaterial(uint32 u4_hash)
	{
		// go through all materials and make a collision key, then delete it
		for (TMaterialVector::iterator i = mvMaterials.begin(); i<mvMaterials.end(); ++i)
		{
			bDeleteCollision( u8CollisionHash( (*i).u4Hash, u4_hash) );
		}

		// Now erase the material from the material list
		for (i = mvMaterials.begin(); i<mvMaterials.end(); ++i)
		{
			if ( (*i).u4Hash == u4_hash )
			{
				mvMaterials.erase(i);
			}
		}
	}

	//*****************************************************************************************
	void RenameMaterial(uint32 u4_hash, CString& cstr)
	{
		uint32 u4_newhash = u4Hash(cstr);
		uint64 u8_oldkey,u8_newkey;

		for (TMaterialVector::iterator i = mvMaterials.begin(); i<mvMaterials.end(); ++i)
		{
			u8_oldkey = u8CollisionHash( (*i).u4Hash, u4_hash);

			if ( (*i).u4Hash == u4_hash)
			{
				u8_newkey = u8CollisionHash( u4_newhash, u4_newhash);
			}
			else
			{
				u8_newkey = u8CollisionHash( (*i).u4Hash, u4_newhash);
			}
			if (bFindCollision(u8_oldkey))
			{
				cmCollisions[u8_newkey] = cmCollisions[u8_oldkey];
				cmCollisions[u8_newkey].u8Key = u8_newkey;
				bDeleteCollision(u8_oldkey);
			}
		}

		// Now fill the old material list element with the new data
		for (i = mvMaterials.begin(); i<mvMaterials.end(); ++i)
		{
			if ( (*i).u4Hash == u4_hash )
			{
				(*i).cstrID = cstr;
				(*i).u4Hash = u4_newhash;
				break;
			}
		}
	}

	//*****************************************************************************************
	uint32 u4TotalMaterials()
	{
		return mvMaterials.size();
	}

	//*****************************************************************************************
	void DeleteAllMaterials()
	{
		mvMaterials.erase(mvMaterials.begin(),mvMaterials.end());
		cmCollisions.erase(cmCollisions.begin(),cmCollisions.end());
	}

	//*****************************************************************************************
	// Return a unique ID that identifies 2 collisions, it does not matter what order the
	// collisions are specified in.
	// At the moment we return the lowest values collision in the lowest 32 bits and the higher
	// values collision in the upper 32 bits.
	//
	uint64 u8CollisionHash(uint32 u4_mat1, uint32 u4_mat2)
	{
		if (u4_mat1<u4_mat2)
		{
			return ((uint64)u4_mat1) | (((uint64)u4_mat2)<<32);
		}
		else
		{
			return ((uint64)u4_mat2) | (((uint64)u4_mat1)<<32);
		}
	}

	//*****************************************************************************************
	uint32 u8NewCollision(uint64 u8_key)
	{
		SCollision	col;

		//
		// Fill in a collision as a single sample that plays the 'MISSING' sample with a
		// default transfer.
		//

		col.u8Key					= u8_key;
		col.u1Samples				= 0;
		col.fMinTimeDelay			= 0;
		col.csmList[0].u4SampleID	= 0;
		col.csmList[1].u4SampleID	= 0;
		col.csmSlide.u4SampleID		= 0;

		cmCollisions[u8_key] = col;

		return u8_key;
	}

	//*****************************************************************************************
	bool bFindCollision(CString cstr_mat1, CString cstr_mat2)
	{
		uint32	u4_id1 = u4Hash(cstr_mat1);
		uint32	u4_id2 = u4Hash(cstr_mat2);
		uint64 u8_key = u8CollisionHash(u4_id1,u4_id2);

		return bFindCollision(u8_key);
	}

	//*****************************************************************************************
	bool bFindCollision(uint64 u8_key)
	{
		TCollisionMap::iterator	i;

		i = cmCollisions.find(u8_key);
		if (i == cmCollisions.end())
			return false;
		else 
			return true;
	}


	//*****************************************************************************************
	bool bDeleteCollision(uint64 u8_key)
	{
		TCollisionMap::iterator	i;

		i = cmCollisions.find(u8_key);
		if (i == cmCollisions.end())
		{
			return false;
		}

		cmCollisions.erase(i);

		return true;
	}

	//*****************************************************************************************
	uint32 u4TotalCollisions()
	{
		return cmCollisions.size();
	}

	//*****************************************************************************************
	void AdjustMaterialCollisions(uint32 u4_hash_old, uint32 u4_hash_new)
	{
		for (TCollisionMap::iterator k = cmCollisions.begin(); k!=cmCollisions.end(); ++k)
		{

			if ( (*k).second.csmList[0].u4SampleID == u4_hash_old)
				(*k).second.csmList[0].u4SampleID = u4_hash_new;

			if ( (*k).second.csmList[1].u4SampleID == u4_hash_old)
				(*k).second.csmList[1].u4SampleID = u4_hash_new;

			if ( (*k).second.csmSlide.u4SampleID == u4_hash_old)
				(*k).second.csmSlide.u4SampleID = u4_hash_new;
		}
	}
};


//*********************************************************************************************
extern const char* strLeafName(const char* str_filename);
extern bool bFileExists(const char* str_filename);
extern bool bFileExtension(const char* str_filename, const char* str_ext);
extern uint32 u4FileSize(const char* str_filename);
extern void LoadFile(const char* str_filename, void* pv_data);
extern uint32 u4Hash(const void* pv, int i_size, bool string);
const char* strGetLine(ifstream& file);

extern CEffectDatabase	edbEffects;


#endif