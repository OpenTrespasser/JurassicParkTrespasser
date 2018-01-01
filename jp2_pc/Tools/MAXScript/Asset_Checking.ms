Utility AssetChecking "Asset GroupChecker"
(

local 	OpenFilename, Index, CIndex, PlacedPosList,
				SortedIndexArray = #(),
				NetLogFileLocation = "\\\JPII_PC\Trespass\Art\LogFiles\\",
				Props_Meshes_dir = "\\\JPII_PC\Trespass\Art\Props\Meshes\\",
				Structure_Meshes_dir = "\\\JPII_PC\Trespass\Art\Structure\Meshes\\",
				Terrain_Meshes_dir = "\\\JPII_PC\Trespass\Art\Terrain\Meshes\\",
				Animal_Meshes_dir = "\\\JPII_PC\Trespass\Art\Animal\Meshes\\",
				Vegetation_Meshes_dir = "\\\JPII_PC\Trespass\Art\Vegetation\Meshes\\",
				FileArray = #(),
				ObjArray = #()

-- FileArray#() = #(str <filename>, ...)
-- ObjArray#() = #(#(ObjList),#(ObjList),#(ObjList),....)

group "Options"
(
	radiobuttons sortBy "Sort objects by:" labels:#("Name","Size") align:#left default:2
	spinner ObjRowCount "Max Objs in Rows:" range:[1,100,5] type:#integer fieldwidth:35 enabled:false
	spinner ObjBufferSize "Obj Buffer Size:" range:[0,10,2] type:#float fieldwidth:35
)
	checkbox ClearScene "Clear scene first" checked:true
	button loadMFLFile "Load MFL File" width:140

group "Status"
	(
		label status1
	)

	on loadMFLFile pressed do
		if (OpenFilename = getOpenFilename caption:"Select MFL File [MAX File List]") != undefined do
			(
				if ClearScene.checked do
					(
						ObjArray = for o in objects collect o
						for i = 1 to ObjArray.count do delete ObjArray[i]
					)

				format "\n\n"
				format "**********************************************************************\n"
				format "** Merging %\n" OpenFilename
				format "**********************************************************************\n"
				f = openfile OpenFilename
				NotLoaded = false
				status1.text = "Merging MAX files"
				while not eof f do
					(
						MaxName = readline f
						if MaxName.count >= 5 do
							(
								New_Meshes_location = undefined
								if MaxName[1] == "V" do (New_Meshes_location = Vegetation_Meshes_dir)
								if MaxName[1] == "v" do (New_Meshes_location = Vegetation_Meshes_dir)
								if MaxName[1] == "T" do	(New_Meshes_location = Terrain_Meshes_dir)
								if MaxName[1] == "t" do	(New_Meshes_location = Terrain_Meshes_dir)
								if MaxName[1] == "A" do	(New_Meshes_location = Animal_Meshes_dir)
								if MaxName[1] == "a" do	(New_Meshes_location = Animal_Meshes_dir)
								if MaxName[1] == "P" do	(New_Meshes_location = Props_Meshes_dir)
								if MaxName[1] == "p" do	(New_Meshes_location = Props_Meshes_dir)
								if MaxName[1] == "S" do	(New_Meshes_location = Structure_Meshes_dir)
								if MaxName[1] == "s" do	(New_Meshes_location = Structure_Meshes_dir)
								if New_Meshes_location != undefined then
									(
										if (findItem FileArray MaxName) == 0 then
											(
												if (MergeMaxFile (New_Meshes_location + MaxName)) != true then
													(
														format "File: '%' was not loaded because MAX could not merge it.\n" MaxName
														NotLoaded = true
													) else (
														-- Log the new objects into
														format "Merging %\n" MaxName
-- 													for o in objects do (1 == 1)
													)
											) else (
												format "% was not loaded because that MAX Filename has already been merged.\n" MaxName
											)
									) else ( 
										format "File: '%' was not loaded because I could not resolve it's path location.\n" MaxName
										NotLoaded = true
									)
							)
					)
				status1.text = "Cleaning up scene"
				-- get rid of the lights
				LightArray = for l in lights collect l
				for i = 1 to LightArray.count do
					(
						if LightArray[i].target != undefined do
							delete LightArray[i].target
						delete LightArray[i]
					)
				-- get rid of the cameras
				CamArray = for c in cameras collect c
				for i = 1 to CamArray.count do
					(
						if CamArray[i].target != undefined do
							delete CamArray[i].target
						delete CamArray[i]
					)
				-- and get rid of the helper objects
				HelperArray = for h in helpers collect h
				for i = 1 to HelperArray.count do
					(
						if HelperArray[i].target != undefined do
							delete HelperArray[i].target
						delete HelperArray[i]
					)
				status1.text = "Sorting Scene"

				if sortBy.state == 1 do
					(
						-- sort by alphabetical order
						1 == 1
					)
					
				if sortBy.state == 2 do
					(
						-- sort the objects by height
						ObjArray = for o in objects collect o
						ObjVolumes = #()
						-- Get all the Volumes
						for i = 1 to ObjArray.count do
							(
								CObj = ObjArray[i]
								CObjScale = (CObj.max - CObj.min)
								CObjVolume = (CObj.Scale.x * CObj.Scale.y * CObj.Scale.z)
								append ObjVolumes CObjVolume
							)
						-- sort them into the new array
						for i = 1 to ObjArray.count do
							( 
								LowVolume = -1.0
								for j = 1 to ObjVolumes.count do
									if ObjVolumes[j] > LowVolume do
										(
											LowVolume = ObjVolumes[j]
											CIndex = i
										)
								append SortedIndexArray CIndex
								ObjVolumes[SortedIndexArray.count] = -1.0
							)
						PlacedPosList = #()
						for i = 1 to SortedIndexArray.count do
							(
								SortedIndex = SortedIndexArray[i]
								CObj = ObjArray[SortedIndex]
								CObjSize = (CObj.max - CObj.min)
								if i == 1 then
									(
										CObj.pos = [0,0,0]
										append PlacedPosList CObj.pos
									) else (
										CObjBefore = ObjArray[SortedIndexArray[i - 1]]
										CObjSizeBefore = (CObjBefore.max - CObjBefore.min)
										ObjHeightOffset = (CObj.pivot.z - CObj.min.z)
										CObj.pos = [(PlacedPosList[i - 1].x + (0.5 * CObjSizeBefore.x) + (0.5 * CObjSize.x) + ObjBufferSize.value), 0.0, ObjHeightOffset]
										append PlacedPosList CObj.pos
									)
							)
					)

				status1.text = "Finding double objects"

				ObjNameArray = #()
				format "\n"
				for o in objects do
					(
						if (findItem ObjNameArray o.name) == 0 then 
							(
								append ObjNameArray o.name
							) else (
								format "Double Object found: %\n" o.name
							)
					)
				-- When placing objects, bring to origin height of world
				MAX select all
				MAX zoomext sel all
				status1.text = "Complete!"
				format "\n**********************************************************************\n"
				format "** Done Merging %\n" OpenFilename
				format "**********************************************************************\n"
				format "\n\n"
				if NotLoaded == true do
					messagebox "Some MAX files were not loaded.\nCheck the Listener Window for more details"
			)
)
