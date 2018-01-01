/**********************************************************************************************
 *
 * Copyright © DreamWorks Interactive, 1996
 *
 * Contents: Implementation of SoundPropertiesDlg.hpp
 *
 **********************************************************************************************
 *
 * $Log:: /JP2_PC/Source/GUIApp/SoundPropertiesDlg.cpp                                              $
 * 
 * 16    9/17/98 3:03p Rwyatt
 * Removed subtitle control icons
 * 
 * 15    9/04/98 2:55a Rwyatt
 * Write hardware state to the registry
 * 
 * 14    98.08.13 4:25p Mmouni
 * Changes for VC++ 5.0sp3 compatibility.
 * 
 * 13    8/05/98 10:17p Rwyatt
 * Disabled all EAX icons when no EAX card is present
 * 
 * 12    8/04/98 3:56p Rwyatt
 * New controls for EAX properties
 * 
 * 11    7/29/98 10:49a Rwyatt
 * Added EAX support
 * 
 * 10    5/24/98 9:59p Rwyatt
 * Added button to disable audio acceleration
 * 
 * 9     2/11/98 4:36p Rwyatt
 * Fill back ground option. Subtitles now use GDI
 * 
 * 8     2/06/98 8:20p Rwyatt
 * New options to control subtitles
 * 
 * 7     11/22/97 10:40p Rwyatt
 * Uses new global audio pointer which is a static member of the CAudio class
 * 
 * 6     11/14/97 12:03a Rwyatt
 * Removed enable/disble icon
 * Added icons for disablng specific sections of audio
 * 
 * 5     97/11/10 16:01 Speter
 * Switched include order.
 * 
 * 4     10/23/97 6:54p Rwyatt
 * New control in dialog for setting speaker configuration and disabling various elements of the
 * audio system.
 * 
 * 3     7/07/97 11:52p Rwyatt
 * Added an enable and disable audio control
 * 
 * 2     6/04/97 7:00p Rwyatt
 * Initial implementation of Sound Outpuit Properties
 * 
 * 1     6/04/97 5:32p Rwyatt
 * Initial Implementation
 * 
 *********************************************************************************************/


//// Includes.
//
#include "stdafx.h"
#include "SoundPropertiesDlg.hpp"
#include "GUIPipeLine.hpp"
#include "GUITools.hpp"
#include "GUIApp.h"
#include "GUIAppDlg.h"
#include "mmsystem.h"
#include "Lib/Audio/Audio.hpp"
#include "Lib/Audio/AudioDaemon.hpp"
#include "Lib/Audio/Eax.h"
#include "Lib/sys/reg.h"
#include "Lib/sys/RegInit.hpp"



//*********************************************************************************************
//
// CDialogSoundProp implementation.
//


//*********************************************************************************************
//
// Message map for CDialogSoundProp.
//

//*********************************************************************************************
BEGIN_MESSAGE_MAP(CDialogSoundProp, CDialog)
	//{{AFX_MSG_MAP(CDialogSoundProp)
	ON_WM_HSCROLL()
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_SOUND_8BIT, On8Bit)
	ON_BN_CLICKED(IDC_SOUND_16BIT, On16Bit)
	ON_BN_CLICKED(IDC_SOUND_STEREO, OnStereo)
	ON_BN_CLICKED(IDC_SOUND_MONO, OnMono)

	ON_BN_CLICKED(IDC_SPEAKER_MONO, OnSpeakerMono)
	ON_BN_CLICKED(IDC_SPEAKER_STEREO, OnSpeakerStereo)
	ON_BN_CLICKED(IDC_SPEAKER_QUAD, OnSpeakerQuad)
	ON_BN_CLICKED(IDC_SPEAKER_SURROUND, OnSpeakerSurround)
	ON_BN_CLICKED(IDC_SPEAKER_PHONES, OnSpeakerPhones)

	ON_BN_CLICKED(IDC_CHECK2, OnDisableEffects)
	ON_BN_CLICKED(IDC_CHECK3, OnDisableVoices)
	ON_BN_CLICKED(IDC_CHECK4, OnDisableAmbient)
	ON_BN_CLICKED(IDC_CHECK5, OnDisableMusic)

	ON_BN_CLICKED(IDC_CHECK7,OnEnableSubtitles)
	ON_BN_CLICKED(IDC_CHECK_HW,OnSetHW)

	ON_BN_CLICKED(IDC_RADIO1, OnSetEnvironment)
	ON_BN_CLICKED(IDC_RADIO2, OnSetEnvironment)
	ON_BN_CLICKED(IDC_RADIO3, OnSetEnvironment)
	ON_BN_CLICKED(IDC_RADIO4, OnSetEnvironment)
	ON_BN_CLICKED(IDC_RADIO5, OnSetEnvironment)
	ON_BN_CLICKED(IDC_RADIO6, OnSetEnvironment)
	ON_BN_CLICKED(IDC_RADIO7, OnSetEnvironment)
	ON_BN_CLICKED(IDC_RADIO8, OnSetEnvironment)
	ON_BN_CLICKED(IDC_RADIO9, OnSetEnvironment)
	ON_BN_CLICKED(IDC_RADIO10, OnSetEnvironment)
	ON_BN_CLICKED(IDC_RADIO11, OnSetEnvironment)
	ON_BN_CLICKED(IDC_RADIO12, OnSetEnvironment)
	ON_BN_CLICKED(IDC_RADIO13, OnSetEnvironment)
	ON_BN_CLICKED(IDC_RADIO14, OnSetEnvironment)
	ON_BN_CLICKED(IDC_RADIO15, OnSetEnvironment)
	ON_BN_CLICKED(IDC_RADIO16, OnSetEnvironment)
	ON_BN_CLICKED(IDC_RADIO17, OnSetEnvironment)
	ON_BN_CLICKED(IDC_RADIO18, OnSetEnvironment)
	ON_BN_CLICKED(IDC_RADIO19, OnSetEnvironment)
	ON_BN_CLICKED(IDC_RADIO20, OnSetEnvironment)
	ON_BN_CLICKED(IDC_RADIO21, OnSetEnvironment)
	ON_BN_CLICKED(IDC_RADIO22, OnSetEnvironment)
	ON_BN_CLICKED(IDC_RADIO23, OnSetEnvironment)
	ON_BN_CLICKED(IDC_RADIO24, OnSetEnvironment)
	ON_BN_CLICKED(IDC_RADIO25, OnSetEnvironment)
	ON_BN_CLICKED(IDC_RADIO26, OnSetEnvironment)

	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


//*********************************************************************************************
//
// CDialogSoundProp constructor.
//

//*********************************************************************************************
CDialogSoundProp::CDialogSoundProp(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogSoundProp::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogSoundProp)
	iSubtitles = 0;
	strVolume = "";
	strDecay = "";
	strDamping = "";
	//}}AFX_DATA_INIT
}



//*********************************************************************************************
//
// CDialogSoundProp member functions.
//



//*********************************************************************************************
//
void CDialogSoundProp::DoDataExchange(CDataExchange* pDX)
//
// MFC-generated shit for static controls.
//
//**************************************
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogSoundProp)
	DDX_Control(pDX, IDC_SLIDER1, sliderFrequency);
	DDX_Radio(pDX, IDC_RADIO1, iEnvironment);
	DDX_Check(pDX, IDC_CHECK7, iSubtitles);
	DDX_Control(pDX, IDC_SLIDER2, sliderVolume);
	DDX_Control(pDX, IDC_SLIDER8, sliderDecay);
	DDX_Control(pDX, IDC_SLIDER9, sliderDamping);
	DDX_Text(pDX,IDC_VOLUME_TEXT, strVolume);
	DDX_Text(pDX,IDC_DECAY_TEXT, strDecay);
	DDX_Text(pDX,IDC_DAMPING_TEXT, strDamping);
	//}}AFX_DATA_MAP
}







//*********************************************************************************************
//
void CDialogSoundProp::OnHScroll
(
	UINT        nSBCode,
	UINT        nPos,
	CScrollBar* pScrollBar
)
//
// Responds to the WM_HSCROLL message by moving one or more scrollbars and updating colour
// information accordingly.
//
//**************************************
{
	// get the ID of the control so we can switch on it
	int					iscr_id=pScrollBar->GetDlgCtrlID();
	uint32				u4_freq;
	uint32				u4_format = CAudio::pcaAudio->u4GetFormat();

	char buf[128];

	// Call base class member function.
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);

	// after calling the default handler set the slider position
	nPos=((CSliderCtrl*)pScrollBar)->GetPos();

	// which slider have we clicked on, we have the position so do not care
	// about the scroll code
	switch (iscr_id)
	{
	case IDC_SLIDER1:			// frequency

		u4_freq = sliderFrequency.GetPos() + 5000;

		CAudio::pcaAudio->SetOutputFormat(u4_freq,
									u4_format & AU_FORMAT_STEREO?escSTEREO:escMONO,
									u4_format & AU_FORMAT_16BIT?esb16BIT:esb8BIT);

		SetText(u4_freq);
		break;

	case IDC_SLIDER2:			// reverb volume
		sprintf(buf,"Reverb Mix Volume: %.2f", (float)nPos / 100.0f);
		strVolume = buf;
		CAudio::pcaAudio->SetEAXReverbVolume((float)nPos / 100.0f);
		break;

	case IDC_SLIDER8:			// reverb decay
		sprintf(buf,"Reverb Decay Time: %.2f", (float)nPos / 100.0f);
		strDecay = buf;
		CAudio::pcaAudio->SetEAXReverbDecay((float)nPos / 100.0f);
		break;

	case IDC_SLIDER9:			// damping
		sprintf(buf,"Damping: %.2f", (float)nPos / 100.0f);
		strDamping = buf;
		CAudio::pcaAudio->SetEAXReverbDamping((float)nPos / 100.0f);
		break;
	}

	UpdateData(false);
}








//*********************************************************************************************
//
void CDialogSoundProp::OnShowWindow
(
	BOOL bShow,
	UINT nStatus
)
//
// Responds to the WM_SHOWWINDOW message by initializing dialog controls.
//
//**************************************
{
	uint32	u4_format = CAudio::pcaAudio->u4GetFormat();


	uint32	u4_feat = padAudioDaemon->u4GetFeatures();
	if (u4_feat & AUDIO_FEATURE_SUBTITLES)
	{
		iSubtitles = true;
	}
	else
	{
		iSubtitles = false;
	}

	// Call the base class member function.
	CDialog::OnShowWindow(bShow, nStatus);

	sliderFrequency.SetRange(0,(44100-5000),TRUE);
	sliderFrequency.SetPos((u4_format & AU_FORMAT_FREQ) - 5000);

	sliderVolume.SetRange(0,100,TRUE);
	sliderDecay.SetRange(10,2000,TRUE);
	sliderDamping.SetRange(0,200,TRUE);

	if (u4_format & AU_FORMAT_16BIT)
	{
		((CButton*)GetDlgItem(IDC_SOUND_8BIT))->SetCheck(false);
		((CButton*)GetDlgItem(IDC_SOUND_16BIT))->SetCheck(true);
	}
	else
	{
		((CButton*)GetDlgItem(IDC_SOUND_8BIT))->SetCheck(true);
		((CButton*)GetDlgItem(IDC_SOUND_16BIT))->SetCheck(false);
	}

	if (u4_format & AU_FORMAT_STEREO)
	{
		((CButton*)GetDlgItem(IDC_SOUND_MONO))->SetCheck(false);
		((CButton*)GetDlgItem(IDC_SOUND_STEREO))->SetCheck(true);
	}
	else
	{
		((CButton*)GetDlgItem(IDC_SOUND_MONO))->SetCheck(true);
		((CButton*)GetDlgItem(IDC_SOUND_STEREO))->SetCheck(false);
	}


	UpdateSpeakerIcons();

	SetIconState( CAudio::pcaAudio->bAudioActive() );

	SetText(u4_format & AU_FORMAT_FREQ);

	if (CAudio::pcaAudio->bHardwareFound())
	{
		GetDlgItem(IDC_A3D_HARDWARE)->SetWindowText("Yes");
		GetDlgItem(IDC_CHECK_HW)->EnableWindow(true);
		((CButton*)GetDlgItem(IDC_CHECK_HW))->SetCheck(CAudio::pcaAudio->bUsingHardware());
	}
	else
	{
		GetDlgItem(IDC_A3D_HARDWARE)->SetWindowText("No");
		GetDlgItem(IDC_CHECK_HW)->EnableWindow(false);
		((CButton*)GetDlgItem(IDC_CHECK_HW))->SetCheck(false);
	}

	SAudioEnvironment	aenv;

	aenv = CAudio::pcaAudio->aenvGetEnvironment();
	if (aenv.u4Environment == -1)
	{
		iEnvironment = 0;
		GetDlgItem(IDC_RADIO1)->EnableWindow(false);
		GetDlgItem(IDC_RADIO2)->EnableWindow(false);
		GetDlgItem(IDC_RADIO3)->EnableWindow(false);
		GetDlgItem(IDC_RADIO4)->EnableWindow(false);
		GetDlgItem(IDC_RADIO5)->EnableWindow(false);
		GetDlgItem(IDC_RADIO6)->EnableWindow(false);
		GetDlgItem(IDC_RADIO7)->EnableWindow(false);
		GetDlgItem(IDC_RADIO8)->EnableWindow(false);
		GetDlgItem(IDC_RADIO9)->EnableWindow(false);
		GetDlgItem(IDC_RADIO10)->EnableWindow(false);
		GetDlgItem(IDC_RADIO11)->EnableWindow(false);
		GetDlgItem(IDC_RADIO12)->EnableWindow(false);
		GetDlgItem(IDC_RADIO13)->EnableWindow(false);
		GetDlgItem(IDC_RADIO14)->EnableWindow(false);
		GetDlgItem(IDC_RADIO15)->EnableWindow(false);
		GetDlgItem(IDC_RADIO16)->EnableWindow(false);
		GetDlgItem(IDC_RADIO17)->EnableWindow(false);
		GetDlgItem(IDC_RADIO18)->EnableWindow(false);
		GetDlgItem(IDC_RADIO19)->EnableWindow(false);
		GetDlgItem(IDC_RADIO20)->EnableWindow(false);
		GetDlgItem(IDC_RADIO21)->EnableWindow(false);
		GetDlgItem(IDC_RADIO22)->EnableWindow(false);
		GetDlgItem(IDC_RADIO23)->EnableWindow(false);
		GetDlgItem(IDC_RADIO24)->EnableWindow(false);
		GetDlgItem(IDC_RADIO25)->EnableWindow(false);
		GetDlgItem(IDC_RADIO26)->EnableWindow(false);
		GetDlgItem(IDC_SLIDER2)->EnableWindow(false);
		GetDlgItem(IDC_SLIDER8)->EnableWindow(false);
		GetDlgItem(IDC_SLIDER9)->EnableWindow(false);
	}

	iEnvironment = aenv.u4Environment;

	sliderVolume.SetPos( (int32)(aenv.fVolume*100.0f) );
	sliderDecay.SetPos( (int32)(aenv.fDecayTime*100.0f) );
	sliderDamping.SetPos( (int32)(aenv.fDamping*100.0f) );

	char buf[128];

	sprintf(buf,"Reverb Mix Volume: %.2f", aenv.fVolume);
	strVolume = buf;
	sprintf(buf,"Reverb Decay Time: %.2f", aenv.fDecayTime);
	strDecay = buf;
	sprintf(buf,"Damping: %.2f", aenv.fDamping);
	strDamping = buf;

	UpdateData(false);
}


//*********************************************************************************************
//
void CDialogSoundProp::On8Bit
(
)
//
// sets the output to 8 bit
//
//**************************************
{
	uint32 u4_format = CAudio::pcaAudio->u4GetFormat();

	CAudio::pcaAudio->SetOutputFormat(u4_format & AU_FORMAT_FREQ,
								u4_format & AU_FORMAT_STEREO?escSTEREO:escMONO,
								esb8BIT);
}



//*********************************************************************************************
//
void CDialogSoundProp::On16Bit
(
)
//
// stes the output to 16 bit
//
//**************************************
{
	uint32 u4_format = CAudio::pcaAudio->u4GetFormat();

	CAudio::pcaAudio->SetOutputFormat(u4_format & AU_FORMAT_FREQ,
								u4_format & AU_FORMAT_STEREO?escSTEREO:escMONO,
								esb16BIT);
}



//*********************************************************************************************
//
void CDialogSoundProp::OnMono
(
)
//
// sets the output to mono
//
//**************************************
{
	uint32 u4_format = CAudio::pcaAudio->u4GetFormat();

	CAudio::pcaAudio->SetOutputFormat(u4_format & AU_FORMAT_FREQ,
								escMONO,
								u4_format & AU_FORMAT_16BIT?esb16BIT:esb8BIT);
}



//*********************************************************************************************
//
void CDialogSoundProp::OnStereo
(
)
//
// sets the output to stereo
//
//**************************************
{
	uint32 u4_format = CAudio::pcaAudio->u4GetFormat();

	CAudio::pcaAudio->SetOutputFormat(u4_format & AU_FORMAT_FREQ,
								escSTEREO,
								u4_format & AU_FORMAT_16BIT?esb16BIT:esb8BIT);
}




//*********************************************************************************************
//
void CDialogSoundProp::SetText
(
	uint32	u4_freq
)
//
// Sets all the text in the window
//
//**************************************
{
	char	buf[128];
	
	wsprintf(buf,"Frequency = %d",u4_freq);
	GetDlgItem(IDC_TEXT_FREQ)->SetWindowText(buf);
}



//*********************************************************************************************
//
void CDialogSoundProp::SetIconState
(
	bool	b_enabled
)
//
// sets the state of the sound dialog icons
//
//**************************************
{
	GetDlgItem(IDC_SOUND_MONO)->EnableWindow(b_enabled);
	GetDlgItem(IDC_SOUND_STEREO)->EnableWindow(b_enabled);
	GetDlgItem(IDC_SOUND_8BIT)->EnableWindow(b_enabled);
	GetDlgItem(IDC_SOUND_16BIT)->EnableWindow(b_enabled);
	GetDlgItem(IDC_SLIDER1)->EnableWindow(b_enabled);

	GetDlgItem(IDC_SPEAKER_MONO)->EnableWindow(b_enabled);
	GetDlgItem(IDC_SPEAKER_STEREO)->EnableWindow(b_enabled);
	GetDlgItem(IDC_SPEAKER_QUAD)->EnableWindow(b_enabled);
	GetDlgItem(IDC_SPEAKER_SURROUND)->EnableWindow(b_enabled);
	GetDlgItem(IDC_SPEAKER_PHONES)->EnableWindow(b_enabled);

	GetDlgItem(IDC_CHECK2)->EnableWindow(b_enabled);
	GetDlgItem(IDC_CHECK3)->EnableWindow(b_enabled);
	GetDlgItem(IDC_CHECK4)->EnableWindow(b_enabled);
	GetDlgItem(IDC_CHECK5)->EnableWindow(b_enabled);

	GetDlgItem(IDC_RADIO1)->EnableWindow(b_enabled & CAudio::pcaAudio->bEAXPresent() );
	GetDlgItem(IDC_RADIO2)->EnableWindow(b_enabled & CAudio::pcaAudio->bEAXPresent() );
	GetDlgItem(IDC_RADIO3)->EnableWindow(b_enabled & CAudio::pcaAudio->bEAXPresent() );
	GetDlgItem(IDC_RADIO4)->EnableWindow(b_enabled & CAudio::pcaAudio->bEAXPresent() );
	GetDlgItem(IDC_RADIO5)->EnableWindow(b_enabled & CAudio::pcaAudio->bEAXPresent() );
	GetDlgItem(IDC_RADIO6)->EnableWindow(b_enabled & CAudio::pcaAudio->bEAXPresent() );
	GetDlgItem(IDC_RADIO7)->EnableWindow(b_enabled & CAudio::pcaAudio->bEAXPresent() );
	GetDlgItem(IDC_RADIO8)->EnableWindow(b_enabled & CAudio::pcaAudio->bEAXPresent() );
	GetDlgItem(IDC_RADIO9)->EnableWindow(b_enabled & CAudio::pcaAudio->bEAXPresent() );
	GetDlgItem(IDC_RADIO10)->EnableWindow(b_enabled & CAudio::pcaAudio->bEAXPresent() );
	GetDlgItem(IDC_RADIO11)->EnableWindow(b_enabled & CAudio::pcaAudio->bEAXPresent() );
	GetDlgItem(IDC_RADIO12)->EnableWindow(b_enabled & CAudio::pcaAudio->bEAXPresent() );
	GetDlgItem(IDC_RADIO13)->EnableWindow(b_enabled & CAudio::pcaAudio->bEAXPresent() );
	GetDlgItem(IDC_RADIO14)->EnableWindow(b_enabled & CAudio::pcaAudio->bEAXPresent() );
	GetDlgItem(IDC_RADIO15)->EnableWindow(b_enabled & CAudio::pcaAudio->bEAXPresent() );
	GetDlgItem(IDC_RADIO16)->EnableWindow(b_enabled & CAudio::pcaAudio->bEAXPresent() );
	GetDlgItem(IDC_RADIO17)->EnableWindow(b_enabled & CAudio::pcaAudio->bEAXPresent() );
	GetDlgItem(IDC_RADIO18)->EnableWindow(b_enabled & CAudio::pcaAudio->bEAXPresent() );
	GetDlgItem(IDC_RADIO19)->EnableWindow(b_enabled & CAudio::pcaAudio->bEAXPresent() );
	GetDlgItem(IDC_RADIO20)->EnableWindow(b_enabled & CAudio::pcaAudio->bEAXPresent() );
	GetDlgItem(IDC_RADIO21)->EnableWindow(b_enabled & CAudio::pcaAudio->bEAXPresent() );
	GetDlgItem(IDC_RADIO22)->EnableWindow(b_enabled & CAudio::pcaAudio->bEAXPresent() );
	GetDlgItem(IDC_RADIO23)->EnableWindow(b_enabled & CAudio::pcaAudio->bEAXPresent() );
	GetDlgItem(IDC_RADIO24)->EnableWindow(b_enabled & CAudio::pcaAudio->bEAXPresent() );
	GetDlgItem(IDC_RADIO25)->EnableWindow(b_enabled & CAudio::pcaAudio->bEAXPresent() );
	GetDlgItem(IDC_RADIO26)->EnableWindow(b_enabled & CAudio::pcaAudio->bEAXPresent() );

	GetDlgItem(IDC_SLIDER2)->EnableWindow(b_enabled & CAudio::pcaAudio->bEAXPresent() );
	GetDlgItem(IDC_SLIDER8)->EnableWindow(b_enabled & CAudio::pcaAudio->bEAXPresent() );
	GetDlgItem(IDC_SLIDER9)->EnableWindow(b_enabled & CAudio::pcaAudio->bEAXPresent() );
}


//*********************************************************************************************
//
void CDialogSoundProp::OnSpeakerMono()
{
	CAudio::pcaAudio->SetSpeakerConfig(esconMONO);
	UpdateSpeakerIcons();
}

//*********************************************************************************************
//
void CDialogSoundProp::OnSpeakerStereo()
{
	CAudio::pcaAudio->SetSpeakerConfig(esconSTEREO);
	UpdateSpeakerIcons();
}

//*********************************************************************************************
//
void CDialogSoundProp::OnSpeakerQuad()
{
	CAudio::pcaAudio->SetSpeakerConfig(esconQUAD);
	UpdateSpeakerIcons();
}

//*********************************************************************************************
//
void CDialogSoundProp::OnSpeakerSurround()
{
	CAudio::pcaAudio->SetSpeakerConfig(esconSURROUND);
	UpdateSpeakerIcons();
}

//*********************************************************************************************
//
void CDialogSoundProp::OnSpeakerPhones()
{
	CAudio::pcaAudio->SetSpeakerConfig(esconHEADPHONES);
	UpdateSpeakerIcons();
}



void CDialogSoundProp::UpdateSpeakerIcons()
{
	((CButton*)GetDlgItem(IDC_SPEAKER_MONO))->SetCheck(false);
	((CButton*)GetDlgItem(IDC_SPEAKER_STEREO))->SetCheck(false);
	((CButton*)GetDlgItem(IDC_SPEAKER_QUAD))->SetCheck(false);
	((CButton*)GetDlgItem(IDC_SPEAKER_SURROUND))->SetCheck(false);
	((CButton*)GetDlgItem(IDC_SPEAKER_PHONES))->SetCheck(false);

	((CButton*)GetDlgItem(IDC_CHECK2))->SetCheck(false);
	((CButton*)GetDlgItem(IDC_CHECK3))->SetCheck(false);
	((CButton*)GetDlgItem(IDC_CHECK4))->SetCheck(false);
	((CButton*)GetDlgItem(IDC_CHECK5))->SetCheck(false);

	switch (CAudio::pcaAudio->esconGetSpeakerConfig())
	{
	case esconMONO:
		((CButton*)GetDlgItem(IDC_SPEAKER_MONO))->SetCheck(true);
		break;

	case esconSTEREO:
		((CButton*)GetDlgItem(IDC_SPEAKER_STEREO))->SetCheck(true);
		break;

	case esconQUAD:
		((CButton*)GetDlgItem(IDC_SPEAKER_QUAD))->SetCheck(true);
		break;

	case esconSURROUND:
		((CButton*)GetDlgItem(IDC_SPEAKER_SURROUND))->SetCheck(true);
		break;

	case esconHEADPHONES:
		((CButton*)GetDlgItem(IDC_SPEAKER_PHONES))->SetCheck(true);
		break;
	}

	uint32	u4_feat = padAudioDaemon->u4GetFeatures();

	if (!(u4_feat & AUDIO_FEATURE_VOICEOVER))
	{
		((CButton*)GetDlgItem(IDC_CHECK3))->SetCheck(true);
	}

	if (!(u4_feat & AUDIO_FEATURE_EFFECT))
	{
		((CButton*)GetDlgItem(IDC_CHECK2))->SetCheck(true);
	}

	if (!(u4_feat & AUDIO_FEATURE_MUSIC))
	{
		((CButton*)GetDlgItem(IDC_CHECK5))->SetCheck(true);
	}

	if (!(u4_feat & AUDIO_FEATURE_AMBIENT))
	{
		((CButton*)GetDlgItem(IDC_CHECK4))->SetCheck(true);
	}
}


void CDialogSoundProp::OnDisableEffects()
{
	if ( ((CButton*)GetDlgItem(IDC_CHECK2))->GetCheck() )
	{
		// feature is enabled so disable it
		padAudioDaemon->SetFeatures(AUDIO_FEATURE_EFFECT, 0);

	}
	else
	{
		// feature is disabled so enable it
		padAudioDaemon->SetFeatures(AUDIO_FEATURE_EFFECT, AUDIO_FEATURE_EFFECT);
	}

	UpdateSpeakerIcons();
}


void CDialogSoundProp::OnDisableVoices()
{
	if ( ((CButton*)GetDlgItem(IDC_CHECK3))->GetCheck() )
	{
		// feature is enabled so disable it
		padAudioDaemon->SetFeatures(AUDIO_FEATURE_VOICEOVER, 0);

	}
	else
	{
		// feature is disabled so enable it
		padAudioDaemon->SetFeatures(AUDIO_FEATURE_VOICEOVER, AUDIO_FEATURE_VOICEOVER);
	}

	UpdateSpeakerIcons();
}

void CDialogSoundProp::OnDisableAmbient()
{
	if ( ((CButton*)GetDlgItem(IDC_CHECK4))->GetCheck() )
	{
		// feature is enabled so disable it
		padAudioDaemon->SetFeatures(AUDIO_FEATURE_AMBIENT, 0);

	}
	else
	{
		// feature is disabled so enable it
		padAudioDaemon->SetFeatures(AUDIO_FEATURE_AMBIENT, AUDIO_FEATURE_AMBIENT);
	}

	UpdateSpeakerIcons();
}

void CDialogSoundProp::OnDisableMusic()
{
	if ( ((CButton*)GetDlgItem(IDC_CHECK5))->GetCheck() )
	{
		// feature is enabled so disable it
		padAudioDaemon->SetFeatures(AUDIO_FEATURE_MUSIC, 0);

	}
	else
	{
		// feature is disabled so enable it
		padAudioDaemon->SetFeatures(AUDIO_FEATURE_MUSIC, AUDIO_FEATURE_MUSIC);
	}

	UpdateSpeakerIcons();
}




void CDialogSoundProp::OnEnableSubtitles()
{
	UpdateData(true);

	if (iSubtitles)
	{
		padAudioDaemon->SetFeatures(AUDIO_FEATURE_SUBTITLES, AUDIO_FEATURE_SUBTITLES);
	}
	else
	{
		padAudioDaemon->SetFeatures(AUDIO_FEATURE_SUBTITLES, 0);
	}
}



void CDialogSoundProp::OnSetHW()
{
	bool b_hw = ((CButton*)GetDlgItem(IDC_CHECK_HW))->GetCheck();
	CAudio::pcaAudio->UseHardware(b_hw);
	SetRegValue(REG_KEY_AUDIO_ENABLE3D, (int)b_hw);
}


void CDialogSoundProp::OnSetEnvironment()
{
	UpdateData(true);

	// set the new environment
	CAudio::pcaAudio->SetEAXEnvironment((uint32)iEnvironment);

	// get the new settings
	SAudioEnvironment	aenv;
	aenv = CAudio::pcaAudio->aenvGetEnvironment();

	sliderVolume.SetPos( (int32)(aenv.fVolume*100.0f) );
	sliderDecay.SetPos( (int32)(aenv.fDecayTime*100.0f) );
	sliderDamping.SetPos( (int32)(aenv.fDamping*100.0f) );

	char buf[128];

	sprintf(buf,"Reverb Mix Volume: %.2f", aenv.fVolume);
	strVolume = buf;
	sprintf(buf,"Reverb Decay Time: %.2f", aenv.fDecayTime);
	strDecay = buf;
	sprintf(buf,"Damping: %.2f", aenv.fDamping);
	strDamping = buf;

	UpdateData(false);
}