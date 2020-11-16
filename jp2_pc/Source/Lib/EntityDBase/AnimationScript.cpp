/***********************************************************************************************
 *
 * Copyright © DreamWorks Interactive. 1998
 *
 * Implementation of AnimationScript.hpp.
 *
 ***********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/Lib/EntityDBase/AnimationScript.cpp                                   $
 * 
 * 12    9/26/98 9:01p Agrant
 * Disable player input during animations involving the camera.
 * Save the animated camera info.
 * 
 * 11    9/03/98 5:16p Mlange
 * If the camera is controlled by the animation script it will now be freed first and
 * re-attached to the original instance when done.
 * 
 * 10    8/25/98 3:00p Rvande
 * Added fix to allow class instatiation (requirement, according to the C++ standard)
 * 
 * 9     8/17/98 12:55p Mlange
 * Can now disable auto frame grabber at end of animation script sequence.
 * 
 * 8     8/14/98 7:37p Mlange
 * Fixed bug - not all constructors were initialising the playback rate data member.
 * 
 * 7     8/13/98 1:46p Mlange
 * The step message now requires registration of its recipients.
 * 
 * 6     8/05/98 3:15p Mlange
 * Added support for forced playback rate. Increased version number.
 * 
 **********************************************************************************************/

#include "gblinc/common.hpp"
#include "AnimationScript.hpp"

#include "Lib/Sys/DebugConsole.hpp"
#include "Lib/Math/FastTrig.hpp"
#include "Lib/Std/PrivSelf.hpp"
#include "Lib/Std/Hash.hpp"
#include "Lib/EntityDBase/WorldDBase.hpp"
#include "Lib/EntityDBase/Instance.hpp"
#include "Lib/EntityDBase/MessageTypes/MsgStep.hpp"
#include "Lib/Loader/SaveBuffer.hpp"
#include "Lib/Loader/SaveFile.hpp"
#include "Lib/Renderer/Camera.hpp"
#include "Lib/View/Grab.hpp"
#include "Game/DesignDaemon/Player.hpp"
#include <string>
#include <set>
#include <ctype.h>
#include <fstream>
#include <iomanip>
#include <algorithm>


namespace
{
	// Current version number of the anim script file format.
	const uint uANIM_SCRIPT_VERSION = 2;

	// Far away location (off the edge of the world) to move objects to, to make them dissapear.
	CVector3<> v3OffWorldPos(10000, 10000, 10000);

	//******************************************************************************************
	//
	CRotate3<> r3Slerp(float f_t, const CRotate3<>& r3_src, const CRotate3<>& r3_dst)
	//
	// Returns:
	//		Spherical interpolation of quaternions.
	//
	//**************************************
	{
		float f_cosom = r3_src.v3S * r3_dst.v3S + r3_src.tC * r3_dst.tC;

		// Make sure we interpolate across shortest arc.
		CRotate3<> r3_src_cpy = r3_src;

		if (f_cosom < 0) 
		{
			f_cosom = fabs(f_cosom);

			r3_src_cpy = CRotate3<>(-r3_src.tC, -r3_src.v3S);
		}

		float f_sclp;
		float f_sclq;

		if ((1 + f_cosom) > .001f)
		{
			if ((1 - f_cosom) > .001f)
			{
				float f_omega = acos(f_cosom);
				float f_sinom = sin(f_omega);
				
				f_sclp = sin((1 - f_t) * f_omega) / f_sinom;
				f_sclq = sin(     f_t  * f_omega) / f_sinom;
			}
			else
			{
				// Quaternions nearly identical, use linear interpolation instead.
				f_sclp = 1 - f_t;
				f_sclq = f_t;
			}

			return CRotate3<>(r3_src_cpy.tC     * f_sclp + r3_dst.tC     * f_sclq,
							  r3_src_cpy.v3S.tX * f_sclp + r3_dst.v3S.tX * f_sclq,
							  r3_src_cpy.v3S.tY * f_sclp + r3_dst.v3S.tY * f_sclq,
							  r3_src_cpy.v3S.tZ * f_sclp + r3_dst.v3S.tZ * f_sclq );
		}
		else
		{
			// Quaternions nearly opposite.
			f_sclp = sin((1 - f_t) * dPI_2);
			f_sclq = sin(     f_t  * dPI_2);

			return CRotate3<>(r3_src_cpy.v3S.tZ,
							  r3_src_cpy.v3S.tX * f_sclp - r3_src_cpy.v3S.tY * f_sclq,
							  r3_src_cpy.v3S.tY * f_sclp + r3_src_cpy.v3S.tX * f_sclq,
							  r3_src_cpy.v3S.tZ * f_sclp - r3_src_cpy.tC     * f_sclq );
		}
	}

	//******************************************************************************************
	void ToLower(std::string& str)
	// Convert given string to lowercase.
	{
		for (int i = 0; i < str.length(); i++)
			str[i] = tolower(str[i]);
	}
}


//**********************************************************************************************
//
class NoCaseCmp
//
// Case insensitive compare object for STL strings.
//
//**************************************
{
public:
	bool operator()(const std::string& s1, const std::string& s2) const;
};

//**********************************************************************************************
//
class CScriptParser
//
// Parse ASCII text script.
//
// Prefix: sp
//
//**************************************
{
	std::ifstream streamScript;

	std::set<std::string, NoCaseCmp> setKeywords;

	//******************************************************************************************
	//
	// Constructors and destructor.
	//
public:
	CScriptParser(const char* str_file);


	//******************************************************************************************
	//
	// Member functions.
	//

	//******************************************************************************************
	//
	void Parse
	(
		CAnimationScript& ras_init
	);
	//
	//
	//**************************************

private:
	//******************************************************************************************
	//
	bool bNextToken();
	//
	// Returns:
	//		'true' if token(s) remaining.
	//
	//**************************************

	//******************************************************************************************
	//
	bool bIsKeyword(const std::string& token) const;
	//
	// Returns:
	//		'true' if token is a keyword.
	//
	//**************************************

	//******************************************************************************************
	//
	bool bReadKeyword(std::string& token, const char* token_match = 0);
	//
	// Read next token or find next named token in stream. On return, stream will point after
	// token. The token is converted to lowercase.
	//
	// Returns:
	//		'true' if keyword found.
	//
	//**************************************

	//******************************************************************************************
	//
	bool bReadIdentifier(std::string& token);
	//
	// Read next identifier in stream. On return, stream will point after token.
	//
	// Returns:
	//		'true' if identifier found.
	//
	//**************************************

	//******************************************************************************************
	//
	bool bReadLiteral(float& lit);
	//
	// Read next literal in stream. On return, stream will point after literal.
	//
	// Returns:
	//		'true' if literal found.
	//
	//**************************************

	//******************************************************************************************
	//
	void OutputError(const char* str_text, const std::string* str_token = 0) const;
	//
	// Output script parse failure, in VER_TEST mode only.
	//
	//**************************************
};



//**********************************************************************************************
//
// CAnimationScript implementation.
//

	//**********************************************************************************************
	//
	class CAnimationScript::CPriv : public CAnimationScript
	//
	// Private implementation of CAnimationScript.
	//
	//**************************************
	{
	public:
	#ifdef __MWERKS__
		// The base class has no default constructor, hence
		// this class needs to explicitly create the base
		// class.  Although this class is never instantiated,
		// an un-instantiatable class (in this context) is
		// invalid, according to the C++ standard.
		CPriv(const char* str_name, const char* str_base_dir) : CAnimationScript(str_name, str_base_dir) {};
	#endif

		//******************************************************************************************
		//
		void Load(const char* str_filename);
		//
		// Load script in binary format.
		//
		//**************************************

		//******************************************************************************************
		//
		void Save(const char* str_filename);
		//
		// Save script in binary format.
		//
		//**************************************

		//******************************************************************************************
		//
		void Init();
		//
		// Init after load.
		//
		//**************************************
	};


	//******************************************************************************************
	CAnimationScript::CAnimationScript(const char* str_name, const char* str_base_dir)
		: fPlaybackRate(0), bDisableAutoGrab(false), bScriptHasMovedCamera(false)
	{
		std::string str_dir = str_base_dir;
		str_dir += '/';
	
		// First, determine if a binary version of the requested animation script exists.
		std::string str_bin_filename = str_dir + str_name + ".asb";

		std::ifstream stream_file(str_bin_filename.c_str(), std::ios::in | std::ios::_Nocreate | std::ios::binary);

		if (stream_file.good())
		{
			stream_file.close();

			priv_self.Load(str_bin_filename.c_str());
		}
		else
		{
			// Load the ASCII version.
			std::string str_asc_filename = str_dir + "anim/"+ str_name + ".asa";

			CScriptParser sp(str_asc_filename.c_str());
			sp.Parse(*this);
	
			// And save it out in binary.
			priv_self.Save(str_bin_filename.c_str());
		}

		priv_self.Init();
	}


	//******************************************************************************************
	CAnimationScript::CAnimationScript(const char* str_filename, bool b_disable_autograb)
		: fPlaybackRate(0), bDisableAutoGrab(b_disable_autograb), bScriptHasMovedCamera(false)
	{
		std::string str_file = str_filename;
		std::string str_ext  = str_file.substr(str_file.find_last_of('.') + 1);

		ToLower(str_ext);

		if (str_ext == "asb")
			priv_self.Load(str_filename);
		else
		{
			// Load the ASCII version.
			CScriptParser sp(str_filename);
			sp.Parse(*this);
		}

		priv_self.Init();
	}


	//******************************************************************************************
	CAnimationScript::~CAnimationScript()
	{
		for (int i_obj = 0; i_obj < paiaiAnimation.uLen; i_obj++)
		{
			delete paiaiAnimation[i_obj].paafrFrames.atArray;
			paiaiAnimation[i_obj].paafrFrames.uLen    = 0;
			paiaiAnimation[i_obj].paafrFrames.atArray = 0;
		}

		delete paiaiAnimation.atArray;
		paiaiAnimation.uLen    = 0;
		paiaiAnimation.atArray = 0;

		delete paiapPlayback.atArray;
		paiapPlayback.uLen    = 0;
		paiapPlayback.atArray = 0;
	}


	//******************************************************************************************
	void CAnimationScript::Rewind()
	{
		sTotalElapsed = 0;

		// Move all instances that are part of the animation to some far away, off world
		// location - except for the camera.
		for (int i_obj = 0; i_obj < paiapPlayback.uLen; i_obj++)
		{
			paiapPlayback[i_obj].iFrameNum = 0;

			CInstance* pins = paiapPlayback[i_obj].pinsInstance;

			if (pins && !ptCast<CCamera>(pins))
				pins->Move(CPlacement3<>(v3OffWorldPos));
		}

		// Setup animating instances for the first frame.
		FrameAdvance(0);
	}


	//******************************************************************************************
	void CAnimationScript::Start()
	{
		bActive = true;
	}

	//******************************************************************************************
	void CAnimationScript::Stop()
	{
		if (bDisableAutoGrab)
			pagAutoGrabber->Stop();

		// Put camera back on the instance it was on before the animation moved it.
		if (bScriptHasMovedCamera)
		{
			pwWorld->pcamGetActiveCamera()->SetAttached(pinsCameraOn, bCameraOnHead);
			pwWorld->pcamGetActiveCamera()->SetRelative(p3CameraRelative);

			CPlayer *pplay = ptCast<CPlayer>(pinsCameraOn);
			if (pplay)
				pplay->bControl = true;
		}

		bActive = false;
		bScriptHasMovedCamera = false;
	}

	//******************************************************************************************
	void CAnimationScript::FrameAdvance(TSec s_elapsed)
	{
		if (fPlaybackRate != 0 && s_elapsed != 0)
			s_elapsed = 1.0f / fPlaybackRate;

		sTotalElapsed += s_elapsed;

		bool b_frames_remaining = false;

		for (int i_obj = 0; i_obj < paiapPlayback.uLen; i_obj++)
		{
			const CPArray<SAnimFrame>& paafr_obj = paiaiAnimation[i_obj].paafrFrames;

			// Is current time beyond the last frame time for this object?
			if (sTotalElapsed <= paafr_obj[paafr_obj.uLen - 1].sTime)
				b_frames_remaining = true;

			// Find the two frames the current time lies between.
			int i_frame_b;

			for (i_frame_b = paiapPlayback[i_obj].iFrameNum; i_frame_b < paafr_obj.uLen; i_frame_b++)
				if (paafr_obj[i_frame_b].sTime > sTotalElapsed)
					break;

			if (i_frame_b != 0)
			{
				int i_frame_a = i_frame_b - 1;

				if (i_frame_b == paafr_obj.uLen)
					i_frame_b = paafr_obj.uLen - 1;

				paiapPlayback[i_obj].iFrameNum = i_frame_a;

				Assert(paafr_obj[i_frame_a].sTime <= sTotalElapsed);

				float f_t = (sTotalElapsed - paafr_obj[i_frame_a].sTime) / (paafr_obj[i_frame_b].sTime - paafr_obj[i_frame_a].sTime);

				SetMin(f_t, 1);

				// Interpolate position.
				CVector3<> v3_interp_pos = (paafr_obj[i_frame_b].pr3Location.v3Pos -
				                            paafr_obj[i_frame_a].pr3Location.v3Pos  ) * f_t + paafr_obj[i_frame_a].pr3Location.v3Pos;

				// Interpolate orientation.
				CRotate3<> r3_interp_rot = r3Slerp(f_t, paafr_obj[i_frame_a].pr3Location.r3Rot, paafr_obj[i_frame_b].pr3Location.r3Rot);

				// Interpolate scale.
				float f_interp_scale = (paafr_obj[i_frame_b].pr3Location.rScale -
				                        paafr_obj[i_frame_a].pr3Location.rScale  ) * f_t + paafr_obj[i_frame_a].pr3Location.rScale;

				CPresence3<> pr3_interp(r3_interp_rot, f_interp_scale, v3_interp_pos);

				if (paiapPlayback[i_obj].pinsInstance)
				{
					// Are we about to move the camera and also for the first time?
					CCamera* pcam = ptCast<CCamera>(paiapPlayback[i_obj].pinsInstance);
					if (pcam && !bScriptHasMovedCamera)
					{
						bScriptHasMovedCamera = true;

						// We will animate the camera which requires it to be free. Record the instance
						// the camera was on so we can restore this when the animation ends.
						pinsCameraOn     = pcam->pinsAttached();
						p3CameraRelative = pcam->p3Relative();
						bCameraOnHead    = pcam->bHead();

						// Free the camera.
						pcam->SetAttached(0);

						CPlayer *pplay = ptCast<CPlayer>(pinsCameraOn);
						if (pplay)
							pplay->bControl = false;
					}

					paiapPlayback[i_obj].pinsInstance->SetScale(f_interp_scale * paiapPlayback[i_obj].fInitialScale);
					paiapPlayback[i_obj].pinsInstance->Move(pr3_interp);
				}
			}
		}

		if (!b_frames_remaining)
			Stop();
	}

	//*****************************************************************************************
	char * CAnimationScript::pcSave(char *  pc_buffer) const
	{
		pc_buffer = pcSaveT(pc_buffer, bActive);
		pc_buffer = pcSaveT(pc_buffer, sTotalElapsed);

		pc_buffer = pcSaveT(pc_buffer, bScriptHasMovedCamera);

		return pc_buffer;
	}

	//*****************************************************************************************
	const char * CAnimationScript::pcLoad(const char *  pc_buffer)
	{
		Rewind();

		pc_buffer = pcLoadT(pc_buffer, &bActive);
		pc_buffer = pcLoadT(pc_buffer, &sTotalElapsed);

		if (CSaveFile::iCurrentVersion >= 18)
		{
			pc_buffer = pcLoadT(pc_buffer, &bScriptHasMovedCamera);
		}
		
		FrameAdvance(0);

		return pc_buffer;
	}


//**********************************************************************************************
//
// CAnimationScript::CPriv implementation.
//

	//******************************************************************************************
	void CAnimationScript::CPriv::Load(const char* str_filename)
	{
		Assert(paiaiAnimation.uLen == 0 && paiapPlayback.uLen == 0);

		std::ifstream stream_file(str_filename, std::ios::in | std::ios::_Nocreate | std::ios::binary);

		// Make sure the file was successfully opened for loading.
		AlwaysAssert(stream_file.good());

		uint u_version;
		stream_file.read(reinterpret_cast<char*>(&u_version),     sizeof(uint));
		stream_file.read(reinterpret_cast<char*>(&fPlaybackRate), sizeof(float));
		
		// Make sure the version number of the file matches.
		AlwaysAssert(uANIM_SCRIPT_VERSION == u_version);

		uint u_num_obj;
		stream_file.read(reinterpret_cast<char*>(&u_num_obj), sizeof(uint));

		CPArray<SInstanceAnimInfo> paiai_ret(u_num_obj);

		// Load each object.
		for (int i_obj = 0; i_obj < paiai_ret.uLen; i_obj++)
		{
			stream_file.read(reinterpret_cast<char*>(&paiai_ret[i_obj].u4HashName), sizeof(uint32));

			uint u_num_frames;
			stream_file.read(reinterpret_cast<char*>(&u_num_frames), sizeof(uint));

			paiai_ret[i_obj].paafrFrames = CPArray<SAnimFrame>(u_num_frames);

			// Load animation frames.
			stream_file.read(reinterpret_cast<char*>(&paiai_ret[i_obj].paafrFrames[0]), u_num_frames * sizeof(SAnimFrame));
		}

		// Make sure we encountered no errors while loading.
		AlwaysAssert(stream_file.good());

		paiaiAnimation = paiai_ret;
	}

	//******************************************************************************************
	void CAnimationScript::CPriv::Save(const char* str_filename)
	{
		std::ofstream stream_file(str_filename, std::ios::out | std::ios::trunc | std::ios::binary);

		// Make sure the file was successfully opened for saving.
		AlwaysAssert(stream_file.good());

		stream_file.write(reinterpret_cast<const char*>(&uANIM_SCRIPT_VERSION), sizeof(uint));
		stream_file.write(reinterpret_cast<const char*>(&fPlaybackRate),        sizeof(float));

		stream_file.write(reinterpret_cast<const char*>(&paiaiAnimation.uLen), sizeof(uint));

		// Save each object.
		for (int i_obj = 0; i_obj < paiaiAnimation.uLen; i_obj++)
		{
			stream_file.write(reinterpret_cast<const char*>(&paiaiAnimation[i_obj].u4HashName), sizeof(uint32));

			// Save animation frames.
			uint u_num_frames = paiaiAnimation[i_obj].paafrFrames.uLen;

			stream_file.write(reinterpret_cast<const char*>(&u_num_frames), sizeof(uint));
			stream_file.write(reinterpret_cast<const char*>(&paiaiAnimation[i_obj].paafrFrames[0]), u_num_frames * sizeof(SAnimFrame));
		}

		// Make sure we encountered no errors while saving.
		AlwaysAssert(stream_file.good());
	}


	//******************************************************************************************
	void CAnimationScript::CPriv::Init()
	{
		paiapPlayback = CPArray<SInstanceAnimPlay>(paiaiAnimation.uLen);

		// Find all animating CInstances in world database.
		for (int i_obj = 0; i_obj < paiaiAnimation.uLen; i_obj++)
		{
			paiapPlayback[i_obj].pinsInstance = pwWorld->pinsFindInstance(paiaiAnimation[i_obj].u4HashName);

			// Verify that the instance was found in the dbase.
			Assert(paiapPlayback[i_obj].pinsInstance);

			if (paiapPlayback[i_obj].pinsInstance)
				paiapPlayback[i_obj].fInitialScale = paiapPlayback[i_obj].pinsInstance->fGetScale();
		}

		Stop();
		Rewind();
	}

//**********************************************************************************************
//
// CScriptParser implementation.
//

	//******************************************************************************************
	bool NoCaseCmp::operator()(const std::string& s1, const std::string& s2) const
	{
		int i_index;
		for (i_index = 0; i_index < Min(s1.length(), s2.length()); i_index++)
			if (tolower(s1[i_index]) != tolower(s2[i_index]))
				break;

		if (i_index == Min(s1.length(), s2.length()))
			return s1.length() < s2.length();

		return tolower(s1[i_index]) < tolower(s2[i_index]);
	}

	//**********************************************************************************************
	//
	class AnimFrameCmp
	//
	// Compare object for animation frames.
	//
	//**************************************
	{
	public:
		bool operator()(const CAnimationScript::SAnimFrame& afr1, const CAnimationScript::SAnimFrame& afr2) const
		{
			return afr1.sTime < afr2.sTime;
		}
	};


	//******************************************************************************************
	namespace
	{
		std::string astrKeywords[] =
		{
			"version",
			"forced_rate",
			"object",
			"end_object",
			"frame",
			"pos",
			"rot",
			"scale"
		};
	}

	//******************************************************************************************
	CScriptParser::CScriptParser(const char* str_file)
		: streamScript(str_file, std::ios::in | std::ios::_Nocreate),
		  setKeywords(astrKeywords, &astrKeywords[sizeof(astrKeywords) / sizeof(std::string)])
	{
		// Ensure the file was opened successfully.
		AlwaysAssert(!streamScript.fail());
	}


	//******************************************************************************************
	//
	// Workaround for stupid, annoying, broken MS compiler.
	//
	struct CAnimationScript_SInstanceAnimInfo : public CAnimationScript::SInstanceAnimInfo {};

	struct CAnimationScript_SAnimFrame : public CAnimationScript::SAnimFrame {};

	//******************************************************************************************
	void CScriptParser::Parse(CAnimationScript& ras_init)
	{
		std::vector<CAnimationScript_SInstanceAnimInfo> vciai_anim_ins;

		std::string str_token;

		// Read version info.
		if (!bReadKeyword(str_token, "version"))
			goto end;

		float f_version;
		if (!bReadLiteral(f_version))
			goto end;

		AlwaysAssert(f_version == uANIM_SCRIPT_VERSION);


		while (bNextToken())
		{
			if (!bReadKeyword(str_token))
				goto end;

			if (str_token == "forced_rate")
			{
				if (!bReadLiteral(ras_init.fPlaybackRate))
					goto end;
			}
			else if (str_token == "object")
			{
				std::string str_obj_name;
				if (!bReadIdentifier(str_obj_name))
					goto end;

				uint32 u4_hash_name;

				if (str_obj_name == "Camera")
					u4_hash_name = pwWorld->pcamGetActiveCamera()->u4GetUniqueHandle();
				else
					u4_hash_name = u4Hash(str_obj_name.c_str());

				// Read all the animation frames.
				std::vector<CAnimationScript_SAnimFrame> vcafr_frames;

				while (bReadKeyword(str_token, "frame"))
				{
					float f_time;
					if (!bReadLiteral(f_time))
						goto end;

					// Read position.
					if (!bReadKeyword(str_token, "pos"))
						goto end;

					CVector3<> v3_pos;
					if (!bReadLiteral(v3_pos.tX))
						goto end;
					if (!bReadLiteral(v3_pos.tY))
						goto end;
					if (!bReadLiteral(v3_pos.tZ))
						goto end;

					// Read orientation.
					if (!bReadKeyword(str_token, "rot"))
						goto end;

					CRotate3<> r3_rotate;
					if (!bReadLiteral(r3_rotate.v3S.tX))
						goto end;
					if (!bReadLiteral(r3_rotate.v3S.tY))
						goto end;
					if (!bReadLiteral(r3_rotate.v3S.tZ))
						goto end;
					if (!bReadLiteral(r3_rotate.tC))
						goto end;

					r3_rotate.Normalise(true);

					// Read scale.
					if (!bReadKeyword(str_token, "scale"))
						goto end;

					float f_scale;
					if (!bReadLiteral(f_scale))
						goto end;

					// Add this frame.
					CAnimationScript_SAnimFrame afr_new;
					afr_new.sTime       = f_time;
					afr_new.pr3Location = CPresence3<>(r3_rotate, f_scale, v3_pos);

					vcafr_frames.push_back(afr_new);
				}

				if (str_token != "end_object")
					goto end;

				// Sort animation frames by time.
				sort(vcafr_frames.begin(), vcafr_frames.end(), AnimFrameCmp());

				// Copy animation frames into a new animation structure.
				CAnimationScript_SInstanceAnimInfo iai_new;
				iai_new.u4HashName  = u4_hash_name;
				iai_new.paafrFrames = CPArray<CAnimationScript::SAnimFrame>(vcafr_frames.size());

				int i_index = 0;
				for (std::vector<CAnimationScript_SAnimFrame>::iterator it = vcafr_frames.begin(); it != vcafr_frames.end(); ++it, ++i_index)
					iai_new.paafrFrames[i_index] = *it;

				// Add animation structure to list.
				vciai_anim_ins.push_back(iai_new);
			}
		}
	
	end:
		CPArray<CAnimationScript::SInstanceAnimInfo> paiai_ret(vciai_anim_ins.size());

		int i_index = 0;
		for (std::vector<CAnimationScript_SInstanceAnimInfo>::iterator it = vciai_anim_ins.begin(); it != vciai_anim_ins.end(); ++it, ++i_index)
			paiai_ret[i_index] = *it;

		ras_init.paiaiAnimation = paiai_ret;
	}


	//******************************************************************************************
	bool CScriptParser::bNextToken()
	{
		char c;
		while (streamScript.get(c))
		{
			if (!isspace(c))
			{
				streamScript.putback(c);
				return true;
			}
		}

		return false;
	}

	//******************************************************************************************
	bool CScriptParser::bIsKeyword(const std::string& token) const
	{
		return setKeywords.find(token) != setKeywords.end();
	}

	//******************************************************************************************
	bool CScriptParser::bReadKeyword(std::string& token, const char* token_match)
	{
		if (streamScript.eof() || streamScript.fail())
		{
			OutputError("unexpected end of file");
			return false;
		}

		streamScript >> token;
		ToLower(token);

		if (!bIsKeyword(token))
		{
			OutputError("expected keyword", &token);
			return false;
		}

		return !token_match || token == token_match;
	}

	//******************************************************************************************
	bool CScriptParser::bReadIdentifier(std::string& token)
	{
		if (streamScript.eof() || streamScript.fail())
		{
			OutputError("unexpected end of file");
			return false;
		}

		streamScript >> token;

		if (bIsKeyword(token))
		{
			OutputError("indentifier may not be a keyword", &token);
			return false;
		}

		return true;
	}

	//******************************************************************************************
	bool CScriptParser::bReadLiteral(float& lit)
	{
		if (streamScript.eof() || streamScript.fail())
		{
			OutputError("unexpected end of file");
			return false;
		}

		streamScript >> lit;

		if (!streamScript.good())
		{
			std::string token;
			streamScript.clear();
			streamScript >> token;

			OutputError("not a literal", &token);
			return false;
		}

		return true;
	}

	//******************************************************************************************
	void CScriptParser::OutputError(const char* str_text, const std::string* str_token) const
	{
	#if VER_TEST
		dout << "Animation script error: '" << str_text << "' ";
		
		if (str_token)
			dout << *str_token;
			
		dout << '\n';
	#endif
	}


//**********************************************************************************************
//
// CAnimations implementation.
//

	//******************************************************************************************
	CAnimations::CAnimations()
	{
		SetInstanceName("CAnimations");

		// Register this entity with the message types it needs to receive.
		CMessageStep::RegisterRecipient(this);
	}

	//******************************************************************************************
	CAnimations::~CAnimations()
	{
		CMessageStep::UnregisterRecipient(this);

		for (std::list<CAnimationScript*>::iterator it = ltansAnims.begin(); it != ltansAnims.end(); ++it)
			delete *it;
	}

	//******************************************************************************************
	void CAnimations::Add(CAnimationScript* pans)
	{
		// If animation exists, rewind it.
		CAnimationScript* pans_exists = pansFind(pans);

		if (pans_exists)
			pans_exists->Rewind();
		else
			// Add new animation.
			ltansAnims.push_back(pans);
	}

	//******************************************************************************************
	void CAnimations::Remove(CAnimationScript* pans)
	{
		ltansAnims.remove(pans);
	}

	//******************************************************************************************
	CAnimationScript* CAnimations::pansFind(const CAnimationScript* pans) const
	{
		for (std::list<CAnimationScript*>::const_iterator it = ltansAnims.begin(); it != ltansAnims.end(); ++it)
			if (pans == (*it))
				return *it;

		return 0;
	}

	//******************************************************************************************
	void CAnimations::Process(const CMessageStep& msgstep)
	{
		for (std::list<CAnimationScript*>::iterator it = ltansAnims.begin(); it != ltansAnims.end(); ++it)
			if ((*it)->bIsPlaying())
				(*it)->FrameAdvance(msgstep.sStep);
	}

	//*****************************************************************************************
	char * CAnimations::pcSave(char *  pc_buffer) const
	{
		for (std::list<CAnimationScript*>::const_iterator it = ltansAnims.begin(); it != ltansAnims.end(); ++it)
			pc_buffer = (*it)->pcSave(pc_buffer);

		return pc_buffer;
	}

	//*****************************************************************************************
	const char * CAnimations::pcLoad(const char *  pc_buffer)
	{
		for (std::list<CAnimationScript*>::iterator it = ltansAnims.begin(); it != ltansAnims.end(); ++it)
			pc_buffer = (*it)->pcLoad(pc_buffer);

		return pc_buffer;
	}

//******************************************************************************************
CAnimations* pAnimations;
