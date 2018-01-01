#include "Database.h"
#include <fstream.h>
#include <ctype.h>


bool bProcessLanguageFilename
(
	int iLang,
	CString& cstr_source, 
	CString& cstr_dest
);


//*********************************************************************************************
CEffectDatabase		edbEffects;
CCRC* CEffectDatabase::pcrcHash = NULL;

//*********************************************************************************************
CEffectDatabase::CEffectDatabase()
{
	pcrcHash = CCRC::pcrcCreateCCITT32();
}


//*********************************************************************************************
CEffectDatabase::~CEffectDatabase()
{
	delete pcrcHash;
}


//*********************************************************************************************
void CEffectDatabase::SaveBinary(const char* str_filename,int i_lang)
{
	char			buff[1024];
	SSampleFile		asf[4096];
	CString			acstr[4096];
	uint32			u4_idx=0;
	uint32			u4_fpos = 0;
	char*			str;
	const char*		str_sample;
	TFilePosHash	fph;
	char			str_log[MAX_PATH];
	HANDLE			h_file;
	uint32			u4_filepos;
	TCollisionMap::iterator k;

	Assert(str_filename);

	strcpy(str_log, str_filename);
	strcpy(str_log+strlen(str_log)-3,"txt");

	ofstream file(str_log, ios::out|ios::trunc);	// Open and truncate.
	Assert(file.is_open());
	
	h_file = CreateFile
	(
		(LPCTSTR)str_filename,	// Pointer to name of the file.
		GENERIC_WRITE,			// Access (read-write) mode.
		FILE_SHARE_WRITE,		// Share mode.
		0,						// Pointer to security descriptor.
		CREATE_ALWAYS,			// How to create.
		FILE_ATTRIBUTE_NORMAL,	// File attributes.
		0 						// Handle to file with attributes to copy.
	);

	if ( (file.is_open() == false) || h_file == INVALID_HANDLE_VALUE)
	{
		CloseHandle(h_file);
		MessageBox(NULL,"Cannot create packed audio file.", "Trespasser Sound Effect Editor",MB_OK|MB_ICONINFORMATION);
		return;
	}

	file << "Language\n";
	file << "--------\n";

	switch (i_lang)
	{
	case 0:
		file << "English\n\n";
		break;
	case 1:
		file << "French\n\n";
		break;
	case 2:
		file << "German\n\n";
		break;
	case 3:
		file << "Spanish\n\n";
		break;
	case 4:
		file << "Portugese\n\n";
		break;
	case 5:
		file << "Japanese\n\n";
		break;
	case 6:
		file << "Chinese\n\n";
		break;
	}

	file << "Unique Samples\n";
	file << "--------------\n";

	for (TEffectVector::iterator i = evEffects.begin(); i<evEffects.end(); ++i)
	{
		CString		cstr_langfile;

		(*i).bProcessed = bProcessLanguageFilename
		(
			i_lang,
			(*i).cstrFilename, 
			cstr_langfile
		);

		str = buff;
		str_sample = cstr_langfile;
		while (*str_sample)
		{
			if (*str_sample == '\\')
				*str = '/';
			else
			{
				*str = tolower(*str_sample);
			}
			str++;
			str_sample++;
		}
		*str = 0;

		CString			cstr = buff;
		SSampleFile*	psf = fph[cstr];
		SCAUHeader		cauheader;


		if ((*i).bProcessed)
		{
			uint32 u4_fl = u4FileSize(cstr_langfile);
			if (u4_fl == 0xffffffff)
			{
				// The language file was not found so we are using the english
				(*i).bLangFound = false;
				(*i).cstrFilenameUsed = (*i).cstrFilename;
			}
			else
			{
				(*i).bLangFound = true;
				(*i).cstrFilenameUsed = cstr_langfile;
			}
		}
		else
		{
			// we did not process the filename so we use the original
			(*i).cstrFilenameUsed = (*i).cstrFilename;
		}


		if (psf == NULL)
		{
			uint32 u4_flen = u4FileSize( (*i).cstrFilenameUsed );
			if (u4_flen == 0xffffffff)
			{
				char buf[1024];

				wsprintf(buf, "Identifer '%s' : Cannot locate sample '%s'.\nThis sample will be replaced by the 'MISSING' sample.", (*i).cstrID, (*i).cstrFilenameUsed );
				MessageBox(NULL,buf, "Trespasser Sound Effect Editor",MB_OK|MB_ICONERROR);

				file << "FILENAME ERROR:" << (*i).cstrFilenameUsed << ": NOT FOUND\n";

				(*i).cstrFilenameUsed = "Missing.cau";
				u4_flen = u4FileSize( (*i).cstrFilenameUsed );
				Assert(u4_flen != 0xffffffff);
			}
			else
			{
				file << "LANGUAGE FILENAME:" << cstr_langfile << "\n";

				if ((*i).bProcessed)
				{
					file << "PACK FILENAME    :" << (*i).cstrFilenameUsed << "\n";
				}
				else
				{
					if (i_lang==0)
					{
						file << "PACK FILENAME    :" << (*i).cstrFilenameUsed << "\n";
					}
					else
					{
						file << "PACK FILENAME    :" << (*i).cstrFilenameUsed << " Not considered for language translation\n";
					}
				}
				file << "LENGTH           :" << u4_flen << "\n";
			}

			HANDLE hfile = CreateFile
			(
				(LPCTSTR)(*i).cstrFilenameUsed,		// Pointer to name of the file.
				GENERIC_READ,			// Access (read-write) mode.
				FILE_SHARE_READ,		// SHARE mode.
				0,						// Pointer to security descriptor.
				OPEN_EXISTING,			// How to create.
				FILE_ATTRIBUTE_NORMAL,	// File attributes.
				0 						// Handle to file with attributes to copy.
			);


			if (hfile == INVALID_HANDLE_VALUE)
			{
				file << "FORMAT: Error reading header\n";
				memset(&cauheader,0,sizeof(SCAUHeader));
			}
			else
			{
				uint32		u4_bytes;

				// Read in file header to memory.
				ReadFile
				(
					hfile,							// Handle of file to read.
					(LPVOID)&cauheader,				// Address of buffer that receives data.
					sizeof(SCAUHeader),				// Number of bytes to read.
					(LPDWORD)&u4_bytes,				// Address of number of bytes read.
					(LPOVERLAPPED)0 				// Address of structure for data.
				);

				// Close the open file.
				CloseHandle(hfile);

				if (cauheader.u4Magic != 'ROBW')
				{
					file << "FORMAT: Not a CAU file\n";
				}
				else
				{
					file << "Sample Rate = " << cauheader.u4Frequency << ", Channels = " << (uint32)cauheader.u1Channels << ", Bits = " 
						<< (uint32)cauheader.u1Bits << ", Compression = ";
					switch (cauheader.u1Compression)
					{
					case 0:
						file << "NONE\n";
						break;

					case 1:
						file << "ADPCM\n";
						break;

					case 2:
						file << "VOICE\n";
						break;
					}
				}
			}

			if (((*i).bProcessed) && (!((*i).bLangFound)))
			{
				file << "DEFAULT LANGUAGE USED\n";
			}

			file << '\n';

			// copy the header for this sample into the index
			asf[u4_idx].cauheaderIndex = cauheader;

			asf[u4_idx].u4Hash = (*i).u4Hash;
			asf[u4_idx].u4CAUStart = u4_fpos;
			asf[u4_idx].u4CAULen = u4_flen;

			acstr[u4_idx] = (*i).cstrFilenameUsed;

			// This filename is not present in the map so add it and allocate space in the file for it.
			fph[cstr] = &asf[u4_idx];
			(*i).psfFile = &asf[u4_idx];
			u4_idx++;
			Assert(u4_idx<4096);
			u4_fpos += u4_flen;
		}
		else
		{
			(*i).psfFile = psf;
		}
	}

	SPackedAudioHeader		pah;
	uint32					u4_reloc_samples;
	uint32					u4_bytes;
	uint32					u4_count;


	file << "Samples Identifiers\n";
	file << "-------------------\n";

	// fill in the header
	pah.u4Version				= u4PACKED_AUDIO_VERSION;
	pah.u4Samples				= u4_idx;
	pah.u4SampleFileOffset		= sizeof(pah);
	pah.u4Collisions			= 0;
	pah.u4CollisionFileOffset	= 0;
	pah.u4Identifiers			= evEffects.size();


	if (!WriteFile
	(
		h_file,							// Handle to file to write to.
		&pah,			// Pointer to data to write to file.
		sizeof(SPackedAudioHeader),		// Number of bytes to write.
		(LPDWORD)&u4_bytes,				// Pointer to number of bytes written.
		NULL
	))
	{
		file << "ERROR: Cannot write to tpa file\n";
		goto error;
	}
	
	//
	// file offset where the samples start
	//
	u4_reloc_samples = pah.u4SampleFileOffset + (u4TotalEffects()*sizeof(SSampleFile));

	
	//
	// Save the sample identifiers
	//
	for (i = evEffects.begin(); i<evEffects.end(); ++i)
	{
		SSampleFile		sf;
		SCAUHeader		cauheader;



		Assert( (*i).psfFile );

		sf.u4Hash = u4Hash((*i).cstrID);
		sf.u4CAUStart = (*i).psfFile->u4CAUStart + u4_reloc_samples;
		sf.u4CAULen = (*i).psfFile->u4CAULen;

		HANDLE hfile = CreateFile
		(
			(LPCTSTR)(*i).cstrFilenameUsed,		// Pointer to name of the file.
			GENERIC_READ,			// Access (read-write) mode.
			FILE_SHARE_READ,		// SHARE mode.
			0,						// Pointer to security descriptor.
			OPEN_EXISTING,			// How to create.
			FILE_ATTRIBUTE_NORMAL,	// File attributes.
			0 						// Handle to file with attributes to copy.
		);


		if (hfile == INVALID_HANDLE_VALUE)
		{
			file << "FORMAT: Error reading header\n";
			memset(&cauheader,0,sizeof(SCAUHeader));
		}
		else
		{
			uint32		u4_bytes;

			// Read in file header to memory, again
			ReadFile
			(
				hfile,							// Handle of file to read.
				(LPVOID)&cauheader,				// Address of buffer that receives data.
				sizeof(SCAUHeader),				// Number of bytes to read.
				(LPDWORD)&u4_bytes,				// Address of number of bytes read.
				(LPOVERLAPPED)0 				// Address of structure for data.
			);

			// Close the open file.
			CloseHandle(hfile);

		}

		// copy the header
		sf.cauheaderIndex = cauheader;

		// new for version 0x140
		sf.fAttenuation = (*i).fAttenuation;
		sf.fMasterVolume = (*i).fMasterVolume;

		wsprintf(buff,"%x", (*i).u4Hash);

		file << "IDENTIFIER: '" << (*i).cstrID << "' [" << buff << "] - " << (*i).cstrFilenameUsed << '\n';

		if (!WriteFile
		(
			h_file,
			&sf,
			sizeof(SSampleFile),
			(LPDWORD)&u4_bytes,
			NULL
		))
		{
			goto error;
		}
	}


	//
	// Save the samples
	//
	for (u4_count = 0; u4_count<u4_idx; u4_count++)
	{
		char*	pch = new char[asf[u4_count].u4CAULen+4096];

		LoadFile(acstr[u4_count],pch);
		if (!WriteFile
		(
			h_file,							// Handle to file to write to.
			pch,							// Pointer to data to write to file.
			asf[u4_count].u4CAULen,			// Number of bytes to write.
			(LPDWORD)&u4_bytes,				// Pointer to number of bytes written.
			NULL
		))
		{
			goto error;
		}

		delete pch;
	}

	// Get the current position in the file.
	u4_filepos = SetFilePointer(h_file, 0, NULL, FILE_CURRENT);


	file << "\nMaterial Collisions\n";
	file << "-------------------\n";
	//
	// now lets write out the collisions....
	//
	SFileCollision		fcol;

	for (k = cmCollisions.begin(); k!=cmCollisions.end(); ++k)
	{		
		file << "COLLISION: \n";

		fcol.u8Key							= (*k).second.u8Key;
		fcol.colCollision.u4SampleFlags		= (uint32)(*k).second.u1Samples;
		fcol.colCollision.fMinTimeDelay		= (*k).second.fMinTimeDelay;
		fcol.colCollision.fTimeLastUsed		= 0.0f;

		fcol.colCollision.sndhndSamples[0]	= (*k).second.csmList[0].u4SampleID;
		fcol.colCollision.sndhndSamples[1]	= (*k).second.csmList[1].u4SampleID;
		fcol.colCollision.sndhndSamples[2]	= (*k).second.csmSlide.u4SampleID;
		memcpy(&fcol.colCollision.stTransfer[0], &(*k).second.csmList[0].stTransfer, sizeof(SSoundTransfer));
		memcpy(&fcol.colCollision.stTransfer[1], &(*k).second.csmList[1].stTransfer, sizeof(SSoundTransfer));
		memcpy(&fcol.colCollision.stTransfer[2], &(*k).second.csmSlide.stTransfer, sizeof(SSoundTransfer));

		if (!WriteFile
		(
			h_file,							// Handle to file to write to.
			&fcol,							// Pointer to data to write to file.
			sizeof(SFileCollision),			// Number of bytes to write.
			(LPDWORD)&u4_bytes,				// Pointer to number of bytes written.
			NULL
		))
		{
			file << "ERROR: Cannot write collision infomation \n";
			goto error;
		}

	}


	// Set the file point back to the start
	SetFilePointer(h_file, 0, NULL, FILE_BEGIN);

	//
	// fill in the header with the prooper collision information,
	//
	pah.u4Collisions			= u4TotalCollisions();
	pah.u4CollisionFileOffset	= u4_filepos;


	if (!WriteFile
	(
		h_file,							// Handle to file to write to.
		&pah,			// Pointer to data to write to file.
		sizeof(SPackedAudioHeader),		// Number of bytes to write.
		(LPDWORD)&u4_bytes,				// Pointer to number of bytes written.
		NULL
	))
	{
		file << "ERROR: Cannot write tpa file header with collisions \n";
		goto error;
	}

	CloseHandle(h_file);
	return;

error:
	MessageBox(NULL,"Error writing to audio pack file.", "Trespasser Sound Effect Editor",MB_OK|MB_ICONINFORMATION);
	CloseHandle(h_file);
	return;
}


//*********************************************************************************************
void CEffectDatabase::SaveTextTransfer(ofstream& file, SCollisionSample& csm)
{
	SEffect* peff = peffFindEffectID(csm.u4SampleID);
	Assert(peff);

	// Write out the sample effect identifier
	file << peff->cstrID << '\n';

	// write out the transfer that is to be used while playing
	file << csm.stTransfer.fVolMin << '\n';
	file << csm.stTransfer.fVolMax << '\n';
	file << csm.stTransfer.fVolSlope << '\n';
	file << csm.stTransfer.fVolInt << '\n';
	file << csm.stTransfer.fPitchMin << '\n';
	file << csm.stTransfer.fPitchMax << '\n';
	file << csm.stTransfer.fPitchSlope << '\n';
	file << csm.stTransfer.fPitchInt << '\n';
	file << csm.stTransfer.fAttenuate << '\n';
	file << csm.stTransfer.fMinVelocity << '\n';
}


//*********************************************************************************************
void CEffectDatabase::SaveText(const char* str_filename)
{
	ofstream file(str_filename, ios::out|ios::trunc);	// Open and truncate.
	Assert(file.is_open());

	// version number
	file << fPACKED_AUDIO_TEXT_VERSION << '\n';

	// number of samples identifies
	file << u4TotalEffects() << '\n';

	// sample identifier followed by sample
	for (TEffectVector::iterator i = evEffects.begin(); i<evEffects.end(); ++i)
	{
		file << (*i).cstrID << '\n';
		file << (*i).cstrFilename << '\n';
		file << (*i).fMasterVolume << '\n';
		file << (*i).fAttenuation << '\n';
	}

	file << u4TotalMaterials() << '\n';
	for (TMaterialVector::iterator j = mvMaterials.begin(); j<mvMaterials.end(); ++j)
	{
		file << (*j).cstrID << '\n';
	}

	file << u4TotalCollisions() << '\n';
	for (TCollisionMap::iterator k = cmCollisions.begin(); k!=cmCollisions.end(); ++k)
	{
		uint32 u4_mat1 = (uint32) (((*k).second.u8Key) & 0xffffffff);
		uint32 u4_mat2 = (uint32) ( (((*k).second.u8Key)>>32) & 0xffffffff);

		// get the materials for this collision
		SMaterialListElement* pmle_mat1 = edbEffects.pmleFindMaterialID(u4_mat1);
		SMaterialListElement* pmle_mat2 = edbEffects.pmleFindMaterialID(u4_mat2);

		file << pmle_mat1->cstrID << '\n';
		file << pmle_mat2->cstrID << '\n';

		// get the number of samples...
		uint32 u4_sam = (*k).second.u4Samples();
		uint32 u4;

		file << u4_sam << '\n';

		// save out the time delay
		float f_time = (*k).second.fGetMinDelay();
		file << f_time << '\n';

		for (u4=0; u4<u4_sam; u4++)
		{
			SaveTextTransfer(file,(*k).second.csmList[u4]);
		}

		// get the number of slide samples
		u4_sam = (*k).second.bSlide()?1:0;
		file << u4_sam << '\n';

		for (u4=0; u4<u4_sam; u4++)
		{
			SaveTextTransfer(file,(*k).second.csmSlide);
		}
	}
}


//*********************************************************************************************
bool CEffectDatabase::LoadText(const char* str_filename)
{
	ifstream file(str_filename,ios::nocreate);

	// could not open the audio file, don't complain just do nothing.
	if (file.is_open() == false)
	{
		MessageBox(NULL,"Load failed - File Error","Trespasser Sound Effect Editor",
			MB_OK|MB_ICONINFORMATION);
		return false;
	}

	float f_ver;
	file >> f_ver;

	if (f_ver > fPACKED_AUDIO_TEXT_VERSION)
	{
		MessageBox(NULL,"Load failed - Incorrect version","Trespasser Sound Effect Editor",
			MB_OK|MB_ICONINFORMATION);
		return false;
	}

	if (evEffects.size()>1)
	{
		if (MessageBox(NULL,"The identifiers already loaded will be lost.", 
			"Trespasser Sound Effect Editor",MB_OKCANCEL|MB_ICONINFORMATION) == IDCANCEL)
		{
			return false;
		}
	}

	//
	// Reset the existing database
	//
	Reset();

	// Get the number of identifiers
	int32 i4_ident;
	file >> i4_ident;

	CString	cstr_id;
	CString cstr_file;


	while (i4_ident>0)
	{
		cstr_id		= strGetLine(file);
		cstr_file	= strGetLine(file);

		float	f_vol;
		float	f_atten;

		if (f_ver>2.0f)
		{
			// Load in the sample specific data
			file >> f_vol;
			file >> f_atten;
		}
		else
		{
			f_vol	= 0.0f;
			f_atten	= 0.0f;
		}

		bNewEffect(cstr_id, cstr_file,f_vol, f_atten);

		i4_ident--;
	}

	cstr_id = "MISSING";
	if (!bFindEffectID( cstr_id ))
	{
		MessageBox(NULL,"Load failed - No 'MISSING' indentifier","Trespasser effect Editor",	MB_OK|MB_ICONINFORMATION);
		Reset();
		return false;
	}

	// Get the number of Materials
	int32 i4_mat;
	file >> i4_mat;

	if (i4_mat == 0)
		return true;

	while (i4_mat>0)
	{
		cstr_id		= strGetLine(file);

		bNewMaterial(cstr_id);

		i4_mat--;
	}

	int32 i4_col;
	file >> i4_col;
	if (i4_col == 0)
		return true;

	CString	cstr_mat1;
	CString	cstr_mat2;

	while (i4_col)
	{
		cstr_mat1 = strGetLine(file);	
		cstr_mat2 = strGetLine(file);
		uint32 u4_hash1 = u4Hash(cstr_mat1);
		uint32 u4_hash2 = u4Hash(cstr_mat2);
		uint64	u8_key = u8CollisionHash(u4_hash1, u4_hash2);

		// Create a collision
		u8NewCollision(u8_key);

		// collision samples
		uint32 u4_samples;
		file >> u4_samples;
		// Setup the number of collision samples
		cmCollisions[u8_key].SetNumSamples(u4_samples);


		// Files above 1.0 had a minimum time associtaed with the collision
		if (f_ver>1.0f)
		{
			float	f_delay;
			file >> f_delay;

			cmCollisions[u8_key].SetMinimumDelay(f_delay);
		}

		for (uint32 u4=0; u4<u4_samples; u4++)
		{
			cstr_id = strGetLine(file);
			cmCollisions[u8_key].csmList[u4].u4SampleID = u4Hash(cstr_id);

			file >> cmCollisions[u8_key].csmList[u4].stTransfer.fVolMin;
			file >> cmCollisions[u8_key].csmList[u4].stTransfer.fVolMax;
			file >> cmCollisions[u8_key].csmList[u4].stTransfer.fVolSlope;
			file >> cmCollisions[u8_key].csmList[u4].stTransfer.fVolInt;
			file >> cmCollisions[u8_key].csmList[u4].stTransfer.fPitchMin;
			file >> cmCollisions[u8_key].csmList[u4].stTransfer.fPitchMax;
			file >> cmCollisions[u8_key].csmList[u4].stTransfer.fPitchSlope;
			file >> cmCollisions[u8_key].csmList[u4].stTransfer.fPitchInt;
			file >> cmCollisions[u8_key].csmList[u4].stTransfer.fAttenuate;

			if (f_ver<3.00f)
			{
				cmCollisions[u8_key].csmList[u4].stTransfer.fMinVelocity = 0.0f;
			}
			else
			{
				file >> cmCollisions[u8_key].csmList[u4].stTransfer.fMinVelocity;
			}
		}

		// get the number of slide samples...
		file >> u4_samples;
		if (u4_samples == 1)
		{
			cmCollisions[u8_key].EnableSlide();

			cstr_id = strGetLine(file);
			cmCollisions[u8_key].csmSlide.u4SampleID = u4Hash(cstr_id);

			file >> cmCollisions[u8_key].csmSlide.stTransfer.fVolMin;
			file >> cmCollisions[u8_key].csmSlide.stTransfer.fVolMax;
			file >> cmCollisions[u8_key].csmSlide.stTransfer.fVolSlope;
			file >> cmCollisions[u8_key].csmSlide.stTransfer.fVolInt;
			file >> cmCollisions[u8_key].csmSlide.stTransfer.fPitchMin;
			file >> cmCollisions[u8_key].csmSlide.stTransfer.fPitchMax;
			file >> cmCollisions[u8_key].csmSlide.stTransfer.fPitchSlope;
			file >> cmCollisions[u8_key].csmSlide.stTransfer.fPitchInt;
			file >> cmCollisions[u8_key].csmSlide.stTransfer.fAttenuate;

			if (f_ver<3.00f)
			{
				cmCollisions[u8_key].csmSlide.stTransfer.fMinVelocity = 0.0f;
			}
			else
			{
				file >> cmCollisions[u8_key].csmSlide.stTransfer.fMinVelocity;
			}
		}
		else
		{
			Assert (u4_samples == 0);
		}

		i4_col--;
	}

	return true;
}


//*********************************************************************************************
// Empty the current STL vector in the database
void CEffectDatabase::Reset()
{
	evEffects.erase(evEffects.begin(),evEffects.end());
	mvMaterials.erase(mvMaterials.begin(),mvMaterials.end());
	cmCollisions.erase(cmCollisions.begin(),cmCollisions.end());
}


//*********************************************************************************************
void CEffectDatabase::DuplicateMaterial(uint32 u4_new, uint32 u4_source)
{
	uint32 u4_mat1, u4_mat2;
	uint32 u4_id1, u4_id2;

	for (TCollisionMap::iterator k = cmCollisions.begin(); k!=cmCollisions.end(); ++k)
	{
		u4_mat1 = (uint32)(((*k).first) & 0x00000000ffffffff);
		u4_mat2 = (uint32)((((*k).first) & 0xffffffff00000000)>>32);

		if ((u4_mat1 == u4_mat2) && (u4_mat1 == u4_source))
		{
			// Collision with our selves
			u4_id1 = u4_new;
			u4_id2 = u4_new;
		}
		else if (u4_mat1 == u4_source)
		{
			u4_id1 = u4_new;
			u4_id2 = u4_mat2;
		}
		else if (u4_mat2 == u4_source)
		{
			u4_id1 = u4_new;
			u4_id2 = u4_mat1;
		}
		else
		{
			continue;
		}

		// Add a collision
		uint64 u8_key = u8CollisionHash(u4_id1, u4_id2);
		u8NewCollision(u8_key);

		cmCollisions[u8_key] = (*k).second;
		cmCollisions[u8_key].u8Key = u8_key;
	}
}



//**********************************************************************************************
// Get a line of data as a char buffer.
//
static char strBuffer[MAX_PATH];


//**********************************************************************************************
//
const char* strGetLine(ifstream& file)
{
	// Eat any leading whitespace.
	char c;
	while (file.get(c))
	{
		if (!isspace(c))
		{
			file.putback(c);
			break;
		}
	}

	// Get the line.
	file.getline(strBuffer, MAX_PATH);
	
	return strBuffer;
}


//*********************************************************************************************
const char* strLeafName(const char* str_filename)
{
	const char* str_end = str_filename + strlen(str_filename);
	const char* str_begin = str_end;

	// back up along the string.
	while (str_begin > str_filename && *str_begin != '/' && *str_begin != '\\' &&
		*str_begin != ':')
	{
		--str_begin;
	}

	// if we are on one of the path control characters, go forward 1.
	if (*str_begin == '/' || *str_begin == '\\' || *str_begin == ':')
		++str_begin;

	Assert(str_begin < str_end);

	return str_begin;
}

//*********************************************************************************************
bool bFileExists(const char* str_filename)
{
	Assert(str_filename);

	// Open a windows file handle.
	HANDLE hfile = CreateFile
	(
		(LPCTSTR)str_filename,	// Pointer to name of the file.
		GENERIC_READ,			// Access (read-write) mode.
		FILE_SHARE_READ,		// Share mode.
		0,						// Pointer to security descriptor.
		OPEN_EXISTING,			// How to create.
		FILE_ATTRIBUTE_NORMAL,	// File attributes.
		0 						// Handle to file with attributes to copy.
	);

	// If the function fails, it is because no file exists.
	if (hfile == INVALID_HANDLE_VALUE)
		return false;

	// Close the open file.
	CloseHandle(hfile);

	// The function succeeded, therefore the file exists.
	return true;
}

//*********************************************************************************************
bool bFileExtension(const char* str_filename, const char* str_ext)
{
	Assert(str_filename);

	const char* str = str_filename +strlen(str_filename)-3;

	if (stricmp(str,str_ext) == 0)
		return true;

	return false;
}


//*********************************************************************************************
uint32 u4FileSize(const char* str_filename)
{
	Assert(str_filename);

	// Open a windows file handle.
	HANDLE hfile = CreateFile
	(
		(LPCTSTR)str_filename,	// Pointer to name of the file.
		GENERIC_READ,			// Access (read-write) mode.
		FILE_SHARE_READ,		// Share mode.
		0,						// Pointer to security descriptor.
		OPEN_EXISTING,			// How to create.
		FILE_ATTRIBUTE_NORMAL,	// File attributes.
		0 						// Handle to file with attributes to copy.
	);

	// If the function fails, it is because no file exists.
	if (hfile == INVALID_HANDLE_VALUE)
		return 0xffffffff;

	uint32 u4_file_size = GetFileSize(hfile, NULL);

	// Close the open file.
	CloseHandle(hfile);

	// return the size of the file.
	return u4_file_size;
}


//*********************************************************************************************
void LoadFile(const char* str_filename, void* pv_data)
{
	Assert(pv_data);
	Assert(str_filename);

	HANDLE hfile = CreateFile
	(
		(LPCTSTR)str_filename,	// Pointer to name of the file.
		GENERIC_READ,			// Access (read-write) mode.
		FILE_SHARE_READ,		// SHARE mode.
		0,						// Pointer to security descriptor.
		OPEN_EXISTING,			// How to create.
		FILE_ATTRIBUTE_NORMAL,	// File attributes.
		0 						// Handle to file with attributes to copy.
	);

	Assert(hfile);
	Assert(hfile != INVALID_HANDLE_VALUE);

	uint32 u4_file_size = GetFileSize(hfile, NULL);
	uint32 u4_num_bytes_read;	// Number of bytes read from the file.

	// Read in file to memory.
	ReadFile
	(
		hfile,							// Handle of file to read.
		(LPVOID)pv_data,				// Address of buffer that receives data.
		u4_file_size,					// Number of bytes to read.
		(LPDWORD)&u4_num_bytes_read,	// Address of number of bytes read.
		(LPOVERLAPPED)0 				// Address of structure for data.
	);

	// Close the open file.
	CloseHandle(hfile);
}


//*********************************************************************************************
uint32 u4Hash(const void* pv, int i_size, bool string)
{
	return CEffectDatabase::u4HashBlock(pv,i_size);
}
