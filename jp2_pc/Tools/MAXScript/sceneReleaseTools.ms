Utility SceneReleaseTools "Scene Releases"
	(
		local It_Is = true
		
		group "Counting Filters"
			(
				checkbox EObjects "\"!\" Objects" checked:true
				checkbox TerrainTextures "Terrain Textures" checked:true
				checkbox OcclusionObjects "Occlusion Objects" checked:true
				checkbox VegObjects "Vegetation Objects" checked:true
				checkbox PropObjects "Prop Objects" checked:true
				checkbox StructureObjects "Structure Objects" checked:true
				checkbox ZerothObjects "\"-00\" Objects" checked:true
				checkbutton AndNot "And"
			)

		button GetAccurateObjCount "Get Object Count"
		
		on AndNot changed state do
			if AndNot.checked then
				(
					AndNot.text = "Not"
					It_Is = false
				) else (
					AndNot.text = "And"
					It_Is = true
				)
		
		on GetAccurateObjCount pressed do
			(
				if It_Is == true do
					(
						Objectcount = 0
						ObjArray = for o in objects collect o
						for i = 1 to ObjArray.count do
							(
								valid = false
								CObj = ObjArray[i]
								CName = CObj.name

								if EObjects.checked do
									if CName[1] == "!" do valid = true

								if TerrainTextures.checked do
									(
										CBuffer = getUserPropBuffer CObj
										if findstring CBuffer "CTerrainObj" != undefined do
											valid = true
									)

								if OcclusionObjects.checked do
									if (substring CName 1 3) == "Occ" or (substring CName 1 3) == "occ" do valid = true

								if VegObjects.checked do
									if CName[1] == "V" or CName[1] == "v" do valid = true

								if PropObjects.checked do
									if CName[1] == "P" or CName[1] == "p" do valid = true

								if StructureObjects.checked do
									if CName[1] == "S" or CName[1] == "s" do valid = true

								if ZerothObjects.checked do
									if (substring CName (CName.count - 2) 3) == "-00" do valid = true

								if valid == true do
									Objectcount = (Objectcount + 1)
							)
						
						MessageBox (ObjectCount as string + " objects counted.")
					)

				if It_Is == false do
					(
						ObjArray = for o in objects collect o
						ObjectCount = ObjArray.count
						for i = 1 to ObjArray.count do
							(
								valid = true
								CObj = ObjArray[i]
								CName = CObj.name

								if EObjects.checked do
									if CName[1] == "!" do valid = false

								if TerrainTextures.checked do
									(
										CBuffer = getUserPropBuffer CObj
										if findstring CBuffer "CTerrainObj" != undefined do
											valid = false
									)

								if OcclusionObjects.checked do
									if (substring CName 1 3) == "Occ" or (substring CName 1 3) == "occ" do valid = false

								if VegObjects.checked do
									if CName[1] == "V" or CName[1] == "v" do valid = false

								if PropObjects.checked do
									if CName[1] == "P" or CName[1] == "p" do valid = false

								if StructureObjects.checked do
									if CName[1] == "S" or CName[1] == "s" do valid = false

								if ZerothObjects.checked do
									if (substring CName (CName.count - 2) 3) == "-00" do valid = false

								if valid == false do 
									Objectcount = (Objectcount - 1)
							)
						MessageBox (ObjectCount as string + " objects counted.")
					)
			)
	)