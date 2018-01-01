/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1996
 *
 * Contents:
 *		Dialog box for setting sound output properties
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/GUIApp/SoundPropertiesDlg.hpp                                        $
 * 
 * 12    9/17/98 3:03p Rwyatt
 * Removed subtitle control icons
 * 
 * 11    98.08.13 4:25p Mmouni
 * Changes for VC++ 5.0sp3 compatibility.
 * 
 * 10    8/04/98 3:56p Rwyatt
 * New controls for EAX properties
 * 
 * 9     7/29/98 10:49a Rwyatt
 * Added EAX support
 * 
 * 8     5/24/98 9:59p Rwyatt
 * Added button to disable audio acceleration
 * 
 * 7     2/11/98 4:36p Rwyatt
 * Fill back ground option. Subtitles now use GDI
 * 
 * 6     2/06/98 8:20p Rwyatt
 * New options to control subtitles
 * 
 * 5     11/14/97 12:04a Rwyatt
 * 
 * 4     10/23/97 6:54p Rwyatt
 * New dialog settings for feature enable/disable and speaker configuration
 * 
 * 3     7/07/97 11:53p Rwyatt
 * Added an enable and disable audio control
 * 
 * 2     6/04/97 7:03p Rwyatt
 * Initial Implementation
 * 
 * 1     6/04/97 5:32p Rwyatt
 * Initial Implementation
 * 
 *********************************************************************************************/

#ifndef HEADER_GUIAPP_DIALOGSOUNDPROP_HPP
#define HEADER_GUIAPP_DIALOGSOUNDPROP_HPP


//
// Class definitions.
//

//*********************************************************************************************
//
class CDialogSoundProp : public CDialog
//
// Sound settings dialog box.
//
//**************************************
{
public:

	//*****************************************************************************************
	//
	// Constructor.
	//

	//*****************************************************************************************
	CDialogSoundProp(CWnd* pParent = NULL);

	//*****************************************************************************************
	//
	// MFC-Generated code.
	//

	//{{AFX_DATA(CDialogSoundProp)
	enum { IDD = IDD_SOUND_PROPERTIES };
	CSliderCtrl			sliderFrequency;

	CSliderCtrl			sliderVolume;
	CSliderCtrl			sliderDecay;
	CSliderCtrl			sliderDamping;				// eax sliders

	CString				strVolume;
	CString				strDecay;
	CString				strDamping;

	int					iEnvironment;
	int					iSubtitles;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogSoundProp)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

protected:

	void SetText(uint32 u4_format);
	void SetIconState(bool b_enabled);
	void UpdateSpeakerIcons();
	//*****************************************************************************************
	//
	// MFC-Generated code.
	//

	//{{AFX_MSG(CDialogSoundProp)
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void On8Bit();
	afx_msg void On16Bit();
	afx_msg void OnStereo();
	afx_msg void OnMono();

	afx_msg void OnSpeakerMono();
	afx_msg void OnSpeakerStereo();
	afx_msg void OnSpeakerQuad();
	afx_msg void OnSpeakerSurround();
	afx_msg void OnSpeakerPhones();

	afx_msg void OnDisableEffects();
	afx_msg void OnDisableVoices();
	afx_msg void OnDisableAmbient();
	afx_msg void OnDisableMusic();


	afx_msg void OnEnableSubtitles();
	afx_msg void OnSetHW();
	afx_msg void OnSetEnvironment();

	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


#endif
