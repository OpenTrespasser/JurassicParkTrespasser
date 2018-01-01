fn convertChar a = 
	(
		if a == "a" do return "A"
		if a == "b" do return "B"
		if a == "c" do return "C"
		if a == "d" do return "D"
		if a == "e" do return "E"
		if a == "f" do return "F"
		if a == "g" do return "G"
		if a == "h" do return "H"
		if a == "i" do return "I"
		if a == "j" do return "J"
		if a == "k" do return "K"
		if a == "l" do return "L"
		if a == "m" do return "M"
		if a == "n" do return "N"
		if a == "o" do return "O"
		if a == "p" do return "P"
		if a == "q" do return "Q"
		if a == "r" do return "R"
		if a == "s" do return "S"
		if a == "t" do return "T"
		if a == "u" do return "U"
		if a == "v" do return "V"
		if a == "w" do return "W"
		if a == "x" do return "X"
		if a == "y" do return "Y"
		if a == "z" do return "Z"
		return a
	)

fn convertStr str = 
	(
		newStr = ""
		for i = 1 to str.count do newStr += (convertChar str[i])
		return newStr
	)

fn IsMaster CName = 
	(
		local DashPos = -1
		for i = 1 to CName.count do
			if CName[i] == "-" do
				DashPos = i
		if DashPos == -1 then
			return false
		else
			(
				if (substring CName (DashPos) 3) == "-00" then
					return true
				else
					return false
			)
	)

fn ReadAudioTxtFile index = 
	(
		if index == 1 do
			filename = "\\\Trespasser\SiteB\AudioPackFiles\Ambient.txt"
		if index == 2 do
			filename = "\\\Trespasser\SiteB\AudioPackFiles\Effects.txt"
		if index == 3 do
			filename = "\\\Trespasser\SiteB\AudioPackFiles\Stream.txt"

		sampleArray = #("")
		f = openFile fileName
		while not eof f do
			(
				CLine = readline f
				if findString CLine "IDENTIFIER:" != undefined do
					if CLine[13] == "'" do
						(
							cnt = 0
							for i = 14 to CLine.count do
								if CLine[i] == "'" then
									(
										CIndex = i
										Exit
									) else (
										cnt += 1
									)
							Identifier = (substring CLine 14 cnt)
							append sampleArray Identifier
						)
			)
		close f
		return sampleArray
	)

Utility AudioCoordinator "Audio Coordinator"
	(

		local debug = true

		local AmbientEELSamples = #(), EffectsEELSamples = #(), StreamEELSamples = #(), SoundData = #(),
					SoundMaterials = #(), SampleData = #(), Samples = #()

		button GetEELs "Parse EEL files" width:140
		label EELData
		button parseScene "Parse Scene" width:140
		label ParsingData
		group "Searching Options"
			(
				checkbox verbose "Verbose output"
				button notFindSMs "Sound Materials not found" width:140
				button notFindSamples "Samples not found" width:140
			)

		on getEELs pressed do
			(
				EELData.text = "Ambient..."
				AmbientEELSamples = ReadAudioTxtFile 1
				EELData.text = "Effects..."
				EffectsEELSamples = ReadAudioTxtFile 2
				EELData.text = "Stream..."
				StreamEELSamples = ReadAudioTxtFile 3
				EELData.text = ("Amb:" + AmbientEELSamples.count as string + "   Eff:" + EffectsEELSamples.count as string + "   Str: " + StreamEELSamples.count as string)
			)

		on notFindSMs pressed do
			(
				if SoundMaterials.count > 0 and EffectsEELSamples.count > 0 then
					(
						format "-- Searching for Sound Materials --\n"
						for i = 1 to SoundMaterials.count do
							(
								SndMat = convertStr SoundMaterials[i]
								if (findItem EffectsEELSamples SndMat) == 0 then
									(
										found = false
										if (findItem AmbientEELSamples SndMat) != 0 do
											(
												if verbose.checked do format "\"%\" was not found in the Effects EEL file but was found in the Ambient EEL\n" SndMat
												found = true
											)
										if (findItem StreamEELSamples SndMat) != 0 do
											(
												if verbose.checked do format "\"%\" was not found in the Effects EEL file but was found in the Stream EEL\n" SndMat
												found = true
											)
										if found == false do
											format "WARNING: \"%\" was not found in any EEL files\n" SndMat
									) else (
										if verbose.checked do format "\"%\" was found in the Effects EEL file\n" SndMat
									)
							)
						format "-- Completed searching for Sound Materials --\n"
					) else (
						messageBox "Either Sound Materials or Effects EEL\nare undefined or empty"
					)
			)

		on notFindSamples pressed do
			(
				if Samples.count > 0 then
					(
						format "-- Searching for Samples --\n"
						for i = 1 to Samples.count do
							(
								Sample = convertStr Samples[i]
								if findItem EffectsEELSamples Sample == 0 then
									(
										if findItem AmbientEELSamples Sample == 0 then
											(
												if findItem StreamEELSamples Sample == 0 then
													format "WARNING: \"%\" was not found in the EEL files\n" Sample
												else
													if verbose.checked do format "\"%\" was found in the Stream EEL file\n" Sample
											) else (
												if verbose.checked do format "\"%\" was found in the Ambient EEL file\n" Sample
											)
									) else (
										if verbose.checked do format "\"%\" was found in the Effects EEL file\n" Sample
									)
							)
						format "-- Completed searching for Samples --\n"
					) else (
						messageBox "Samples array is empty"
					)
			)


		on ParseScene pressed do
			(
				SoundData = #()
				SoundMaterials = #()
				SampleData = #()
				Samples = #()
				for o in objects where (o.name[1] != "!") do
					(
						-- is it a master instance?
						if (IsMaster o.name) do
							(
								-- yes
								CBuffer = getUserPropBuffer o
								-- searching for a soundMaterial
								if (CIndex = findString CBuffer "SoundMaterial") != undefined do
									(
										-- we have a sound prop
										-- get the sound property
										cnt = 0
										for j = (CIndex) to CBuffer.count do
											(
												if CBuffer[j] == "\"" do
													(
														for k = (j+1) to CBuffer.count do
															(
																if CBuffer[k] == "\"" do
																	(
																		SoundMaterial = substring CBuffer (j+1) cnt
																		append SoundData #(o.name, SoundMaterial)
																		if (findItem SoundMaterials SoundMaterial) == 0 do append SoundMaterials SoundMaterial
																		exit
																	)
																cnt += 1
															)
														exit
													)
											)
									)
								-- Searching for a Sample
								if (CIndex = findString CBuffer "Sample") != undefined do
									(
										-- we have a sample
										-- get the sound property
										cnt = 0
										for j = (CIndex) to CBuffer.count do
											(
												if CBuffer[j] == "\"" do
													(
														for k = (j+1) to CBuffer.count do
															(
																if CBuffer[k] == "\"" do
																	(
																		Sample = substring CBuffer (j+1) cnt
																		append SampleData #(o.name, Sample)
																		if (findItem Samples Sample) == 0 do append Samples Sample
																		exit
																	)
																cnt += 1
															)
														exit
													)
											)
									)
							)
					)
				ParsingData.text = ("Snd Mats:" + SoundMaterials.count as string + "    Samples:" + Samples.count as string)
			)
	)