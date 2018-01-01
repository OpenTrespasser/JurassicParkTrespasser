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

Utility SoundMatUpdate "Sound Material Update"
	(
		-- debug mode
		local debug = true
		
		checkbox WarningsOnly "Warnings Only"
		button ExportSelection "Export Selection" width:140
		button ImportSelection "Import Selection" width:140
		label status1

		on ExportSelection pressed do
			(
				sel = selection as array
				sc = sel.count
				SoundData = #()
				-- gather the sound material info from the objects...
				status1.text = "Gathering Info...."
				for o in sel do
					(
						if heapfree < 2000000 do if heapsize < 64000000 do heapsize += 2000000
						if (isMaster o.name) and (o.name[1] != "!") do
							(
								if debug do (format "Master object detected...%\n" o.name)
								CBuffer = getUserPropBuffer o
								if (CIndex = findString CBuffer "SoundMaterial") != undefined do
									(
										if debug do (format "Has a sound Material\n")
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
																		if debug do (format "SoundMaterial = %\n" SoundMaterial)
																		append SoundData #(o.name, SoundMaterial)
																	)
																cnt += 1
															)
													)
											)
									)
							)
					)
				status1.text = "Writing data to file...."
				if SoundData.count > 0 then
					(
						format "SoundData Count:%\n" SoundData.count
						if (f = createFile (getSaveFileName())) != undefined then
							(
								format "; Exported: %\n" localtime to:f
								for i = 1 to SoundData.count do
									(
										format "%:%\n" SoundData[i][1] SoundData[i][2]
										format "%:%\n" SoundData[i][1] SoundData[i][2] to:f
									)
								close f
							) else (
								MessageBox "FOO"
							)
					) else (
						if debug do format "no sound materials found\n"
					)
				status1.text = ""
			)

		on ImportSelection pressed do
			(
				if (fName = getOpenFileName()) != undefined do
					(
						status1.text = "Reading File..."
						ObjData = #()
						f = openFile fName
						warn = false
						while not eof f do
							(
								cLine = readLine f
							
								if CLine[1] != ";" do
									(
										index = 0
										for i = 1 to cLine.count do
											(
												-- find the colon
												if CLine[i] == ":" do
													(
														index = i
														Exit
													)
											)
										if index != 0 then
											(
												ObjectName = substring CLine 1 (index - 1)
												SoundMaterialName = substring CLine (index + 1) (CLine.count - index)
												append ObjData #(ObjectName, SoundMaterialName)
											) else (
												format "WARNING: could not read SoundMaterial from file... line \"%\"\n" cLine
												warn = true
											)
									)
							)
						close f
						status1.text = "Updating Sound Materials"
						ProgressStart "Updating Sound Materials..."
						inv = (1.0 / ObjData.count) * 100
						sel = #()
						for o in selection do
							if (IsMaster o.name) do append sel o
						for i = 1 to ObjData.count do
							(
								if heapfree < 2000000 do
									(
										format "heapFree is below 2 megs...%:%\n" i heapfree
										if heapsize < 64000000 do
											(
												format "heapSize is below 64 megs...%:%\n" i heapsize
												heapsize += 2000000
											)
									)
								progressUpdate (i * inv)
								CName = ObjData[i][1]
								foundObject = false
								for o in sel do
									(
										if (o.name == CName) do
												(
													foundObject = true
													CBuffer = getUserPropBuffer o
													-- find the start if the line
													StartIndex = -1
													EndIndex = -1
													if (CIndex = FindString CBuffer "SoundMaterial") != undefined then
														(
															for j = CIndex to 1 by -1 do
																(
																	if CBuffer[j] == "\n" do
																		(
																			StartIndex = j+1
																			Exit
																		)
																)
															-- find the end of the line
															for j = CIndex to CBuffer.count do
																(
																	if CBuffer[j] == "\r" and CBuffer[j+1] == "\r" and CBuffer[j+2] == "\n" do
																		(
																			EndIndex = j-1
																			Exit
																		)
																)
															if StartIndex != -1 and EndIndex != -1 then
																(
																	CBuffer = Replace CBuffer StartIndex (EndIndex - StartIndex + 1) ("string SoundMaterial = \"" + ObjData[i][2] + "\";")
																	setUserPropBuffer o CBuffer
																	if WarningsOnly.checked == false do format "Successfully UPDATED a better sound material for %\n" o.name
																) else (
																	format "WARNING: could not update sound material info for object: %\n" o.name
																	warn = true
																)
														) else (
															setUserPropBuffer o ((getUserPropBuffer o) + "\r\r\ns SoundMaterial = \"" + ObjData[i][2] + "\";\r\r\n")
															if WarningsOnly.checked == false do format "Successfully ADDED quite possibly the best sound material for %\n" o.name
														)
											)
									)
								if foundObject == false do (format "WARNING: Could not find object \"%\" in the selection...\n" CName)
							)
						ProgressEnd()
						status1.text = ""
						if warn do (MessageBox "Warning! Check the Listener Window")
					)
			)		
	)