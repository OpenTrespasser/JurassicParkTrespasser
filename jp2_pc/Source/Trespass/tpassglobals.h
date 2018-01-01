


#ifndef __TPASSGLOBALS_H__
#define __TPASSGLOBALS_H__

// Global name for a temporary save game.
#define strTEMP_GAME "temp.scx"
#define SAVENAME_SIZE 20

class CRasterDC;
class CDib;

BOOL GetNextAvailSaveName(LPSTR pszFileName, int icFileLen, int iBaseExt = -1);

class CTPassGlobals
{
public:
    CTPassGlobals();
    ~CTPassGlobals();

    void GetSaveBmp(CDib * pdib);
    void SaveBmp(LPSTR pszName);

    BOOL DeleteGame(LPSTR pszName);
    BOOL SaveGame(LPCSTR pszName, int iBaseExt = -1);
    BOOL SaveNamedGame(LPCSTR pszFile, LPCSTR pszName);

	// Load as scene, adding the data directory to the path.
	BOOL LoadLevel(LPCSTR pszName);

    BOOL LoadScene(LPSTR pszScene, LPSTR pszOrigSCN);
    BOOL LoadLastScene();

	// For the separate loading and saving of scenes, needed for the temp autoload file.
	BOOL bSaveScene(LPSTR pszScene);

    void InitGamma();
    int  GetGamma();
    void SetGamma(int iGamma);

    void RenderQualityUp();
    void RenderQualityDn();
    int  GetRenderQuality();
    void SetRenderQuality(int iQuality);

    void SetupBackground();
    void CaptureBackground(bool bBackbuffer = false);
    void DarkenBackground();

    void ResetScreen(BOOL bWithInit = TRUE);

    void CreateMenuAudioDatabase();
    void SetupMenuAudio();
    void SetupButtonAudio();
    void FreeMenuAudio();
    void StartBackgroundAudio();
    void StopBackgroundAudio();
    void PlayRandomAudio();
    void PlayButtonAudio();

    void SaveDefaultKeys();
    void RestoreDefaultKeys();
    void GetSavedKeys();
    void PersistKeys();
	void HardScreenReset(BOOL b_reset_world = FALSE);
	void ClearVidBuffers();
    
    CRasterDC * m_prasBkgnd;
    CRasterDC * m_prasMiniBkgnd;
	bool bInGame;
	bool bHardReset;

    char                m_szSCN[20];

private:
    char                m_szLastScene[_MAX_PATH];
    CAudioDatabase *    m_padbMenu;
    CSample *           m_psamBkgnd;
    CSample *           m_apsamRandoms[13];
    CSample *           m_psamButton;
    SKeyMapping         m_DefaultKeyMapping[KEYMAP_COUNT];
};

extern CTPassGlobals     g_CTPassGlobals;

#endif // __TPASSGLOBALS_H__

