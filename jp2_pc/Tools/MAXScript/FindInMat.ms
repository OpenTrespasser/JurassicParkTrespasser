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


Utility FindInMat "Find In Materials"
	(
		edittext SearchString "Search for:"
		CheckBox SearchMatName "Material Names"
		CheckBox SearchMapNames "Bitmap Names" checked:true
		checkbox SearchTexture "Texture Maps" offset:[10,0] checked:true
		checkbox SearchOpacity	"Opacity Maps" offset:[10,0]
		checkbox SearchBump "Bump Maps" offset:[10,0]

		button Search "Search" width:100

		on SearchMapNames changed state do
			if SearchMapNames.checked then
				SearchTexture.enabled = SearchOpacity.enabled = SearchBump.enabled = true
			else
				SearchTexture.enabled = SearchOpacity.enabled = SearchBump.enabled = false

		on search pressed do
			(
				if SearchString.text != "" then
					(
						count = objects.count
						MAX Select none
						ProgressStart "Searching...."
						Cnt = 0
						o = geometry as array
						ObjArray = #()
						for i = 1 to o.count do
							if (IsMaster o[i].name) do
								append ObjArray o[i]

						o = undefined
						for o in ObjArray do
							(
								Cnt += 1
								ProgressUpdate ((Cnt/count as float) * 100)
								-- Standard Material **********************************************************************************
								if classOf o.mat == standardMaterial do
									(
										if SearchMatName.checked do
											if (findstring o.mat.name SearchString.text) != undefined do
												selectmore o

										if SearchMapNames.checked do
											(
												if SearchTexture.checked do
													if classof o.mat.diffusemap == bitmaptexture do
														if (findstring (FilenameFromPath o.mat.diffusemap.filename) SearchString.text) != undefined do
															selectmore o
												if SearchOpacity.checked do
													if classof o.mat.opacitymap == bitmaptexture do
														if (findstring (FilenameFromPath o.mat.opacitymap.filename) SearchString.text) != undefined do
															selectmore o
		
												if SearchBump.checked do
													if classof o.mat.bumpmap == bitmaptexture do
														if (findstring (FilenameFromPath o.mat.bumpmap.filename) SearchString.text) != undefined do
															selectmore o
											)
									)
								-- Multi Material ***************************************************************************************
								if classOf o.mat == multimaterial do
									(
										ns = o.mat.numsubs
										if SearchMatName.checked do
											(
												if (findstring o.mat.name SearchString.text) != undefined do
													selectmore o
												for i = 1 to ns do
													if (findstring o.mat[i].name SearchString.text) != undefined do
														selectmore o
											)

										if SearchMapNames.checked do
											(
												if SearchTexture.checked do
													for i = 1 to ns do
														if classof o.mat[i].diffusemap == bitmaptexture do
															if (findstring (FilenameFromPath o.mat[i].diffusemap.filename) SearchString.text) != undefined do
																selectmore o

												if SearchOpacity.checked do
													for i = 1 to ns do
														if classof o.mat[i].opacitymap == bitmaptexture do
															if (findstring (FilenameFromPath o.mat[i].opacitymap.filename) SearchString.text) != undefined do
																selectmore o
		
												if SearchBump.checked do
													for i = 1 to ns do
														if classof o.mat[i].bumpmap == bitmaptexture do
															if (findstring (FilenameFromPath o.mat[i].bumpmap.filename) SearchString.text) != undefined do
																selectmore o
											)
									)
								if heapfree < 2000000 do heapsize += 4000000
							)
						ProgressEnd()
					) else (
						messageBox "No search text is defined!"
					)
			)
	)