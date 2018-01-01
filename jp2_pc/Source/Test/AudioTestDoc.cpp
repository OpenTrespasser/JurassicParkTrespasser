// AudioTestDoc.cpp : implementation of the CAudioTestDoc class
//

#include "stdafx.h"
#include "AudioTest.h"

#include <assert.h>

#include <yvals.h>

#include "lib/audio/audio_lib.hpp"

#include "AudioTestDoc.h"
#include "AttribDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAudioTestDoc

IMPLEMENT_DYNCREATE(CAudioTestDoc, CDocument)

BEGIN_MESSAGE_MAP(CAudioTestDoc, CDocument)
	//{{AFX_MSG_MAP(CAudioTestDoc)
	ON_COMMAND(ID_AUDIO_CREATESCENE, OnAudioCreatescene)
	ON_COMMAND(ID_AUDIO_ADDLISTENER, OnAudioAddListener)
	ON_UPDATE_COMMAND_UI(ID_AUDIO_ADDLISTENER, OnUpdateAudioAddListener)
	ON_UPDATE_COMMAND_UI(ID_AUDIO_DELETESCENE, OnUpdateAudioDeleteScene)
	ON_COMMAND(ID_AUDIO_DELETESCENE, OnAudioDeleteScene)
	ON_COMMAND(ID_CONTROL_PLAY, OnControlPlay)
	ON_COMMAND(ID_CONTROL_STOP, OnControlStop)
	ON_UPDATE_COMMAND_UI(ID_CONTROL_PLAY, OnUpdateControlPlay)
	ON_UPDATE_COMMAND_UI(ID_CONTROL_STOP, OnUpdateControlStop)
	ON_COMMAND(ID_CONTROL_ZOOMDOWN, OnControlZoomDown)
	ON_COMMAND(ID_CONTROL_ZOOMUP, OnControlZoomUp)
	ON_COMMAND(ID_AUDIO_ENVIRONMENT, OnAudioEnvironment)
	ON_UPDATE_COMMAND_UI(ID_AUDIO_ENVIRONMENT, OnUpdateAudioEnvironment)
	ON_COMMAND(ID_AUDIO_GAME_ADDSOUND, OnAudioGameAddsound)
	ON_UPDATE_COMMAND_UI(ID_AUDIO_GAME_ADDSOUND, OnUpdateAudioGameAddsound)
	ON_UPDATE_COMMAND_UI(ID_AUDIO_CREATESCENE, OnUpdateAudioCreatescene)
	ON_COMMAND(ID_AUDIO_MUSIC_ADDSOUND, OnAudioMusicAddsound)
	ON_UPDATE_COMMAND_UI(ID_AUDIO_MUSIC_ADDSOUND, OnUpdateAudioMusicAddsound)
	ON_COMMAND(ID_AUDIO_ENVIRONMENTALLOOPS_ADDSOUND, OnAudioEnvironmentalloopsAddsound)
	ON_UPDATE_COMMAND_UI(ID_AUDIO_ENVIRONMENTALLOOPS_ADDSOUND, OnUpdateAudioEnvironmentalloopsAddsound)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAudioTestDoc construction/destruction

CAudioTestDoc::CAudioTestDoc()
{
	// TODO: add one-time construction code here
	pscene = 0;
}

CAudioTestDoc::~CAudioTestDoc()
{
	if (pscene)
		delete pscene;
}

BOOL CAudioTestDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	init_document();
	// (SDI documents will reuse this document)

	return TRUE;
}

void CAudioTestDoc :: init_document()
{
	scale = 8;
	xmin = ymin = -5;
	xmax = ymax = 5;
	x_displacement = -xmin*scale;
	y_displacement = -ymin*scale;
	size.cx = (xmax-xmin)*scale;
	size.cy = (ymax-ymin)*scale;
}

/////////////////////////////////////////////////////////////////////////////
// CAudioTestDoc serialization

void CAudioTestDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CAudioTestDoc diagnostics

#ifdef _DEBUG
void CAudioTestDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CAudioTestDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CAudioTestDoc commands

void CAudioTestDoc::OnAudioCreatescene() 
{
	HWND hwnd =	AfxGetMainWnd ()->GetSafeHwnd ();
	pscene = new CAuScene (hwnd);

	Assert (pscene);

	env_loop = 0;
}

void CAudioTestDoc::OnAudioDeleteScene() 
{
	if (pscene)
	{
		delete pscene;
		pscene = 0;
	}

	UpdateAllViews (NULL);
}

void CAudioTestDoc::OnAudioAddListener() 
{
	if (!pscene)
		return;
	
	CVector3<float> v3Position (0.0f,0.0f,0.0f);
	CVector3<float>	v3Direction (0.0f,0.0f,1.0f);
	CVector3<float> v3Up (0.0f,1.0f,0.0f);

	CListenerAttribDlg		ladlg;
	
	ladlg.m_dirx =		v3Direction.tX;
	ladlg.m_diry =		v3Direction.tY;
	ladlg.m_dirz =		v3Direction.tZ;
	ladlg.m_upx =		v3Up.tX;
	ladlg.m_upy =		v3Up.tY;
	ladlg.m_upz =		v3Up.tZ;
	ladlg.m_posx =		v3Position.tX;
	ladlg.m_posy =		v3Position.tY;
	ladlg.m_posz =		v3Position.tZ;

	if (ladlg.DoModal () == IDOK)
	{
		v3Position.tX =	ladlg.m_posx;
		v3Position.tY =	ladlg.m_posy;
		v3Position.tZ =	ladlg.m_posz;
		v3Direction.tX =	ladlg.m_dirx;
		v3Direction.tY =	ladlg.m_diry;
		v3Direction.tZ =	ladlg.m_dirz;
		v3Up.tX =		ladlg.m_upx;
		v3Up.tY =		ladlg.m_upy;
		v3Up.tZ =		ladlg.m_upz;

		CAuListener * plistener = pscene->CreateListener ();
		plistener->SetPosition (v3Position);
		plistener->SetOrientation (v3Direction, v3Up);

		// update windows sizes and redraw
		update_logical_coords ((int)v3Position.tX, (int)v3Position.tZ);
		UpdateAllViews (NULL);
	}
}


void CAudioTestDoc::OnUpdateAudioDeleteScene(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable (pscene != 0);
}

void CAudioTestDoc::OnUpdateAudioAddListener(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable (pscene != 0);
}


void CAudioTestDoc::OnControlPlay() 
{
	Assert (pscene);
	pscene->Play (0);
}

void CAudioTestDoc::OnControlStop() 
{
	Assert (pscene);
	pscene->Stop ();
}

void CAudioTestDoc::OnUpdateControlPlay(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable (pscene != 0);
}

void CAudioTestDoc::OnUpdateControlStop(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable (pscene != 0);
}

void CAudioTestDoc:: control_listener() 
{
	Assert (pscene && pscene->GetListener ());
	
	CVector3<float>	v3Position;
	CVector3<float>	v3Direction;
	CVector3<float>	v3Up;
	pscene->GetListener()->GetPosition (&v3Position);
	pscene->GetListener()->GetOrientation (&v3Direction, &v3Up);

	CListenerAttribDlg	listener_dlg;
	listener_dlg.m_posx	= v3Position.tX;
	listener_dlg.m_posy	= v3Position.tY;
	listener_dlg.m_posz	= v3Position.tZ;
	listener_dlg.m_dirx	= v3Direction.tX;
	listener_dlg.m_diry	= v3Direction.tY;
	listener_dlg.m_dirz	= v3Direction.tZ;
	listener_dlg.m_upx	= v3Up.tX;
	listener_dlg.m_upy	= v3Up.tY;
	listener_dlg.m_upz	= v3Up.tZ;

	if (listener_dlg.DoModal () == IDOK)
	{
		v3Position.tX = listener_dlg.m_posx;
		v3Position.tY = listener_dlg.m_posy;
		v3Position.tZ = listener_dlg.m_posz;
		v3Direction.tX = listener_dlg.m_dirx;
		v3Direction.tY = listener_dlg.m_diry;
		v3Direction.tZ = listener_dlg.m_dirz;
		v3Up.tX = listener_dlg.m_upx;
		v3Up.tY = listener_dlg.m_upy;
		v3Up.tZ = listener_dlg.m_upz;
		pscene->GetListener()->SetPosition( v3Position );
		pscene->GetListener()->SetOrientation( v3Direction, v3Up );

		update_logical_coords ((int)v3Position.tX, (int)v3Position.tZ);
		UpdateAllViews (NULL);
	}

}

void CAudioTestDoc :: control_sound (CAuSound * psound)
{
	Assert (pscene);
	
	CAuSound * pos_sound = (CAuSound *)psound;
	
	SAuSoundModel	model;
	pos_sound->GetModel( &model );

	CVector3<float>	pos;
	CVector3<float>	dir;
	pos_sound->GetPosition( &pos );
	pos_sound->GetOrientation( &dir );

	// set dialog
	CSoundAttribDlg	sound_attrib_dlg;
	
	sound_attrib_dlg.m_filename = psound->GetName ();
	
	sound_attrib_dlg.m_intensity = pos_sound->GetIntensity ();
	sound_attrib_dlg.m_maxback = model.fMaxBack;
	sound_attrib_dlg.m_maxfront = model.fMaxFront;
	sound_attrib_dlg.m_minback = model.fMinBack;
	sound_attrib_dlg.m_minfront = model.fMinFront;

	sound_attrib_dlg.m_orientx = dir.tX;
	sound_attrib_dlg.m_orienty = dir.tY;
	sound_attrib_dlg.m_orientz = dir.tZ;
	sound_attrib_dlg.m_posx = pos.tX;
	sound_attrib_dlg.m_posy = pos.tY;
	sound_attrib_dlg.m_posz = pos.tZ;

	sound_attrib_dlg.m_bControl = TRUE;
	if( sound_attrib_dlg.DoModal() == IDOK )
	{
		dir.tX = sound_attrib_dlg.m_orientx;
		dir.tY = sound_attrib_dlg.m_orienty;
		dir.tZ = sound_attrib_dlg.m_orientz;
		pos.tX = sound_attrib_dlg.m_posx;
		pos.tY = sound_attrib_dlg.m_posy;
		pos.tZ = sound_attrib_dlg.m_posz;
		pos_sound->SetOrientation( dir );
		pos_sound->SetPosition( pos );
	
		pos_sound->SetIntensity (sound_attrib_dlg.m_intensity);

		model.fMaxBack = sound_attrib_dlg.m_maxback;
		model.fMaxFront = sound_attrib_dlg.m_maxfront;
		model.fMinBack = sound_attrib_dlg.m_minback;
		model.fMinFront = sound_attrib_dlg.m_minfront;
		pos_sound->SetModel( model );

		update_logical_coords ((int)pos.tX, (int)pos.tZ);
		UpdateAllViews (NULL);
	}
	sound_attrib_dlg.m_bControl = FALSE;
}

void CAudioTestDoc::OnCloseDocument() 
{
	OnAudioDeleteScene ();
	
	CDocument::OnCloseDocument();
}

void CAudioTestDoc::OnControlZoomDown() 
{
	update_logical_coords (0.5f);
	UpdateAllViews (NULL);
}

void CAudioTestDoc::OnControlZoomUp() 
{
	update_logical_coords (2.0f);
	UpdateAllViews (NULL);
}


void CAudioTestDoc :: update_logical_coords (int x, int y)
{
	if (x < xmin+1) xmin = x-1;		// 1 logical unit buffer
	else if (x > xmax-1) xmax = x+1;
	if (y < ymin+1) ymin = y-1;
	else if (y > ymax-1) ymax = y+1;

	x_displacement = -xmin*scale;
	y_displacement = -ymin*scale;
	size.cx = (xmax-xmin)*scale;
	size.cy = (ymax-ymin)*scale;
}

void CAudioTestDoc :: update_logical_coords (float scale_factor)
{
	scale = (int)((float)scale*scale_factor);
	if (scale < 1) scale = 1;
	else if (scale > 64) scale = 64;
	
	x_displacement = -xmin*scale;
	y_displacement = -ymin*scale;
	size.cx = (xmax-xmin)*scale;
	size.cy = (ymax-ymin)*scale;
}

void CAudioTestDoc :: convert_dlc (float x_lc, float y_lc, int * px, int * py)
{
	x_lc *= scale;
	y_lc *= scale;
	*px = (int)x_lc + x_displacement;
	*py = (int)y_lc + y_displacement;
}

void CAudioTestDoc :: convert_lc (int x_dlc, int y_dlc, float * px, float * py)
{
	x_dlc -= x_displacement;
	y_dlc -= y_displacement;

	*px = (float)(x_dlc/scale);
	*py = (float)(y_dlc/scale);
}

void CAudioTestDoc::OnAudioEnvironment() 
{
	if (!pscene)
		return;
	
	CEnvAttribDlg dlg;

	ECPULocalization	ecpul;
	ecpul = pscene->GetCPUBudget ();
	dlg.m_cpulocbudget = ecpul - ecpul_NONE;

	dlg.m_speedofsound = pscene->GetSpeedOfSound ();
	
	SAuReverb reverb;
	pscene->GetReverb (&reverb);
	dlg.m_usereverb = reverb.bUseReverb;
	dlg.m_revdecay = reverb.fDecay;
	dlg.m_revintensity = reverb.fIntensity;

	if (dlg.DoModal () == IDOK)
	{
		ecpul = (ECPULocalization) (dlg.m_cpulocbudget + ecpul_NONE);
		pscene->SetCPUBudget (ecpul);

		pscene->SetSpeedOfSound (dlg.m_speedofsound);

		reverb.bUseReverb = dlg.m_usereverb;
		reverb.fDecay = dlg.m_revdecay;
		reverb.fIntensity = dlg.m_revintensity;
		pscene->SetReverb (reverb);
	}
}

void CAudioTestDoc::OnUpdateAudioEnvironment(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable (pscene != 0);

	return;
}

void CAudioTestDoc::OnAudioGameAddsound() 
{
	if (!pscene)
		return;
	
	CFileDialog fdlg (TRUE, _T ("wav"), _T ("*.wav"),
					  OFN_FILEMUSTEXIST|OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
					  _T ("Wave Files (*.wav)|*.wav|"));

	if (fdlg.DoModal () == IDOK)
	{
		CString	path = fdlg.GetPathName ();
		CString fname = fdlg.GetFileName ();
		
		// default sound model
		SAuSoundModel	ausm;
		ausm.fMinBack = 2.0f;
		ausm.fMinFront = 5.0f;
		ausm.fMaxBack = 6.0f;
		ausm.fMaxFront = 15.0f;
		
		// default orientation and position
		CVector3<float>	v3Orientation (0.0f,0.0f,1.0f);
		CVector3<float> v3Position (0.0f,0.0f,0.0f);

		CSoundAttribDlg		sadlg;

		// initialize dialog values with default values
		sadlg.m_fattenuate =	true;
		sadlg.m_fspatialize =	true;
		sadlg.m_fdoppler =	true;
		sadlg.m_freverb =	true;
		
		sadlg.m_filename =	CString (fname);
		sadlg.m_intensity =	1.0f;
		sadlg.m_minback =	ausm.fMinBack;
		sadlg.m_minfront =	ausm.fMinFront;
		sadlg.m_maxback =	ausm.fMaxBack;
		sadlg.m_maxfront =	ausm.fMaxFront;
		sadlg.m_posx =		v3Position.tX;
		sadlg.m_posy =		v3Position.tY;
		sadlg.m_posz =		v3Position.tZ;
		sadlg.m_orientx =	v3Orientation.tX;
		sadlg.m_orienty =	v3Orientation.tY;
		sadlg.m_orientz =	v3Orientation.tZ;
		
		sadlg.m_bControl = FALSE;
		if (sadlg.DoModal () == IDOK)
		{
			unsigned int flags = 0;
			if (!sadlg.m_fattenuate) flags |= SOUND_NOATTENUATE;
			if (!sadlg.m_fspatialize) flags |= SOUND_NOSPATIALIZE;
			if (!sadlg.m_fdoppler) flags |= SOUND_NODOPPLER;
			if (!sadlg.m_freverb) flags |= SOUND_NOREVERB;

			ausm.fMinBack =		sadlg.m_minback;
			ausm.fMinFront =	sadlg.m_minfront;
			ausm.fMaxBack =		sadlg.m_maxback;
			ausm.fMaxFront =	sadlg.m_maxfront;
			v3Position.tX =		sadlg.m_posx;
			v3Position.tY =		sadlg.m_posy;
			v3Position.tZ =		sadlg.m_posz;
			v3Orientation.tX =	sadlg.m_orientx;
			v3Orientation.tY =	sadlg.m_orienty;
			v3Orientation.tZ =	sadlg.m_orientz;

			CAuSound * pos_sound = pscene->CreatePositionalSound (path, flags);
			pos_sound->SetOrientation (v3Orientation);
			pos_sound->SetPosition (v3Position);
			pos_sound->SetModel (ausm);
			pos_sound->SetIntensity (sadlg.m_intensity);

			// update windows sizes and redraw
			update_logical_coords ((int)v3Position.tX, (int)v3Position.tZ);
			UpdateAllViews (NULL);
		}
	}
}

void CAudioTestDoc::OnUpdateAudioGameAddsound(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable (pscene != 0);
}


void CAudioTestDoc::OnUpdateAudioCreatescene(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable (pscene == 0);
}

void CAudioTestDoc::OnAudioMusicAddsound() 
{
	if (!pscene)
		return;
	
	CFileDialog fdlg (TRUE, _T ("wav"), _T ("*.wav"),
		OFN_FILEMUSTEXIST|OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
		_T ("Wave Files (*.wav)|*.wav|"));

	if (fdlg.DoModal () == IDOK)
	{
		CString	path = fdlg.GetPathName ();
		CString fname = fdlg.GetFileName ();
		
		CMusicSoundAttribDlg dlg;

		// initialize dialog values with default values
		dlg.m_fdoppler =	true;
		dlg.m_freverb =	true;
		
		if (dlg.DoModal () == IDOK)
		{
			unsigned int flags = 0;
			if (!dlg.m_fdoppler) flags |= SOUND_NODOPPLER;
			if (!dlg.m_freverb) flags |= SOUND_NOREVERB;

			CAuSound * mus_sound = pscene->CreateMusicSound (path, flags);
		}
	}
}

void CAudioTestDoc::OnUpdateAudioMusicAddsound(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable (pscene != 0);
}

void CAudioTestDoc::OnAudioEnvironmentalloopsAddsound() 
{
	if (!pscene)
		return;
	
	CFileDialog fdlg (TRUE, _T ("wav"), _T ("*.wav"),
		OFN_FILEMUSTEXIST|OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
		_T ("Wave Files (*.wav)|*.wav|"));

	if (fdlg.DoModal () == IDOK)
	{
		CString	path = fdlg.GetPathName ();
		CString fname = fdlg.GetFileName ();
		
		if (env_loop == 0)
			env_loop = pscene->CreateEnvironmentalSound (path);
		else
			env_loop->AddSound (path);
	}
}

void CAudioTestDoc::OnUpdateAudioEnvironmentalloopsAddsound(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable (pscene != 0);
}
