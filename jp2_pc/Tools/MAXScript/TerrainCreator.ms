Utility TrnCreate "Terrain Creator"
	(
		button Float_it "Float it"
		rollout TerrainCreator "Trespasser Terrain Creator"
			(
				local TessMaps = #(), DisplaceMaps = #(), SecTessMaps = #(), SecDisplaceMaps = #(),
					GameAreaPositions = #([-1792,-768,0], [-1792,0,0], [-1024,256,0], [0,512,0], [0,-256,0], [256,-768,0], [768,1024,0], [768,1024,0], [1792,0,0], [1024, 512, 0])

				local DefaultArea = 4
				local debug = false
				
				DropDownList GameArea "Game Area:" items:#("Beach", "Jungle Road", "Platation House", "Industrial Jungle", "InGEN Town", "Plains", "PineValley", "Shore", "*InGEN Lab", "Ascent", "Summit") selection:DefaultArea
				label TessFileLabel "Tesselation File:" align:#left
				button ChooseTessFile width:150 align:#right offset:[0,-20]
				label DisplaceFileLabel "Displace File:" align:#left
				button ChooseDisplaceFile width:150 align:#right offset:[0,-20]

				label SecTessFileLabel "Sec Tesselation File:" align:#left
				button SecChooseTessFile width:150 align:#right offset:[0,-20]
				label SecDisplaceFileLabel "Sec Displace File:" align:#left
				button SecChooseDisplaceFile width:150 align:#right offset:[0,-20]

				spinner DispStr "Displacement Map Strength:" fieldwidth:50 range:[0,1000,174.57]
				spinner SecDispStr "Secondary Map Strength:" fieldwidth:50 range:[0,1000, 20]
				button GenerateTerrain "Generate Terrain" width:250 height:30
				button ProcessSecondary "Process Secondary Displacements" width:250 height:30
				label status1
				
				-- *****************************************************************************************************
				-- * Load the Tessellation map list
				-- *****************************************************************************************************
				on ChooseTessFile pressed do
					(
						if (TessFile = getOpenFilename "Choose Tesselation Map List:") != undefined do
							(
								TessMaps = #()
								WorkingFolder = getFilenamePath TessFile
								ChooseTessFile.text = FilenameFromPath TessFile
								f = openFile TessFile
								while not eof f do
									(
										CLine = readline f
										-- ";" is a commented line
										if CLine[1] != ";" do
											(
												-- prune off the newline character if it's there
												if (substring CLine CLine.count 1) == "\n" do CLine = (substring CLine 1 (CLine.count - 1))
												append TessMaps (WorkingFolder + CLine)
												if debug do format "Added \"%\" to Tessellation Map Array\n" (WorkingFolder + CLine)
											)
									)
								close f
								print TessMaps
							)
					)

				-- *****************************************************************************************************
				-- * Load the Tessellation map list
				-- *****************************************************************************************************
				on SecChooseTessFile pressed do
					(
						if (SecTessFile = getOpenFilename "Choose Tesselation Map List:") != undefined do
							(
								SecTessMaps = #()
								WorkingFolder = getFilenamePath SecTessFile
								SecChooseTessFile.text = FilenameFromPath SecTessFile
								f = openFile SecTessFile
								while not eof f do
									(
										CLine = readline f
										-- ";" is a commented line
										if CLine[1] != ";" do
											(
												-- prune off the newline character if it's there
												if (substring CLine CLine.count 1) == "\n" do CLine = (substring CLine 1 (CLine.count - 1))
												append SecTessMaps (WorkingFolder + CLine)
												if debug do format "Added \"%\" to Tessellation Map Array\n" (WorkingFolder + CLine)
											)
									)
								close f
								print SecTessMaps
							)
					)

				-- *****************************************************************************************************
				-- * Load the Displacement map list
				-- *****************************************************************************************************
				on SecChooseDisplaceFile pressed do
					(
						if (SecDisplaceFile = getOpenFilename "Choose Secondary Map List:") != undefined do
							(
								SecDisplaceMaps = #()
								WorkingFolder = getFilenamePath SecDisplaceFile
								SecChooseDisplaceFile.text = FilenameFromPath SecDisplaceFile
								f = openFile SecDisplaceFile
								while not eof f do
									(
										CLine = readline f
										-- ";" is a commented line
										if CLine[1] != ";" do
											(
												-- prune off the newline character if it's there
												if (substring CLine CLine.count 1) == "\n" do CLine = (substring CLine 1 (CLine.count - 1))
												append SecDisplaceMaps (WorkingFolder + CLine)
												if debug do format "Added \"%\" to Displacement Map Array\n" (WorkingFolder + CLine)
											)
									)
								close f
								print SecDisplaceMaps
							)
					)

				-- *****************************************************************************************************
				-- * Load the Displacement map list
				-- *****************************************************************************************************
				on ChooseDisplaceFile pressed do
					(
						if (DisplaceFile = getOpenFilename "Choose Displacement Map List:") != undefined do
							(
								DisplaceMaps = #()
								WorkingFolder = getFilenamePath DisplaceFile
								ChooseDisplaceFile.text = FilenameFromPath DisplaceFile
								f = openFile DisplaceFile
								while not eof f do
									(
										CLine = readline f
										-- ";" is a commented line
										if CLine[1] != ";" do
											(
												-- prune off the newline character if it's there
												if (substring CLine CLine.count 1) == "\n" do CLine = (substring CLine 1 (CLine.count - 1))
												append DisplaceMaps (WorkingFolder + CLine)
												if debug do format "Added \"%\" to Displacement Map Array\n" (WorkingFolder + CLine)
											)
									)
								close f
								print DisplaceMaps
							)
					)

				-- *****************************************************************************************************
				-- * Generate Terrain
				-- *****************************************************************************************************
				on GenerateTerrain pressed do
					(
						CTerrain = mesh length:2048 width:2048 lengthSegs:8 widthSegs:8
						CTerrain.pivot = CTerrain.center
						Cterrain.pos = GameAreaPositions[GameArea.selection]
						addmodifier CTerrain (UVWMap())
						CTerrain.modifiers[1].gizmo.scale = [1.00001, 1.00001, 1.00001] 
						convertToMesh CTerrain
						CTerrain.name = ("!Terrain" + localtime)
						CTerrain.wirecolor = (color 76 82 110)
						-- Generate the tesselation
						for i = 1 to TessMaps.count do
							(
								status1.text = ("Tesselating " + i as string + " of " + TessMaps.count as string + " " + filenameFromPath TessMaps[i])
								BMS = ("BMS" + i as string) as name
								TES = ("TES" + i as string) as name
								addmodifier CTerrain (Bitmap_Select())
								CTerrain.modifiers[#Bitmap_Select].name = BMS
								CTerrain.Modifiers[BMS].bitmap = (openbitmap TessMaps[i])
								addmodifier CTerrain (Tessellate())
								CTerrain.modifiers[#Tessellate].name = TES
								CTerrain.modifiers[TES].tension = 0
							)
						ConvertToMesh CTerrain
						setFaceSelection CTerrain #()
						select CTerrain
						max modify mode
						-- max subobject sel
						-- max subobject sel
						-- generate the Displacements
						for i = 1 to DisplaceMaps.count do
							(
								status1.text = ("Displacing " + i as string + " of " + DisplaceMaps.count as string + " " + filenameFromPath DisplaceMaps[i])
								DIS = ("DIS" + i as string) as name
								addmodifier CTerrain (displace())
								CTerrain.modifiers[#Displace].name = DIS
								CTerrain.modifiers[DIS].gizmo.scale = [1.0001, 1.0001, 1.0001]
								CTerrain.modifiers[DIS].bitmap = (openbitmap DisplaceMaps[i])
								CTerrain.modifiers[DIS].strength = DispStr.value
							)
						ConvertToMesh CTerrain
						select CTerrain
						max zoomExt sel all
						-- in coordsys local rotate CTerrain 180 z_axis
						status1.text = ""
						-- collect all that memory back!
						freeSceneBitmaps()
						gc()
					)

				-- *****************************************************************************************************
				-- * Generate Secondary Terrain modifications
				-- *****************************************************************************************************
				on ProcessSecondary pressed do
					(
						if CTerrain != undefined do
							(
								for i = 1 to SecTessMaps.count do
									(
										print i
										status1.text = ("Tesselating " + i as string + " of " + SecTessMaps.count as string + " " + filenameFromPath SecTessMaps[i])
										BMS = ("BMS" + i as string) as name
										TES = ("TES" + i as string) as name
										addmodifier CTerrain (Bitmap_Select())
										CTerrain.modifiers[#Bitmap_Select].name = BMS
										CTerrain.Modifiers[BMS].bitmap = (openbitmap SecTessMaps[i])
										addmodifier CTerrain (Tessellate())
										CTerrain.modifiers[#Tessellate].name = TES
										CTerrain.modifiers[TES].tension = 0
									)
								ConvertToMesh CTerrain
								setFaceSelection CTerrain #()
								select CTerrain
								max modify mode
								-- generate the Displacements
								for i = 1 to SecDisplaceMaps.count do
									(
										print i
										status1.text = ("Displacing " + i as string + " of " + SecDisplaceMaps.count as string + " " + filenameFromPath SecDisplaceMaps[i])
										DIS = ("DIS" + i as string) as name
										addmodifier CTerrain (displace())
										CTerrain.modifiers[#Displace].name = DIS
										CTerrain.modifiers[DIS].gizmo.scale = [1.0001, 1.0001, 1.0001]
										CTerrain.modifiers[DIS].bitmap = (openbitmap SecDisplaceMaps[i])
										CTerrain.modifiers[DIS].strength = SecDispStr.value
									)
								ConvertToMesh CTerrain
								select CTerrain
								max zoomExt sel all
								-- in coordsys local rotate CTerrain 180 z_axis
								status1.text = ""
								-- collect all that memory back!
								freeSceneBitmaps()
								gc()
							)
					)
			) -- End Rollout

		on Float_it pressed do
			(
				Terrain_Creator = newRolloutFloater "Trespasser Terrain Creation" 300 363 110 400
				addrollout TerrainCreator Terrain_Creator
			)
	)
