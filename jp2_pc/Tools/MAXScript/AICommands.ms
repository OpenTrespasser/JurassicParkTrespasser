fn GetNumStrings CBuffer = 
	(
		NBuffer = CopyString Cbuffer
		cnt = 0
		foundPos = true
		while foundPos do
			(
				if (StartPos = findString NBuffer "string A") != undefined then
					(
						cnt += 1
						NBuffer = replace NBuffer StartPos 1 "@"
					) else (
						foundPos = false
					)
			)
		return cnt 
	)

fn QuickSort l r a = 
	(
		if r > l do
			(
				i = l - 1
				j = r
				do (
					do (i += 1) while a[i] < a[r]
					while j > 1 do
						(
							j -= 1
							if a[j] <= a[r] do Exit
						)
					t = a[i]
					a[i] = a[j]
					a[j] = t
				) while (
					i < j
				)
				a[j] = a[i]
				a[i] = a[r]
				a[r] = t
				QuickSort l (i - 1) a
				QuickSort (i + 1) r a
			)
		return a
	)

fn intAsString n = 
	(
		if n < 10 and n > -1 do return ("0" + n as string)
		if n > 10 do return (n as string)
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

Utility AICommands "AI Commands"
	(
		local VocalSampleControlRollout
		
		dropDownList AIType "AI Command Type" items:#("Vocal Sample Control")
		button ApplyToSelection "Apply to Selection" width:140
		
		on AIType selected s do
			(
				if s == 1 do
					(
						removeRollout VocalSampleControlRollout
						addRollout VocalSampleControlRollout
					)
			)
		
		on ApplyToSelection pressed do
			if selection.count > 0 then
				(
					ObjArray = selection as array
					oc = ObjArray.count
					-- contruct the new buffer
					ObjClassString = "string Class = \"AI Command\";\r\r\n"
					PropsString = ""
					-- Type Vocal Sample Control
					if AIType.selection == 1 do
						(
							DinoTypeString = ""
							VocalTypeString = ""
							IdentifiersString = ""

							if (iDinoType = (VocalSampleControlRollout.DinoType.selection - 2)) != -1 do
								DinoTypeString = ("int Dinosaur = " + (iDinoType as integer) as string + ";\r\r\n")

							if (iVocalType = (VocalSampleControlRollout.VocalType.selection - 2)) != -1 do
								VocalTypeString = ("int Vocal = " + (iVocalType as integer) as string + ";\r\r\n")
						
							CIdentifiers = VocalSampleControlRollout.AddedIdentifiers.items
							for i = 1 to CIdentifiers.count do
								IdentifiersString += ("string A" + (intAsString (i - 1)) + " = \"" + CIdentifiers[i] + "\";\r\r\n")
							PropsString = (DinoTypeString + VocalTypeString + IdentifiersString)
						)

					for i = 1 to oc do
						setUserPropBuffer ObjArray[i] (ObjClassString + PropsString)
				) else (
					MessageBox "umm... apply to SELECTION?!?!"
				)

		-- -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
		-- Vocal Sample Rollout
		-- -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
		rollout VocalSampleControlRollout "Vocal Sample Control"
			(
				dropDownList DinoType "Dino Type:" items:#("", "Velociraptor", "T Rex", "Para", "Triceratops", "Stegosaur", "Brachiosaur")
				-- VOCAL TYPES LIST
				dropDownList VocalType "Vocal Type:" items:#("","Ouch", "Help", "Howl", "Snarl", "Croon", "Wimper", "Dying", "Attack", "Bite", "Eat", "Drink", "Swallow", "Sniff", "Tear", "Gimme the grape!")
				listbox AddedIdentifiers height:5
				dropDownList Identifiers "Identifiers" height:20
				button Add "Add" width:70 offset:[-37,0]
				button Remove "Remove" width:70 offset:[37,-26]
				button AppendSound "Append" width:70 offset:[-37,0]
				button Clear "Clear" width:70 offset:[37,-26]

				on Clear pressed do
					(
						AddedIdentifiers.items = #()
					)

				on AppendSound pressed do
					(
						for o in selection do
							(
								CBuffer = getUserPropBuffer o
								numSounds = GetNumStrings CBuffer
								IdentifiersString = ""
								CIdentifiers = AddedIdentifiers.items
								for i = 1 to CIdentifiers.count do
									IdentifiersString += ("string A" + (intAsString (numSounds + i - 1)) + " = \"" + CIdentifiers[i] + "\";\r\r\n")
								setUserPropBuffer o (CBuffer + IdentifiersString)
							)
					)

				on Add pressed do
					if Identifiers.selection != 1 do
						(
							CIdentifiers = AddedIdentifiers .items
							if findItem CIdentifiers Identifiers.items[Identifiers.selection] == 0 do
								(
									append CIdentifiers Identifiers.items[Identifiers.selection]
									AddedIdentifiers .items = CIdentifiers
								)
						)

				on Remove pressed do
					if AddedIdentifiers.items.count != 0 do
						(
							CIdentifiers = AddedIdentifiers.items
							deleteItem CIdentifiers AddedIdentifiers.selection
							AddedIdentifiers.items = CIdentifiers
						)

				on VocalSampleControlRollout open do
					(
						if (MaterialTypesArray = (ReadAudioTxtFile 2)) != undefined do
							(
								QuickSort 1 MaterialTypesArray.count MaterialTypesArray
								Identifiers .items = MaterialTypesArray
							)
					)
			)

		on AICommands open do
			(
				addRollout VocalSampleControlRollout
			)

		on AICommands close do
			(
				removeRollout VocalSampleControlRollout
			)
	)