-- Object Properties Tool functions
-- ****************************************************************************************************
-- Define Functions
-- ****************************************************************************************************
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

fn PruneNewLines str = 
	(
		NotFound = false
		while NotFound == false do
			(
				if (CIndex = findstring str "\r\r\n") != undefined then
					(
						if CIndex != (str.count - 2) then
							str = ((substring str 1 (CIndex - 1)) + "\n" + (substring str (CIndex + 3) (str.count - (CIndex + 3))))
						else
							NotFound = true
					) else (
						NotFound = true
					)
			)
		return str
	)

	fn TpFltr obj = (ClassOf obj == Tape)

	fn OPWordArray textline = 
		(
			local c
			local Line = ""
			local Lines = #()
			for i = 1 to textline.count do
				(
					c = textline[i]
					if c != ";" then
						if (c != "\n") and (c != "\r") do
							Line = Line + c
					else
						(
							Line = Line + ";"
							append Lines Line
							Line = ""
						)
				)
			return Lines
		)

	fn FloatAsString f = 
		(
			CString = f as string
			Convert = true
			for i = 1 to CString.count do
				if CString[i] == "." do
					Convert = false
			if Convert == true do
				CString = (CString + ".0")
			return CString
		)
	
	fn IsBox Obj =
		(
			local exitCode = 1
			if obj.numfaces != 12 do (exitCode = -1)
			if obj.numverts != 8 do (exitCode = -1)
			if exitCode == 1 then return true else return false
		)

	fn LoadSoundTypes = 
		(
			local MaterialTypesArray = #("")
			if (f = openfile "\\\JPII_PC\JPII_PC\Users\Brady\\Test Scenes\Material Source Type\Material Types.txt") != undefined then
				(
					while not eof f do
						(
							MatName = readline f
							if MatName.count >=3 and MatName[1] != ";" do
								(
									if MatName[MatName.count] == "\n" then MatName = replace MatName MatName.count 1 ""
									append MaterialTypesArray MatName
								)
						)
					close f
					return MaterialTypesArray
				) else (
					MessageBox "Sound types file not found!"
					return undefined
				)
		)
