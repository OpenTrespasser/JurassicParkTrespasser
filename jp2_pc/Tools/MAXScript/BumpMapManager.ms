-- **********************************************************************************************
-- *
-- * Copyright © DreamWorks Interactive, 1997
-- *
-- * Contents:
-- *						Implementation of BumpMapManager
-- * Bugs:
-- *
-- * To do:
-- *
-- * Notes:
-- *
-- **********************************************************************************************
-- compares two arrays - element by element - note that it only goes one level deep
fn ArrayCompare foo a b =
	(
		same = true
		for i = 1 to a.count do
			if a[i] != b[i] do same = false
		return same
	)

-- **********************************************************************************************
-- * Start Utility
-- **********************************************************************************************
Utility BumpMapManager "BumpMapManager"
	(
		local MaterialArray, DiffuseMapArray,
		Props_Maps_dir = "\\\JPII_PC\Trespass\Art\Props\Maps\\",
		Structure_Maps_dir = "\\\JPII_PC\Trespass\Art\Structure\Maps\\",
		Terrain_Maps_dir = "\\\JPII_PC\Trespass\Art\Terrain\Maps\\",
		Animal_Maps_dir = "\\\JPII_PC\Trespass\Art\Animal\Maps\\",
		Vegetation_Maps_dir = "\\\JPII_PC\Trespass\Art\Vegetation\Maps\\",
		temp_Maps_dir = "d:\\temp\\"

		local debug = false
		
		checkbox GenNewTextures "Generate New Textures" checked:true
		button SearchScene "Search Scene"

		on SearchScene pressed do
			(
				MaterialArray = #()
				DiffuseMapArray = #()
				for o in objects do
					(
						if (substring o.name (o.name.count - 2) 3) == "-00" do
							(
								-- Standard Material definition
								if classOf o.mat == standardmaterial do
									(
										CMat = #("","","")
										if classOf o.mat.diffusemap == bitmaptexture do
											CMat[1] = (FilenamefromPath o.mat.diffusemap.filename)
										if classOf o.mat.opacitymap == bitmaptexture do
											CMat[2] = (FilenamefromPath o.mat.opacitymap.filename)
										if classOf o.mat.bumpmap == bitmaptexture do
											CMat[3] = (FilenamefromPath o.mat.bumpmap.filename)
										found = false
										for j = 1 to MaterialArray.count do
											if (ArrayCompare true CMat MaterialArray[j]) == true do found = true
										if found == false do
											(
												append DiffuseMapArray (FilenamefromPath o.mat.diffusemap.filename)
												append MaterialArray CMat
											)
									)
								-- Multi-Material definition
								if classOf o.mat == multimaterial do
									(
										ns = o.mat.numsubs
										for i = 1 to ns do
											(
												CMat = #("","","")
												if classOf o.mat[i].diffusemap == bitmaptexture do
													CMat[1] = (FilenamefromPath o.mat[i].diffusemap.filename)
												if classOf o.mat[i].opacitymap == bitmaptexture do
													CMat[2] = (FilenamefromPath o.mat[i].opacitymap.filename)
												if classOf o.mat[i].bumpmap == bitmaptexture do
													CMat[3] = (FilenamefromPath o.mat[i].bumpmap.filename)
												found = false
												for j = 1 to MaterialArray.count do
													if (ArrayCompare true CMat MaterialArray[j]) == true do found = true
												if found == false do
													(
														append DiffuseMapArray (FilenamefromPath o.mat[i].diffusemap.filename)
														append MaterialArray CMat
													)
											)
									)
							)
					)

				NewModDiffuseArray = #()
				ModDiffuseArray = #()
				ModMatArray = #()

				for i = 1 to DiffuseMapArray.count do	
					(
						CName = DiffuseMapArray[i]
						for j = 1 to DiffuseMapArray.count do
							if i != j do
								if CName == DiffuseMapArray[j] do
									(
										append ModDiffuseArray CName
										ModMatArray[ModDiffuseArray.count] = MaterialArray[i]
										if debug do format "Found bad definition at: %\n" j
									)
					)

				if debug do
					(
						for i = 1 to ModMatArray.count do
							format "%: %\n" i ModMatArray[i]
						format "%\n" ModDiffuseArray
					)

				for i = 1 to ModDiffuseArray.count do
					(
						CMapName = ModDiffuseArray[i]
						-- first, let's see if this texturemap is used in a meterial definition alone
						if ModMatArray[i][1] == CMapName do
							if ModMatArray[i][2] == "" and ModMatArray[i][3] == "" do
								(
									if debug do format "Detected % being used in Material definition % ALONE\n" CMapName ModMatArray[i]
									if GenNewtextures.checked do
										(
											-- note that we're assuming "<name>##t2.bmp" naming convention for the creation of this new name
											NewMapName = ((substring CMapName 1 (CMapName.count - 6)) + "Xt2.bmp")
											NewPath = undefined
											-- "D" type maps are debugging maps
											if NewMapName[1] == "d" or NewMapName[1] == "D" do NewPath = Temp_Maps_dir
											-- "D" type maps are debugging maps
											if NewMapName[1] == "s" or NewMapName[1] == "S" do NewPath = Structure_Maps_dir
											if NewMapName[1] == "p" or NewMapName[1] == "P" do NewPath = Props_Maps_dir
											if NewMapName[1] == "a" or NewMapName[1] == "A" do NewPath = Animal_Maps_dir
											if NewMapName[1] == "t" or NewMapName[1] == "T" do NewPath = Terrain_Maps_dir
											if NewMapName[1] == "v" or NewMapName[1] == "V" do NewPath = Vegetation_Maps_dir
											if NewPath != undefined then
												(
													NewMapName = (NewPath + NewMapName)
													if debug do format "NewMapName: %\n" NewMapName
													DeleteFile NewMapName
													copyFile (NewPath + CMapName) NewMapName
													NewModDiffuseArray[i] = NewMapName
												)
											else
												format "Unable to determine Map type from filename \"%\"\n" NewMapName
										)
								)
					)

				if debug do format "NewModDiffuseArray: %\n" NewModDiffuseArray
				ModObjects = 0
				ModMaps = 0
				format "\n***********************   Start Log   *************************\n"
				for i = 1 to NewModDiffuseArray.count do
					if NewModDiffuseArray[i] != undefined do
						(
							for o in objects do
								(
									CMod = false
									-- Standard Material
									if classOf o.mat == standardmaterial do
										(
											CMat = #("","","")
											if classOf o.mat.diffusemap == bitmaptexture do
												CMat[1] = (FilenamefromPath o.mat.diffusemap.filename)
											if classOf o.mat.opacitymap == bitmaptexture do
												CMat[2] = (FilenamefromPath o.mat.opacitymap.filename)
											if classOf o.mat.bumpmap == bitmaptexture do
												CMat[3] = (FilenamefromPath o.mat.bumpmap.filename)

											-- if we have a match, modify the material
											if (ArrayCompare true CMat ModMatArray[i]) == true do
												(
													if debug do format "Found Match - modifying material....\n"
													o.mat.diffusemap.filename = NewModDiffuseArray[i]
													ModMaps += 1
													Cmod = true
													format "Object \"%\"'s diffuse bitmap \"%\"was changed to  \"%\"\n" o.name CMat[1] (FileNameFromPath NewModDiffuseArray[i])
												)
										)
									-- MultiMaterial
									if classOf o.mat == multiMaterial do
										(
											ns = o.mat.numsubs
											for j = 1 to ns do
												(
													CMat = #("","","")
													if classOf o.mat[j].diffusemap == bitmaptexture do
														CMat[1] = (FilenamefromPath o.mat[j].diffusemap.filename)
													if classOf o.mat[j].opacitymap == bitmaptexture do
														CMat[2] = (FilenamefromPath o.mat[j].opacitymap.filename)
													if classOf o.mat[j].bumpmap == bitmaptexture do
														CMat[3] = (FilenamefromPath o.mat[j].bumpmap.filename)
		
													-- if we have a match, modify the material
													if (ArrayCompare true CMat ModMatArray[i]) == true do
														(
															if debug do format "Found Match - modifying material....\n"
															o.mat[j].diffusemap.filename = NewModDiffuseArray[i]
															ModMaps += 1
															Cmod = true
															format "Object \"%\"'s SubMaterial# % DiffuseBitmap \"%\" was changed to \"%\"\n" o.name j CMat[1] (FilenameFromPath NewModDiffuseArray[i])
														)
												)
										)
									if CMod == true do ModObjects += 1
								)
						)
				format "***********************   End Log  *************************\n"
				MessageBox (ModMaps as string + " bitmaps have been modified\nacross " + ModObjects as string + " objects.\nPlease see the Listener Window for more details.")
			)
	)
