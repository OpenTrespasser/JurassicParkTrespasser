/*******************************************************************
 *
 *    DESCRIPTION: Standard materials, textures, and fog: generic interface
 *
 *    AUTHOR:  Dan Silva
 *
 *    HISTORY:   Created 3/5/96
 *
 *******************************************************************/

#ifndef __STDMAT__H
#define __STDMAT_H

// Shade values
#define NSHADES 	3
#define SHADE_CONST 	0
#define SHADE_PHONG 	1	
#define SHADE_METAL 	2

// Transparency types
#define TRANSP_SUBTRACTIVE     	0
#define TRANSP_ADDITIVE     	1
#define TRANSP_FILTER     		2

#define NTEXMAPS 11

// Texture map indices
#define ID_AM 0   // ambient
#define ID_DI 1   // diffuse
#define ID_SP 2   // specular
#define ID_SH 3   // shininesNs
#define ID_SS 4   // shininess strength
#define ID_SI 5   // self-illumination
#define ID_OP 6   // opacity
#define ID_FI 7   // filter color
#define ID_BU 8   // bump 
#define ID_RL 9   // reflection
#define ID_RR 10  // refraction 

class StdMat: public Mtl {
	public:
	 	virtual void SetShading(int s)=0;
		virtual void SetSoften(BOOL onoff)=0;
		virtual void SetFaceMap(BOOL onoff)=0;
		virtual void SetTwoSided(BOOL onoff)=0;
		virtual void SetWire(BOOL onoff)=0;
		virtual void SetWireUnits(BOOL onOff)=0;
		virtual void SetFalloffOut(BOOL onOff)=0;  // 1: out, 0: in
		virtual void SetTransparencyType(int type)=0;
		virtual void SetAmbient(Color c, TimeValue t)=0;		
		virtual void SetDiffuse(Color c, TimeValue t)=0;		
		virtual void SetSpecular(Color c, TimeValue t)=0;
		virtual void SetFilter(Color c, TimeValue t)=0;
		virtual void SetShininess(float v, TimeValue t)=0;		
		virtual void SetShinStr(float v, TimeValue t)=0;		
		virtual void SetSelfIllum(float v, TimeValue t)=0;		
		virtual void SetOpacity(float v, TimeValue t)=0;		
		virtual void SetOpacFalloff(float v, TimeValue t)=0;		
		virtual void SetWireSize(float s, TimeValue t)=0;
		virtual void SetIOR(float v, TimeValue t)=0;
		virtual void LockAmbDiffTex(BOOL onOff)=0;
		virtual void EnableMap(int i, BOOL onoff)=0;
		virtual void SetTexmapAmt(int imap, float amt, TimeValue t)=0;

	 	virtual int GetShading()=0;
		virtual BOOL GetSoften()=0;
		virtual BOOL GetFaceMap()=0;
		virtual BOOL GetTwoSided()=0;
		virtual BOOL GetWire()=0;
		virtual BOOL GetWireUnits()=0;
		virtual BOOL GetFalloffOut()=0;  // 1: out, 0: in
		virtual int GetTransparencyType()=0;
		virtual Color GetAmbient(TimeValue t)=0;		
		virtual Color GetDiffuse(TimeValue t)=0;		
		virtual Color GetSpecular(TimeValue t)=0;
		virtual Color GetFilter(TimeValue t)=0;
		virtual float GetShininess( TimeValue t)=0;		
		virtual float GetShinStr(TimeValue t)=0;		
		virtual float GetSelfIllum(TimeValue t)=0;		
		virtual float GetOpacity( TimeValue t)=0;		
		virtual float GetOpacFalloff(TimeValue t)=0;		
		virtual float GetWireSize(TimeValue t)=0;
		virtual float GetIOR( TimeValue t)=0;
		virtual BOOL GetAmbDiffTexLock()=0;
		virtual BOOL MapEnabled(int i)=0;
		virtual float GetTexmapAmt(int imap, TimeValue t)=0;

		
	};

// Mapping types for SetCoordMapping
#define UVMAP_EXPLICIT   0
#define UVMAP_SPHERE_ENV 1
#define UVMAP_CYL_ENV  	 2
#define UVMAP_SHRINK_ENV 3
#define UVMAP_SCREEN_ENV 4

class StdUVGen: public UVGen {
	public:
	virtual void SetCoordMapping(int)=0;
	virtual void SetUOffs(float f, TimeValue t)=0;
	virtual void SetVOffs(float f, TimeValue t)=0;
	virtual void SetUScl(float f,  TimeValue t)=0;
	virtual void SetVScl(float f,  TimeValue t)=0;
	virtual void SetAng(float f,   TimeValue t)=0; // angle in radians
	virtual void SetBlur(float f,  TimeValue t)=0;
	virtual void SetBlurOffs(float f,  TimeValue t)=0; 
	virtual void SetNoiseAmt(float f,  TimeValue t)=0; 
	virtual void SetNoiseSize(float f,  TimeValue t)=0; 
	virtual void SetNoiseLev(int i,  TimeValue t)=0; 
	virtual void SetNoisePhs(float f,  TimeValue t)=0; 
	virtual void SetTextureTiling(int tiling)=0;

	virtual int  GetCoordMapping(int)=0;
	virtual float GetUOffs( TimeValue t)=0;
	virtual float GetVOffs( TimeValue t)=0;
	virtual float GetUScl(  TimeValue t)=0;
	virtual float GetVScl(  TimeValue t)=0;
	virtual float GetAng(   TimeValue t)=0; // angle in radians
	virtual float GetBlur(  TimeValue t)=0;
	virtual float GetBlurOffs(  TimeValue t)=0; 
	virtual float GetNoiseAmt(  TimeValue t)=0; 
	virtual float GetNoiseSize(  TimeValue t)=0; 
	virtual int GetNoiseLev( TimeValue t)=0; 
	virtual float GetNoisePhs(  TimeValue t)=0; 
	virtual int GetTextureTiling()=0;

	};

// Image filtering types
#define FILTER_PYR     0
#define FILTER_SAT     1
#define FILTER_NADA	   2

// Alpha source types
#define ALPHA_FILE 	0
#define ALPHA_RGB	2
#define ALPHA_NONE	3

// End conditions:
#define END_LOOP     0
#define END_PINGPONG 1
#define END_HOLD     2

class BitmapTex: public Texmap {
	public:
	virtual void SetFilterType(int ft)=0;
	virtual void SetAlphaSource(int as)=0;  
	virtual void SetEndCondition(int endcond)=0;
	virtual void SetAlphaAsMono(BOOL onoff)=0;
	virtual void SetMapName(TCHAR *name)=0;
	virtual void SetStartTime(TimeValue t)=0;
	virtual void SetPlaybackRate(float r)=0;

	virtual int GetFilterType()=0;
	virtual int GetAlphaSource()=0;
	virtual int GetEndCondition()=0;
	virtual BOOL GetAlphaAsMono(BOOL onoff)=0;
	virtual TCHAR *GetMapName()=0;
	virtual TimeValue GetStartTime()=0;
	virtual float GetPlaybackRate()=0;

	virtual StdUVGen* GetUVGen()=0;
	virtual TextureOutput* GetTexout()=0;
	};

class MultiMtl: public Mtl {
	public:
	virtual void SetNumSubMtls(int n)=0;
	};

class Tex3D: public Texmap {
	public:
	virtual void ReadSXPData(TCHAR *name, void *sxpdata)=0;
	};

class MultiTex: public Texmap {
	public:
	virtual void SetNumSubTexmaps(int n) {}
	virtual void SetColor(int i, Color c, TimeValue t=0){}
	};

class GradTex: public MultiTex {
	public:
	virtual StdUVGen* GetUVGen()=0;
	virtual TextureOutput* GetTexout()=0;
	virtual void SetMidPoint(float m, TimeValue t=0) {}
	};


//===============================================================================
// StdCubic
//===============================================================================
class StdCubic: public Texmap {
	public:
	virtual void SetSize(int n, TimeValue t)=0;
	virtual void SetDoNth(BOOL onoff)=0;
	virtual void SetNth(int n)=0;
	virtual void SetApplyBlur(BOOL onoff)=0;
	virtual void SetBlur(float b, TimeValue t)=0;
	virtual void SetBlurOffset(float b, TimeValue t)=0;
	virtual int GetSize(TimeValue t)=0;
	virtual BOOL GetDoNth()=0;
	virtual int GetNth()=0;
	virtual BOOL GetApplyBlur()=0;
	virtual float GetBlur(TimeValue t)=0;
	virtual float GetBlurOffset(TimeValue t)=0;
	};

//===============================================================================
// StdMirror
//===============================================================================
class StdMirror: public Texmap {
	public:
	virtual void SetDoNth(BOOL onoff)=0;
	virtual void SetNth(int n)=0;
	virtual void SetApplyBlur(BOOL onoff)=0;
	virtual void SetBlur(float b, TimeValue t)=0;
	virtual BOOL GetDoNth()=0;
	virtual int GetNth()=0;
	virtual BOOL GetApplyBlur()=0;
	virtual float GetBlur(TimeValue t)=0;
	};

//===============================================================================
// StdFog
//===============================================================================

// Fallof Types
#define FALLOFF_TOP		0
#define FALLOFF_BOTTOM	1
#define FALLOFF_NONE	2

class StdFog : public Atmospheric {
	public:
	virtual void SetColor(Color c, TimeValue t)=0;
	virtual void SetUseMap(BOOL onoff)=0;
	virtual void SetUseOpac(BOOL onoff)=0;
	virtual void SetColorMap(Texmap *tex)=0;
	virtual void SetOpacMap(Texmap *tex)=0;
	virtual void SetFogBackground(BOOL onoff)=0;
	virtual void SetType(int type)=0;  // 0:Regular, 1:Layered
	virtual void SetNear(float v, TimeValue t)=0;
	virtual void SetFar(float v, TimeValue t)=0;
	virtual void SetTop(float v, TimeValue t)=0;
	virtual void SetBottom(float v, TimeValue t)=0;
	virtual void SetDensity(float v, TimeValue t)=0;
	virtual void SetFalloffType(int tv)=0;
	virtual void SetUseNoise(BOOL onoff)=0;
	virtual void SetNoiseScale(float v, TimeValue t)=0;
	virtual void SetNoiseAngle(float v, TimeValue t)=0;
	virtual void SetNoisePhase(float v, TimeValue t)=0;

	virtual Color GetColor(TimeValue t)=0;
	virtual BOOL GetUseMap()=0;
	virtual BOOL GetUseOpac()=0;
	virtual Texmap *GetColorMap()=0;
	virtual Texmap *GetOpacMap()=0;
	virtual BOOL GetFogBackground()=0;
	virtual int GetType()=0;  // 0:Regular, 1:Layered
	virtual float GetNear(TimeValue t)=0;
	virtual float GetFar(TimeValue t)=0;
	virtual float GetTop(TimeValue t)=0;
	virtual float GetBottom(TimeValue t)=0;
	virtual float GetDensity(TimeValue t)=0;
	virtual int GetFalloffType()=0;
	virtual BOOL GetUseNoise()=0;
	virtual float GetNoiseScale( TimeValue t)=0;
	virtual float GetNoiseAngle( TimeValue t)=0;
	virtual float GetNoisePhase( TimeValue t)=0;

	};


// Subclasses of Tex3D call this on loading to register themselves
// as being able to read sxpdata for sxpName.  (name includes ".SXP")
CoreExport void RegisterSXPReader(TCHAR *sxpName, Class_ID cid);

// When importing, this is called to get a "reader" for the sxp being loaded.
CoreExport Tex3D *GetSXPReaderClass(TCHAR *sxpName);

//==========================================================================
// Create new instances of the standard materials, textures, and atmosphere
//==========================================================================
CoreExport StdMat *NewDefaultStdMat();
CoreExport BitmapTex *NewDefaultBitmapTex();
CoreExport MultiMtl *NewDefaultMultiMtl();
CoreExport MultiTex *NewDefaultCompositeTex();
CoreExport MultiTex *NewDefaultMixTex();
CoreExport MultiTex *NewDefaultTintTex();
CoreExport GradTex *NewDefaultGradTex();
CoreExport StdCubic *NewDefaultStdCubic();
CoreExport StdMirror *NewDefaultStdMirror();
CoreExport StdFog *NewDefaultStdFog();

#endif