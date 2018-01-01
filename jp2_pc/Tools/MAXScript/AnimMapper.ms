-- **********************************************************************************************
-- *
-- * Copyright © DreamWorks Interactive, 1997
-- *
-- * Contents:
-- *
-- * Bugs:
-- *
-- * To do:
-- *
-- * Notes:
-- *
-- **********************************************************************************************
Utility AnimMapper "Anim Mapper"
	(
	
		local IFL, oIFL, ImagePath
		
		group "Options"
			(
				button GetIFL "Load IFL" width:140
				Label IFLLabel
			)
		button CreateObject "Create Object" width:140
		
		on GetIFL pressed do
			(
				if (IFLFile = getopenFileName caption:"Choose Image File List") != undefined do
					(
						ImagePath = GetFilenamePath IFLFile
						IFL = #()
						oIFL = #()
						f = openFile IFLFile
						while not eof f do
							(
								CLine = readline f
								if CLine[CLine.count] == "\n" do (CLine = (substring CLine 1 (CLine.count - 1)))
								colonPosition = -1
								for i = 1 to CLine.count do
									if CLine[i] == ":" do
										colonPosition = i
								if colonPosition == -1 then
									(
										if CLine.count > 5 do
											if (substring CLine (CLine.count - 3) 1) == "." do
												(
													append IFL CLine
													append oIFL ""
												)
									) else (
										if (substring CLine (colonPosition - 4) 1) == "." do
											append IFL (substring CLine 1 (colonPosition - 1))
										if (substring CLine (CLine.count - 3) 1) == "." then
											append oIFL (substring CLine (colonPosition + 1) (cLine.count - colonPosition))
										else 
											append oIFL ""
									)
							)
						close f
						if (IFL.count <= 32) and (IFL.count > 0) then
							(
								GetIFL.text = (FilenameFromPath IFLFile)
								IFLLabel.text = (IFL.count as string + " frames loaded.")
								print IFL
								print oIFL
							) else (
								messageBox "More than 32 frames loaded!\nThat's too many frames!\n\nTry again."
								IFL = #()
								oIFL = #()
								GetIFL.text = "Load IFL File"
							)
					)
			)

		on CreateObject pressed do
			(
				NumFrames = IFL.count
				-- make the sphere
				s = sphere()
				s.radius = 0.25
				s.pos = [0,0,-10]
				s.segs = 8
				s.mapcoords = true
				convertToMesh s
				for i = 1 to s.numfaces do SetFaceMatID s i 1
				update s
				s.name = (IFL[1] + "-00")
				setUserPropBuffer s ("string Class = \"CInstance\";\r\r\nbool Visible = false;")
				NMat = multiMaterial()
				NMat.numSubs = NumFrames
				for i = 1 to NumFrames do
					(
						NMat[i].name = ("Anim_" + i as string)
						NMat[i].diffusemap = bitmaptexture()
						NMat[i].diffusemap.filename = (ImagePath + IFL[i])
						if oIFL[i] != "" do
							(
								NMat[i].opacitymap = bitmaptexture()
								NMat[i].opacitymap.filename = (ImagePath + oIFL[i])
							)
					)
				s.mat = NMat
				for i = 1 to NumFrames do setFaceMatID s i i
				update s
			)
	)