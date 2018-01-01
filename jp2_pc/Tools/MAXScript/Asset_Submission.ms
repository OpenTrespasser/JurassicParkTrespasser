-- **********************************************************************************************
-- *
-- * Copyright © DreamWorks Interactive, 1997
-- *
-- * Contents:
-- *		Implementation of Asset_Submission.ms 
-- *		"Art Asset Submission & Tracking Utility"
-- *
-- * Bugs:
-- *
-- * To do:
-- *
-- * Notes:
-- *
-- **********************************************************************************************
-- Functions
-- **********************************************************************************************

fn Thumb_Worthy o = 
	(
		renderable = true
		if classof o == omnilight do (renderable = false)
		if classof o == directionallight do (renderable = false)
		if classof o == targetspot do (renderable = false)
		if classof o == targetobject do (renderable = false)
		if classof o == freespot do (renderable = false)
		if classof o == targetcamera do (renderable = false)
		if classof o == freecamera do (renderable = false)
		if classof o == dummy do (renderable = false)
		if classof o == point do (renderable = false)
		if classof o == tape do (renderable = false)
		if classof o == grid do (renderable = false)
		if classof o == spaceripple do (renderable = false)

		if classof o == bomb do (renderable = false)

		if classof o == spacewave do (renderable = false)

		if classof o == gravity do (renderable = false)

		if classof o == wind do (renderable = false)

		if classof o == spacedisplace do (renderable = false)

		if classof o == deflector do (renderable = false)

		if classof o == bone do (renderable = false)
		if classof o == combustion do (renderable = false)
		if classof o == line do (renderable = false)

		if classof o == donut do (renderable = false)

		if classof o == circle do (renderable = false)

		if classof o == arc do (renderable = false)

		if classof o == helix do (renderable = false)

		if classof o == ngon do (renderable = false)

		if classof o == rectangle do (renderable = false)

		if classof o == ellipse do (renderable = false)

		if classof o == star do (renderable = false)

		if classof o == text do (renderable = false)

		return renderable
	)

-- **********************************************************************************************
-- Start Utility
-- **********************************************************************************************

global
NetLogFileLocation = "\\\JPII_PC\Trespass\Art\LogFiles\\",
NetLogFileLocation2 = "\\\JPII_PC\JPII_PC\Users\Galvan\HockeyJunk\\",
NetLogFileLocation3 = "\\\JPII_PC\JPII_PC\Users\Galvan\HockeyJunk\Working\\",
Temp_dir = "\\\JPII_PC\Trespass\Art\Temp\\",
Props_Maps_dir = "\\\JPII_PC\Trespass\Art\Props\Maps\\",
Props_Source_dir = "\\\JPII_PC\Trespass\Art\Props\Maps\Source\\",
Props_Meshes_dir = "\\\JPII_PC\Trespass\Art\Props\Meshes\\",
MProps_Meshes_dir = "\\\Trespasser\SiteB\Completed Assets\Props\\",
-- Structure
Structure_Maps_dir = "\\\JPII_PC\Trespass\Art\Structure\Maps\\",
Structure_Source_dir = "\\\JPII_PC\Trespass\Art\Structure\Maps\Source\\",
Structure_Meshes_dir = "\\\JPII_PC\Trespass\Art\Structure\Meshes\\",
MStructure_Meshes_dir = "\\\Trespasser\SiteB\Completed Assets\Structures\\",
-- Terrain
Terrain_Maps_dir = "\\\JPII_PC\Trespass\Art\Terrain\Maps\\",
Terrain_Source_dir = "\\\JPII_PC\Trespass\Art\Terrain\Maps\Source\\",
Terrain_Meshes_dir = "\\\JPII_PC\Trespass\Art\Terrain\Meshes\\",
MTerrain_Meshes_dir = "\\\Trespasser\SiteB\Completed Assets\Terrain\\",
-- Animal
Animal_Maps_dir = "\\\JPII_PC\Trespass\Art\Animal\Maps\\",
Animal_Source_dir = "\\\JPII_PC\Trespass\Art\Animal\Maps\Source\\",
Animal_Meshes_dir = "\\\JPII_PC\Trespass\Art\Animal\Meshes\\",
MAnimal_Meshes_dir = "\\\Trespasser\SiteB\Completed Assets\Animal\\",
-- Vegetation
Vegetation_Maps_dir = "\\\JPII_PC\Trespass\Art\Vegetation\Maps\\",
Vegetation_Source_dir = "\\\JPII_PC\Trespass\Art\Vegetation\Maps\Source\\",
Vegetation_Meshes_dir = "\\\JPII_PC\Trespass\Art\Vegetation\Meshes\\"
MVegetation_Meshes_dir = "\\\Trespasser\SiteB\Completed Assets\Vegetation\\"

Utility Asset_Submission "Submit Assets"
(

-- Setup Asset Submission Paths
-- Props
local maps_exist_count = 0, GameArea, o, f = undefined

-- **********************************************************************************************
-- Interface Code
-- **********************************************************************************************

group "Scene Test"
(
	button test_scene "Test Scene" width:135
	label tr "Test Results:" align: #left
	label error " -- none --" align: #left
	label tr1 "" align:#left
)

group "Asset Tracking"
(
	DropDownList ArtStaff "Submitter:" items:#(" ","Alan", "Austin", "Brady", "Brian M", "Brian R", "Jay", "Jenny", "Jon P", "Kyle", "Lonnie", "Phil", "Rich", "Rolf", "AssetTeam") selection:1 height:7
	label desc1 "Please enter a description,"
	label desc2 "and any comments you have."
	label desc "Asset Description/Comments:" align:#left
	EditText AssetDesc "" Width: 300 offset:[-4,0]
	Label AreaLabel "Game Area:" align: #Left
	DropDownList Area items: #("Beach", "Jungle Road", "Platation House", "Industrial Jungle", "InGEN Town", "Plains", "PineValley", "Shore", "InGEN Lab", "Ascent", "Summit", "Gun", "Animal", "Jungle Vegetation", "Pine Vegetation", "Plains Vegetation", "Mountain Vegetation", "Rocks & Boulders", " ") selection:19 height:15
	checkbox handpacked "maps to be hand-packed"
	radiobuttons SubmitType "Submission Type:" labels:#("Art", "Physics", "Sound","Detail Reduction") align:#left
)

group "Scene Submit"
(
	checkbox overwritemaps "Overwrite Existing Maps" checked:true
	checkbox generateThumbnail "Generate Thumbnails"
	checkbox generateLogFile "Generate Logfile" checked:true
	button submit "Submit Scene" width:145
	button GenerateLog "Generate logfile only" width:145
	label status0 "Submit Status:" align:#left
	label status1 "" align:#left
	label status2 "" align:#left
)

on generateLogFile changed state do
	(
		if generateLogFile.state == off then
			(
				ArtStaff.enabled = false
				desc.enabled = false
				AssetDesc.enabled = false
				AreaLabel.enabled = false
				Area.enabled = false
				handpacked.enabled = false
			) else (
				ArtStaff.enabled = true
				desc.enabled = true
				AssetDesc.enabled = true
				AreaLabel.enabled = true
				Area.enabled = true
				handpacked.enabled = true
			)
	)

-- **********************************************************************************************
-- Generate LogFile only
-- **********************************************************************************************
on GenerateLog pressed do
	(
		Description = AssetDesc.text
		-- Who is submitting?
		if ArtStaff.selection == 1 do (ArtistName = undefined)
		if ArtStaff.selection == 2 do (ArtistName = "Alan Hickey")
		if ArtStaff.selection == 3 do (ArtistName = "Austin Grossman")
		if ArtStaff.selection == 4 do (ArtistName = "Brady Bell")
		if ArtStaff.selection == 5 do (ArtistName = "Brian Moore")
		if ArtStaff.selection == 6 do (ArtistName = "Brian Reed")
		if ArtStaff.selection == 7 do (ArtistName = "Jay Jang")
		if ArtStaff.selection == 8 do (ArtistName = "Jenny Hansen")
		if ArtStaff.selection == 9 do (ArtistName = "Jon Paquette")
		if ArtStaff.selection == 10 do (ArtistName = "Kyle McKisic")
		if ArtStaff.selection == 11 do (ArtistName = "Lonnie Kraatz")
		if ArtStaff.selection == 12 do (ArtistName = "Phil Salas")
		if ArtStaff.selection == 13 do (ArtistName = "Richard Wyckoff")
		if ArtStaff.selection == 14 do (ArtistName = "Rolf Mohr")
		if ArtStaff.selection == 15 do (ArtistName = "Asset Team")

		-- What Area?
		if Area.selection == 1 then GameArea = "Beach"
		if Area.selection == 2 then GameArea = "Jungle Road"
		if Area.selection == 3 then GameArea = "Plantation House"
		if Area.selection == 4 then GameArea = "Industrial Jungle"
		if Area.selection == 5 then GameArea = "InGEN Town"
		if Area.selection == 6 then GameArea = "Plains"
		if Area.selection == 7 then GameArea = "Pine Valley"
		if Area.selection == 8 then GameArea = "Shore"
		if Area.selection == 9 then GameArea = "InGEN Lab"
		if Area.selection == 10 then GameArea = "Ascent"
		if Area.selection == 11 then GameArea = "Summit"
		if Area.selection == 12 then GameArea = "Gun"
		if Area.selection == 13 then GameArea = "Animal"
		if Area.selection == 14 then GameArea = "Jungle Vegetation"
		if Area.selection == 15 then GameArea = "Pine Vegetation"
		if Area.selection == 16 then GameArea = "Plains Vegetation"
		if Area.selection == 17 then GameArea = "Mountain Vegetation"
		if Area.selection == 18 then GameArea = "Rocks & Boulders"

		if GameArea != undefined then
			(
				if Description.count != 0 then
					(
						if ArtistName != undefined then
							(
								if SubmitType.state == 1 do LogName = ((substring MAXFileName 1 (MAXFileName.count - 4)) + "_A")
								if SubmitType.state == 2 do LogName = ((substring MAXFileName 1 (MAXFileName.count - 4)) + "_P")
								if SubmitType.state == 3 do LogName = ((substring MAXFileName 1 (MAXFileName.count - 4)) + "_S")
								if SubmitType.state == 4 do LogName = ((substring MAXFileName 1 (MAXFileName.count - 4)) + "_A")
								save_filename = (NetLogFileLocation + LogName + ".dat")
								f = createfile save_filename
								save_filename2 = (NetLogFileLocation2 + LogName + ".dat")
								f2 = createfile save_filename2
								format "MAX FileName:\t%\n" MAXFileName to:f
								format "Objects:\t\t" to:f
								for o in objects do
									if classof o == editable_mesh do
										format "% (% faces/% verts), " o.name o.numverts o.numfaces to:f
								format "\n" to:f
								format "Artist:\t\t%\n" ArtistName to:f
								format "GameArea:\t%\n" GameArea to:f
								format "Date/Time:\t% \n" LocalTime to:f
								format "Description:\t%\n" Description to:f
								if handpacked.checked then
									format "My bitmaps will be hand packed.\n" to:f
								else
									format "My bitmaps will NOT be hand packed, make sure to check me on it!!!\n" to:f
								format "\n" to:f
								format "I posted this asset without using the submit tool because I know what I'm doing.\n" to:f
								if SubmitType.state == 1 do format "Submitted as ART\n" to:f
								if SubmitType.state == 2 do format "Submitted as PHYSICS\n" to:f
								if SubmitType.state == 3 do format "Submitted as SOUND\n" to:f
								if SubmitType.state == 4 do format "Submitted as ART\n" to:f
								close f

								format "MAX FileName:\t%\n" MAXFileName to:f2
								format "Objects:\t\t" to:f2
								for o in objects do
									if classof o == editable_mesh do
										format "% (% faces/% verts), " o.name o.numverts o.numfaces to:f2
								format "\n" to:f2
								format "Artist:\t\t%\n" ArtistName to:f2
								format "GameArea:\t%\n" GameArea to:f2
								format "Date/Time:\t% \n" LocalTime to:f2
								format "Description:\t%\n" Description to:f2
								if handpacked.checked then
									format "My bitmaps will be hand packed.\n" to:f2
								else
									format "My bitmaps will NOT be hand packed, make sure to check me on it!!!\n" to:f2
								format "\n" to:f2
								format "I posted this asset without using the submit tool because I know what I'm doing.\n" to:f2
								if SubmitType.state == 1 do format "Submitted as ART\n" to:f2
								if SubmitType.state == 2 do format "Submitted as PHYSICS\n" to:f2
								if SubmitType.state == 3 do format "Submitted as SOUND\n" to:f2
								if SubmitType.state == 4 do format "Submitted as ART (Detail Reduction)\n" to:f2
								close f2

								status1.text = "log file generated"
								status2.text = ""
							) else (
								MessageBox "You must choose yourself from the Artist list!"
							)
					) else (
						MessageBox "You must type in a description first!"
					)
			) else (
				MessageBox "You must choose a game area first!"
			)
	)

-- **********************************************************************************************
-- Test Scene
-- **********************************************************************************************

on test_scene pressed do
	(
		ExitCode = 1
		status1.text = ""
		status2.text = ""
		MapArray = #()
		maps_exist_count = 0;
		for o in objects do
			(
				if o.mat != undefined do
					(
						if classof o.mat == MultiMaterial do
							(
								ns = o.mat.numsubs
								for sub_mat_count = 1 to ns do
									(
									
									-- Process Diffuse Map
									if classof o.mat[sub_mat_count].diffusemap == bitmaptexture do
									(
										map_name = (FileNameFromPath o.mat[sub_mat_count].diffusemap.filename);
		
										New_Maps_location = undefined
										if map_name[1] == "V" do (New_Maps_location = Vegetation_Maps_dir)
										if map_name[1] == "v" do (New_Maps_location = Vegetation_Maps_dir)
										if map_name[1] == "T" do (New_Maps_location = Terrain_Maps_dir)
										if map_name[1] == "t" do (New_Maps_location = Terrain_Maps_dir)
										if map_name[1] == "A" do (New_Maps_location = Animal_Maps_dir)
										if map_name[1] == "a" do (New_Maps_location = Animal_Maps_dir)
										if map_name[1] == "P" do (New_Maps_location = Props_Maps_dir)
										if map_name[1] == "p" do (New_Maps_location = Props_Maps_dir)
										if map_name[1] == "S" do (New_Maps_location = Structure_Maps_dir)
										if map_name[1] == "s" do (New_Maps_location = Structure_Maps_dir)
		
										New_Source_location = undefined
										if map_name[1] == "V" do (New_Source_location = Vegetation_Source_dir)
										if map_name[1] == "v" do (New_Source_location = Vegetation_Source_dir)
										if map_name[1] == "T" do (New_Source_location = Terrain_Source_dir)
										if map_name[1] == "t" do (New_Source_location = Terrain_Source_dir)
										if map_name[1] == "A" do (New_Source_location = Animal_Source_dir)
										if map_name[1] == "a" do (New_Source_location = Animal_Source_dir)
										if map_name[1] == "P" do (New_Source_location = Props_Source_dir)
										if map_name[1] == "p" do (New_Source_location = Props_Source_dir)
										if map_name[1] == "S" do (New_Source_location = Structure_Source_dir)
										if map_name[1] == "s" do (New_Source_location = Structure_Source_dir)
										
										if New_Maps_Location == undefined do 
											(
												messagebox ("Bitmap \"" + map_name + "\" is not named correctly\n Please rename your map and try again\n\n Your map MUST begin with one of these letters:\n\n                     A    V    P    S    T \n\n             Asset Submission is exiting...") title:"Trespasser Asset Submission Error!"
												ExitCode = 0
												Exit
											)
										
										-- test for maps that exist
										if (getfiles (New_Maps_location + Map_name)).count == 1 do
											(
												format "Map: % was found . . .\n" (New_Maps_location + Map_name)
												if findItem MapArray Map_name == 0 do 
													(
														append MapArray Map_Name
														maps_exist_count = (maps_exist_count + 1)
													)
											)
										error.text = ((maps_exist_count as string) + " map instances already exist")
									)
									
								-- Process Opacity Map
								if classof o.mat[sub_mat_count].opacitymap == bitmaptexture do
									(
									
										map_name = (FileNameFromPath o.mat[sub_mat_count].opacitymap.filename);
											
										New_Maps_location = undefined
										if map_name[1] == "V" do (New_Maps_location = Vegetation_Maps_dir)
										if map_name[1] == "v" do (New_Maps_location = Vegetation_Maps_dir)
										if map_name[1] == "T" do (New_Maps_location = Terrain_Maps_dir)
										if map_name[1] == "t" do (New_Maps_location = Terrain_Maps_dir)
										if map_name[1] == "A" do (New_Maps_location = Animal_Maps_dir)
										if map_name[1] == "a" do (New_Maps_location = Animal_Maps_dir)
										if map_name[1] == "P" do (New_Maps_location = Props_Maps_dir)
										if map_name[1] == "p" do (New_Maps_location = Props_Maps_dir)
										if map_name[1] == "S" do (New_Maps_location = Structure_Maps_dir)
										if map_name[1] == "s" do (New_Maps_location = Structure_Maps_dir)
		
										New_Source_location = undefined
										if map_name[1] == "V" do (New_Source_location = Vegetation_Source_dir)
										if map_name[1] == "v" do (New_Source_location = Vegetation_Source_dir)
										if map_name[1] == "T" do (New_Source_location = Terrain_Source_dir)
										if map_name[1] == "t" do (New_Source_location = Terrain_Source_dir)
										if map_name[1] == "A" do (New_Source_location = Animal_Source_dir)
										if map_name[1] == "a" do (New_Source_location = Animal_Source_dir)
										if map_name[1] == "P" do (New_Source_location = Props_Source_dir)
										if map_name[1] == "p" do (New_Source_location = Props_Source_dir)
										if map_name[1] == "S" do (New_Source_location = Structure_Source_dir)
										if map_name[1] == "s" do (New_Source_location = Structure_Source_dir)
											
										if New_Maps_Location == undefined do 
											(
												messagebox ("Bitmap \"" + map_name + "\" is not named correctly\n Please rename your map and try again\n\n Your map MUST begin with one of these letters:\n\n                     A    V    P    S    T \n\n             Asset Submission is exiting...") title:"Trespasser Asset Submission Error!"
												ExitCode = 0
												Exit
											)
											
										-- test for maps that exist
										if (getfiles (New_Maps_location + Map_name)).count == 1 do
											(
												format "Map: % was found . . .\n" (New_Maps_location + Map_name)
												if findItem MapArray Map_name == 0 do 
													(
														append MapArray Map_Name
														maps_exist_count = (maps_exist_count + 1)
													)
											)
										error.text = ((maps_exist_count as string) + " map instances already exist")
										)
									
									-- Process Bump Map
									
								if classof o.mat[sub_mat_count].bumpmap == bitmaptexture do
									(
										map_name = (FileNameFromPath o.mat[sub_mat_count].bumpmap.filename);
											
										New_Maps_location = undefined
										if map_name[1] == "V" do (New_Maps_location = Vegetation_Maps_dir)
										if map_name[1] == "v" do (New_Maps_location = Vegetation_Maps_dir)
										if map_name[1] == "T" do (New_Maps_location = Terrain_Maps_dir)
										if map_name[1] == "t" do (New_Maps_location = Terrain_Maps_dir)
										if map_name[1] == "A" do (New_Maps_location = Animal_Maps_dir)
										if map_name[1] == "a" do (New_Maps_location = Animal_Maps_dir)
										if map_name[1] == "P" do (New_Maps_location = Props_Maps_dir)
										if map_name[1] == "p" do (New_Maps_location = Props_Maps_dir)
										if map_name[1] == "S" do (New_Maps_location = Structure_Maps_dir)
										if map_name[1] == "s" do (New_Maps_location = Structure_Maps_dir)
		
										New_Source_location = undefined
										if map_name[1] == "V" do (New_Source_location = Vegetation_Source_dir)
										if map_name[1] == "v" do (New_Source_location = Vegetation_Source_dir)
										if map_name[1] == "T" do (New_Source_location = Terrain_Source_dir)
										if map_name[1] == "t" do (New_Source_location = Terrain_Source_dir)
										if map_name[1] == "A" do (New_Source_location = Animal_Source_dir)
										if map_name[1] == "a" do (New_Source_location = Animal_Source_dir)
										if map_name[1] == "P" do (New_Source_location = Props_Source_dir)
										if map_name[1] == "p" do (New_Source_location = Props_Source_dir)
										if map_name[1] == "S" do (New_Source_location = Structure_Source_dir)
										if map_name[1] == "s" do (New_Source_location = Structure_Source_dir)
											
										if New_Maps_Location == undefined do 
											(
												messagebox ("Bitmap \"" + map_name + "\" is not named correctly\n Please rename your map and try again\n\n Your map MUST begin with one of these letters:\n\n                     A    V    P    S    T \n\n             Asset Submission is exiting...") title:"Trespasser Asset Submission Error!"
												ExitCode = 0
												Exit
											)
									
										-- test for maps that exist
										if (getfiles (New_Maps_location + Map_name)).count == 1 do
											(
												format "Map: % was found . . .\n" (New_Maps_location + Map_name)
												if findItem MapArray Map_name == 0 do 
													(
														append MapArray Map_Name
														maps_exist_count = (maps_exist_count + 1)
													)
											)
										error.text = ((maps_exist_count as string) + " map instances already exist")
										)
									)
				
							)
				
							if classof o.mat == StandardMaterial do
								(
							
							-- Process Diffuse Map
							if classof o.mat.diffusemap == bitmaptexture do
								(
									map_name = (FileNameFromPath o.mat.diffusemap.filename);
							
									New_Maps_location = undefined
									if map_name[1] == "V" do (New_Maps_location = Vegetation_Maps_dir)
									if map_name[1] == "v" do (New_Maps_location = Vegetation_Maps_dir)
									if map_name[1] == "T" do (New_Maps_location = Terrain_Maps_dir)
									if map_name[1] == "t" do (New_Maps_location = Terrain_Maps_dir)
									if map_name[1] == "A" do (New_Maps_location = Animal_Maps_dir)
									if map_name[1] == "a" do (New_Maps_location = Animal_Maps_dir)
									if map_name[1] == "P" do (New_Maps_location = Props_Maps_dir)
									if map_name[1] == "p" do (New_Maps_location = Props_Maps_dir)
									if map_name[1] == "S" do (New_Maps_location = Structure_Maps_dir)
									if map_name[1] == "s" do (New_Maps_location = Structure_Maps_dir)
		
									New_Source_location = undefined
									if map_name[1] == "V" do (New_Source_location = Vegetation_Source_dir)
									if map_name[1] == "v" do (New_Source_location = Vegetation_Source_dir)
									if map_name[1] == "T" do (New_Source_location = Terrain_Source_dir)
									if map_name[1] == "t" do (New_Source_location = Terrain_Source_dir)
									if map_name[1] == "A" do (New_Source_location = Animal_Source_dir)
									if map_name[1] == "a" do (New_Source_location = Animal_Source_dir)
									if map_name[1] == "P" do (New_Source_location = Props_Source_dir)
									if map_name[1] == "p" do (New_Source_location = Props_Source_dir)
									if map_name[1] == "S" do (New_Source_location = Structure_Source_dir)
									if map_name[1] == "s" do (New_Source_location = Structure_Source_dir)
									
									if New_Maps_Location == undefined do 
										(
											messagebox ("Bitmap \"" + map_name + "\" is not named correctly\n Please rename your map and try again\n\n Your map MUST begin with one of these letters:\n\n                     A    V    P    S    T \n\n             Asset Submission is exiting...") title:"Trespasser Asset Submission Error!"
											ExitCode = 0
											Exit
										)
									
										-- test for maps that exist
										if (getfiles (New_Maps_location + Map_name)).count == 1 do
											(
												if findItem MapArray Map_name == 0 do 
													(
														append MapArray Map_Name
														maps_exist_count = (maps_exist_count + 1)
													)
											)
										error.text = ((maps_exist_count as string) + " map instances already exist")
								)
							
							
							-- Process Opacity Map
							if classof o.mat.opacitymap == bitmaptexture do
								(
									map_name = FileNameFromPath o.mat.opacitymap.filename
							
									New_Maps_location = undefined
									if map_name[1] == "V" do (New_Maps_location = Vegetation_Maps_dir)
									if map_name[1] == "v" do (New_Maps_location = Vegetation_Maps_dir)
									if map_name[1] == "T" do (New_Maps_location = Terrain_Maps_dir)
									if map_name[1] == "t" do (New_Maps_location = Terrain_Maps_dir)
									if map_name[1] == "A" do (New_Maps_location = Animal_Maps_dir)
									if map_name[1] == "a" do (New_Maps_location = Animal_Maps_dir)
									if map_name[1] == "P" do (New_Maps_location = Props_Maps_dir)
									if map_name[1] == "p" do (New_Maps_location = Props_Maps_dir)
									if map_name[1] == "S" do (New_Maps_location = Structure_Maps_dir)
									if map_name[1] == "s" do (New_Maps_location = Structure_Maps_dir)
		
									New_Source_location = undefined
									if map_name[1] == "V" do (New_Source_location = Vegetation_Source_dir)
									if map_name[1] == "v" do (New_Source_location = Vegetation_Source_dir)
									if map_name[1] == "T" do (New_Source_location = Terrain_Source_dir)
									if map_name[1] == "t" do (New_Source_location = Terrain_Source_dir)
									if map_name[1] == "A" do (New_Source_location = Animal_Source_dir)
									if map_name[1] == "a" do (New_Source_location = Animal_Source_dir)
									if map_name[1] == "P" do (New_Source_location = Props_Source_dir)
									if map_name[1] == "p" do (New_Source_location = Props_Source_dir)
									if map_name[1] == "S" do (New_Source_location = Structure_Source_dir)
									if map_name[1] == "s" do (New_Source_location = Structure_Source_dir)
							
									if New_Maps_Location == undefined do 
										(
										messagebox ("Bitmap \"" + map_name + "\" is not named correctly\n Please rename your map and try again\n\n Your map MUST begin with one of these letters:\n\n                     A    V    P    S    T \n\n             Asset Submission is exiting...") title:"Trespasser Asset Submission Error!"
										ExitCode = 0
										Exit
										)
									
										-- test for maps that exist
										if (getfiles (New_Maps_location + Map_name)).count == 1 do
											(
												if findItem MapArray Map_name == 0 do 
													(
														append MapArray Map_Name
														maps_exist_count = (maps_exist_count + 1)
													)
											)
										error.text = ((maps_exist_count as string) + " map instances already exist")
								)
							
							
							-- Process Bump Map
							if classof o.mat.bumpmap == bitmaptexture do
								(
									map_name = FileNameFromPath o.mat.bumpmap.filename
									
									New_Maps_location = undefined
									if map_name[1] == "V" do (New_Maps_location = Vegetation_Maps_dir)
									if map_name[1] == "v" do (New_Maps_location = Vegetation_Maps_dir)
									if map_name[1] == "T" do (New_Maps_location = Terrain_Maps_dir)
									if map_name[1] == "t" do (New_Maps_location = Terrain_Maps_dir)
									if map_name[1] == "A" do (New_Maps_location = Animal_Maps_dir)
									if map_name[1] == "a" do (New_Maps_location = Animal_Maps_dir)
									if map_name[1] == "P" do (New_Maps_location = Props_Maps_dir)
									if map_name[1] == "p" do (New_Maps_location = Props_Maps_dir)
									if map_name[1] == "S" do (New_Maps_location = Structure_Maps_dir)
									if map_name[1] == "s" do (New_Maps_location = Structure_Maps_dir)
		
									New_Source_location = undefined
									if map_name[1] == "V" do (New_Source_location = Vegetation_Source_dir)
									if map_name[1] == "v" do (New_Source_location = Vegetation_Source_dir)
									if map_name[1] == "T" do (New_Source_location = Terrain_Source_dir)
									if map_name[1] == "t" do (New_Source_location = Terrain_Source_dir)
									if map_name[1] == "A" do (New_Source_location = Animal_Source_dir)
									if map_name[1] == "a" do (New_Source_location = Animal_Source_dir)
									if map_name[1] == "P" do (New_Source_location = Props_Source_dir)
									if map_name[1] == "p" do (New_Source_location = Props_Source_dir)
									if map_name[1] == "S" do (New_Source_location = Structure_Source_dir)
									if map_name[1] == "s" do (New_Source_location = Structure_Source_dir)
									
									if New_Maps_Location == undefined do 
										(
											messagebox ("Bitmap \"" + map_name + "\" is not named correctly\n Please rename your map and try again\n\n Your map MUST begin with one of these letters:\n\n                     A    V    P    S    T \n\n             Asset Submission is exiting...") title:"Trespasser Asset Submission Error!"
											ExitCode = 0
											Exit
										)
									
										-- test for maps that exist
										if (getfiles (New_Maps_location + Map_name)).count == 1 do
											(
												if findItem MapArray Map_name == 0 do 
													(
														append MapArray Map_Name
														maps_exist_count = (maps_exist_count + 1)
													)
											)
										error.text = ((maps_exist_count as string) + " map instances already exist")
								)
							
							)
					)
			)
		
		New_Meshes_location = undefined
		if MaxFilename[1] == "V" do (New_Meshes_location = Vegetation_Meshes_dir)
		if MaxFilename[1] == "v" do (New_Meshes_location = Vegetation_Meshes_dir)
		if MaxFilename[1] == "T" do	(New_Meshes_location = Terrain_Meshes_dir)
		if MaxFilename[1] == "t" do	(New_Meshes_location = Terrain_Meshes_dir)
		if MaxFilename[1] == "A" do	(New_Meshes_location = Animal_Meshes_dir)
		if MaxFilename[1] == "a" do	(New_Meshes_location = Animal_Meshes_dir)
		if MaxFilename[1] == "P" do	(New_Meshes_location = Props_Meshes_dir)
		if MaxFilename[1] == "p" do	(New_Meshes_location = Props_Meshes_dir)
		if MaxFilename[1] == "S" do	(New_Meshes_location = Structure_Meshes_dir)
		if MaxFilename[1] == "s" do	(New_Meshes_location = Structure_Meshes_dir)

		if New_Meshes_location == undefined do (messagebox ("File \"" + MaxFilename + "\" is not named correctly\n Please rename your MAX File and try again\n\n Your file MUST begin with one of these letters:\n\n                     A    V    P    S    T \n\n\n Remember! MAX Files have similar naming conventions as the maps!\n\n             Asset Submission is exiting...") title:"Trespasser Asset Submission Error!")
	
) -- end Scene Testing

-- **********************************************************************************************
-- Submit Scene
-- **********************************************************************************************

on submit pressed do
(

	-- delete the lihts, cameras, and helpers
	for l in lights do delete l
	for c in cameras do delete c
	-- for h in helpers do
	-- if h.children.count == 0 do delete h
	
	-- What Area?
	if Area.selection == 1 then GameArea = "Beach"
	if Area.selection == 2 then GameArea = "Jungle Road"
	if Area.selection == 3 then GameArea = "Plantation House"
	if Area.selection == 4 then GameArea = "Industrial Jungle"
	if Area.selection == 5 then GameArea = "InGEN Town"
	if Area.selection == 6 then GameArea = "Plains"
	if Area.selection == 7 then GameArea = "Pine Valley"
	if Area.selection == 8 then GameArea = "Shore"
	if Area.selection == 9 then GameArea = "InGEN Lab"
	if Area.selection == 10 then GameArea = "Ascent"
	if Area.selection == 11 then GameArea = "Summit"
	if Area.selection == 12 then GameArea = "Gun"
	if Area.selection == 13 then GameArea = "Animal"
	if Area.selection == 14 then GameArea = "Jungle Vegetation"
	if Area.selection == 15 then GameArea = "Pine Vegetation"
	if Area.selection == 16 then GameArea = "Plains Vegetation"
	if Area.selection == 17 then GameArea = "Mountain Vegetation"
	if Area.selection == 18 then GameArea = "Rocks & Boulders"

	if generateLogFile.checked == false do
		GameArea = ""

	if GameArea != undefined then
		(
			Description = AssetDesc.text

			if generateLogFile.checked == false do
				Description = "foo"

			if Description.count > 25 then
				(
					-- Who is submitting?
					if ArtStaff.selection == 1 do (ArtistName = undefined)
					if ArtStaff.selection == 2 do (ArtistName = "Alan Hickey")
					if ArtStaff.selection == 3 do (ArtistName = "Austin Grossman")
					if ArtStaff.selection == 4 do (ArtistName = "Brady Bell")
					if ArtStaff.selection == 5 do (ArtistName = "Brian Moore")
					if ArtStaff.selection == 6 do (ArtistName = "Brian Reed")
					if ArtStaff.selection == 7 do (ArtistName = "Jay Jang")
					if ArtStaff.selection == 8 do (ArtistName = "Jenny Hansen")
					if ArtStaff.selection == 9 do (ArtistName = "Jon Paquette")
					if ArtStaff.selection == 10 do (ArtistName = "Kyle McKisic")
					if ArtStaff.selection == 11 do (ArtistName = "Lonnie Kraatz")
					if ArtStaff.selection == 12 do (ArtistName = "Phil Salas")
					if ArtStaff.selection == 13 do (ArtistName = "Richard Wyckoff")
					if ArtStaff.selection == 14 do (ArtistName = "Rolf Mohr")
					if ArtStaff.selection == 15 do (ArtistName = "Asset Team")

					if generateLogFile.checked == false do
						ArtistName = "The Unknown Artist"

					if ArtistName != undefined then
						(
							ExitCode = 1
							status1.text = ""
							status2.text = ""
							if generateLogFile.checked then
								(
									if SubmitType.state == 1 do LogName = ((substring MAXFileName 1 (MAXFileName.count - 4)) + "_A")
									if SubmitType.state == 2 do LogName = ((substring MAXFileName 1 (MAXFileName.count - 4)) + "_P")
									if SubmitType.state == 3 do LogName = ((substring MAXFileName 1 (MAXFileName.count - 4)) + "_S")
									if SubmitType.state == 4 do LogName = ((substring MAXFileName 1 (MAXFileName.count - 4)) + "_A")

									save_filename = (NetLogFileLocation + LogName + ".dat")
									save_filename2 = (NetLogFileLocation2 + LogName + ".dat")
									f = createfile save_filename
									f2 = createfile save_filename2
									if SubmitType.state == 4 then
										(
											mfn = ((substring maxFileName 1 (maxFileName.count - 4)) + "_D.max")
											format "MAX FileName:\t%\n" mfn to:f
											format "MAX FileName:\t%\n" mfn to:f2
										) else (
											format "MAX FileName:\t%\n" MAXFileName to:f
											format "MAX FileName:\t%\n" MAXFileName to:f2
										)
									format "Objects:\t\t" to:f
								) else (
									LogName = (substring MAXFileName 1 (MAXFileName.count - 4))
									save_filename3 = (NetLogFileLocation3 + LogName + ".dat")
									f3 = createfile save_filename3
									format "MAX FileName:\t%\n" MAXFileName to:f3
									format "%\n" localTime to:f3
									format "\nI'm Submitting this asset without writing a logfile....\n" to:f3
									close f3
								)
				
							for o in objects do
								(
									convertToMesh o
									if generateLogFile.checked do
										(
											format "% (% faces/% verts), " o.name o.numverts o.numfaces to:f
											format "% (% faces/% verts), " o.name o.numverts o.numfaces to:f2
										)
									if o.mat != undefined do
										(
											if classof o.mat == MultiMaterial do
												(
												for sub_mat_count = 1 to o.mat.numsubs do
													(
													-- Process Diffuse Map
													
													if classof o.mat[sub_mat_count].diffusemap == bitmaptexture do
														(
															map_name = (FileNameFromPath o.mat[sub_mat_count].diffusemap.filename);
			
															New_Maps_location = undefined
															if map_name[1] == "V" do (New_Maps_location = Vegetation_Maps_dir)
															if map_name[1] == "v" do (New_Maps_location = Vegetation_Maps_dir)
															if map_name[1] == "T" do (New_Maps_location = Terrain_Maps_dir)
															if map_name[1] == "t" do (New_Maps_location = Terrain_Maps_dir)
															if map_name[1] == "A" do (New_Maps_location = Animal_Maps_dir)
															if map_name[1] == "a" do (New_Maps_location = Animal_Maps_dir)
															if map_name[1] == "P" do (New_Maps_location = Props_Maps_dir)
															if map_name[1] == "p" do (New_Maps_location = Props_Maps_dir)
															if map_name[1] == "S" do (New_Maps_location = Structure_Maps_dir)
															if map_name[1] == "s" do (New_Maps_location = Structure_Maps_dir)
			
															New_Source_location = undefined
															if map_name[1] == "V" do (New_Source_location = Vegetation_Source_dir)
															if map_name[1] == "v" do (New_Source_location = Vegetation_Source_dir)
															if map_name[1] == "T" do (New_Source_location = Terrain_Source_dir)
															if map_name[1] == "t" do (New_Source_location = Terrain_Source_dir)
															if map_name[1] == "A" do (New_Source_location = Animal_Source_dir)
															if map_name[1] == "a" do (New_Source_location = Animal_Source_dir)
															if map_name[1] == "P" do (New_Source_location = Props_Source_dir)
															if map_name[1] == "p" do (New_Source_location = Props_Source_dir)
															if map_name[1] == "S" do (New_Source_location = Structure_Source_dir)
															if map_name[1] == "s" do (New_Source_location = Structure_Source_dir)
			
															if New_Maps_Location == undefined do 
																(
																	messagebox ("Bitmap \"" + map_name + "\" is not named correctly\n Please rename your map and try again\n\n Your map MUST begin with one of these letters:\n\n                     A    V    P    S    T \n\n             Asset Submission is exiting...") title:"Trespasser Asset Submission Error!"
																	ExitCode = 0
																	Exit
																)
															
															-- Copy Maps to their new locations
															
															if overwritemaps.checked == true then
																(
																	status1.text = ("MapFile:" + Map_name)
																	CopyFile o.mat[sub_mat_count].diffusemap.filename (Temp_dir + Map_name)
																	o.mat[sub_mat_count].diffusemap.filename = (Temp_dir + Map_name)
																	DeleteFile (New_Maps_location + Map_name)
																	CopyFile o.mat[sub_mat_count].diffusemap.filename (New_Maps_location + Map_name)
																	DeleteFile (New_Source_location + Map_name)
																	CopyFile o.mat[sub_mat_count].diffusemap.filename (New_Source_location + Map_name)
																) else (
																	status1.text = ("MapFile:" + Map_name)
																	if (getfiles (New_Maps_location + Map_name)).count == 0 do
																		CopyFile (o.mat[sub_mat_count].diffusemap.filename) (New_Maps_location + Map_name)
																	if (getfiles (New_Source_location + Map_name)).count == 0 do
																		CopyFile (o.mat[sub_mat_count].diffusemap.filename) (New_Source_location + Map_name)
																)
															-- Reset Map Path to point to correct Network location and delete Temp files
															o.mat[sub_mat_count].diffusemap.filename = (New_Maps_location + Map_name)
															deletefile (Temp_dir + Map_name)
														)
													
													-- Process Opacity Map
													if classof o.mat[sub_mat_count].opacitymap == bitmaptexture do
														(
															map_name = (FileNameFromPath o.mat[sub_mat_count].opacitymap.filename)
															
															New_Maps_location = undefined
															if map_name[1] == "V" do (New_Maps_location = Vegetation_Maps_dir)
															if map_name[1] == "v" do (New_Maps_location = Vegetation_Maps_dir)
															if map_name[1] == "T" do (New_Maps_location = Terrain_Maps_dir)
															if map_name[1] == "t" do (New_Maps_location = Terrain_Maps_dir)
															if map_name[1] == "A" do (New_Maps_location = Animal_Maps_dir)
															if map_name[1] == "a" do (New_Maps_location = Animal_Maps_dir)
															if map_name[1] == "P" do (New_Maps_location = Props_Maps_dir)
															if map_name[1] == "p" do (New_Maps_location = Props_Maps_dir)
															if map_name[1] == "S" do (New_Maps_location = Structure_Maps_dir)
															if map_name[1] == "s" do (New_Maps_location = Structure_Maps_dir)
			
															New_Source_location = undefined
															if map_name[1] == "V" do (New_Source_location = Vegetation_Source_dir)
															if map_name[1] == "v" do (New_Source_location = Vegetation_Source_dir)
															if map_name[1] == "T" do (New_Source_location = Terrain_Source_dir)
															if map_name[1] == "t" do (New_Source_location = Terrain_Source_dir)
															if map_name[1] == "A" do (New_Source_location = Animal_Source_dir)
															if map_name[1] == "a" do (New_Source_location = Animal_Source_dir)
															if map_name[1] == "P" do (New_Source_location = Props_Source_dir)
															if map_name[1] == "p" do (New_Source_location = Props_Source_dir)
															if map_name[1] == "S" do (New_Source_location = Structure_Source_dir)
															if map_name[1] == "s" do (New_Source_location = Structure_Source_dir)
															
															if New_Maps_Location == undefined do 
																(
																	messagebox ("Bitmap \"" + map_name + "\" is not named correctly\n Please rename your map and try again\n\n Your map MUST begin with one of these letters:\n\n                     A    V    P    S    T \n\n             Asset Submission is exiting...") title:"Trespasser Asset Submission Error!"
																	ExitCode = 0
																	Exit
																)
															
															-- Copy Maps to their new locations
															
															if overwritemaps.checked == true then
																(
																	status1.text = ("MapFile:" + Map_name)
																	CopyFile (o.mat[sub_mat_count].opacitymap.filename) (Temp_dir + Map_name)
																	o.mat[sub_mat_count].opacitymap.filename = (Temp_dir + Map_name);
																	DeleteFile (New_Maps_location + Map_name)
																	CopyFile (o.mat[sub_mat_count].opacitymap.filename) (New_Maps_location + Map_name)
																	DeleteFile (New_Source_location + Map_name)
																	CopyFile (o.mat[sub_mat_count].opacitymap.filename) (New_Source_location + Map_name)
																) else (
																	status1.text = ("MapFile:" + Map_name)
																	if (getfiles (New_Maps_location + Map_name)).count == 0 do
																		CopyFile (o.mat[sub_mat_count].opacitymap.filename) (New_Maps_location + Map_name)
																	if (getfiles (New_Source_location + Map_name)).count == 0 do
																		CopyFile (o.mat[sub_mat_count].opacitymap.filename) (New_Source_location + Map_name)
																)
															-- Reset Map Path to point to correct Network location and delete Temp files
															o.mat[sub_mat_count].opacitymap.filename = (New_Maps_location + Map_name);
															deletefile (Temp_dir + Map_name)
														)
													
													-- Process Bump Map
													if classof o.mat[sub_mat_count].bumpmap == bitmaptexture do
														(
															map_name = (FileNameFromPath o.mat[sub_mat_count].bumpmap.filename);
			
															New_Maps_location = undefined
															if map_name[1] == "V" do (New_Maps_location = Vegetation_Maps_dir)
															if map_name[1] == "v" do (New_Maps_location = Vegetation_Maps_dir)
															if map_name[1] == "T" do (New_Maps_location = Terrain_Maps_dir)
															if map_name[1] == "t" do (New_Maps_location = Terrain_Maps_dir)
															if map_name[1] == "A" do (New_Maps_location = Animal_Maps_dir)
															if map_name[1] == "a" do (New_Maps_location = Animal_Maps_dir)
															if map_name[1] == "P" do (New_Maps_location = Props_Maps_dir)
															if map_name[1] == "p" do (New_Maps_location = Props_Maps_dir)
															if map_name[1] == "S" do (New_Maps_location = Structure_Maps_dir)
															if map_name[1] == "s" do (New_Maps_location = Structure_Maps_dir)
			
															New_Source_location = undefined
															if map_name[1] == "V" do (New_Source_location = Vegetation_Source_dir)
															if map_name[1] == "v" do (New_Source_location = Vegetation_Source_dir)
															if map_name[1] == "T" do (New_Source_location = Terrain_Source_dir)
															if map_name[1] == "t" do (New_Source_location = Terrain_Source_dir)
															if map_name[1] == "A" do (New_Source_location = Animal_Source_dir)
															if map_name[1] == "a" do (New_Source_location = Animal_Source_dir)
															if map_name[1] == "P" do (New_Source_location = Props_Source_dir)
															if map_name[1] == "p" do (New_Source_location = Props_Source_dir)
															if map_name[1] == "S" do (New_Source_location = Structure_Source_dir)
															if map_name[1] == "s" do (New_Source_location = Structure_Source_dir)
															
															if New_Maps_Location == undefined do 
																(
																	messagebox ("Bitmap \"" + map_name + "\" is not named correctly\n Please rename your map and try again\n\n Your map MUST begin with one of these letters:\n\n                     A    V    P    S    T \n\n             Asset Submission is exiting...") title:"Trespasser Asset Submission Error!"
																	ExitCode = 0
																	Exit
																)
															
															-- Copy Maps to their new locations
															if overwritemaps.checked == true then
																(
																	status1.text = ("MapFile:" + Map_name)
																	CopyFile (o.mat[sub_mat_count].bumpmap.filename) (Temp_dir + Map_name)
																	o.mat[sub_mat_count].bumpmap.filename = (Temp_dir + Map_name);
																	DeleteFile (New_Maps_location + Map_name)
																	CopyFile (o.mat[sub_mat_count].bumpmap.filename) (New_Maps_location + Map_name)
																	DeleteFile (New_Source_location + Map_name)
																	CopyFile (o.mat[sub_mat_count].bumpmap.filename) (New_Source_location + Map_name)
																) else (
																	status1.text = ("MapFile:" + Map_name)
																	if (getfiles (New_Maps_location + Map_name)).count == 0 do
																		CopyFile (o.mat[sub_mat_count].bumpmap.filename) (New_Maps_location + Map_name)
																	if (getfiles (New_Source_location + Map_name)).count == 0 do
																		CopyFile (o.mat[sub_mat_count].bumpmap.filename) (New_Source_location + Map_name)
																)
															-- Reset Map Path to point to correct Network location and delete Temp files
															o.mat[sub_mat_count].bumpmap.filename = (New_Maps_location + Map_name);
															deletefile (Temp_dir + Map_name)
														)
													)
												)
										
										if classof o.mat == StandardMaterial do
										(
										
										-- Process Diffuse Map
										if classof o.mat.diffusemap == bitmaptexture do
											(
												map_name = (FileNameFromPath o.mat.diffusemap.filename);
			
												New_Maps_location = undefined
												if map_name[1] == "V" do (New_Maps_location = Vegetation_Maps_dir)
												if map_name[1] == "v" do (New_Maps_location = Vegetation_Maps_dir)
												if map_name[1] == "T" do (New_Maps_location = Terrain_Maps_dir)
												if map_name[1] == "t" do (New_Maps_location = Terrain_Maps_dir)
												if map_name[1] == "A" do (New_Maps_location = Animal_Maps_dir)
												if map_name[1] == "a" do (New_Maps_location = Animal_Maps_dir)
												if map_name[1] == "P" do (New_Maps_location = Props_Maps_dir)
												if map_name[1] == "p" do (New_Maps_location = Props_Maps_dir)
												if map_name[1] == "S" do (New_Maps_location = Structure_Maps_dir)
												if map_name[1] == "s" do (New_Maps_location = Structure_Maps_dir)
			
												New_Source_location = undefined
												if map_name[1] == "V" do (New_Source_location = Vegetation_Source_dir)
												if map_name[1] == "v" do (New_Source_location = Vegetation_Source_dir)
												if map_name[1] == "T" do (New_Source_location = Terrain_Source_dir)
												if map_name[1] == "t" do (New_Source_location = Terrain_Source_dir)
												if map_name[1] == "A" do (New_Source_location = Animal_Source_dir)
												if map_name[1] == "a" do (New_Source_location = Animal_Source_dir)
												if map_name[1] == "P" do (New_Source_location = Props_Source_dir)
												if map_name[1] == "p" do (New_Source_location = Props_Source_dir)
												if map_name[1] == "S" do (New_Source_location = Structure_Source_dir)
												if map_name[1] == "s" do (New_Source_location = Structure_Source_dir)
			
												if New_Maps_Location == undefined do 
													(
														messagebox ("Bitmap \"" + map_name + "\" is not named correctly\n Please rename your map and try again\n\n Your map MUST begin with one of these letters:\n\n                     A    V    P    S    T \n\n             Asset Submission is exiting...") title:"Trespasser Asset Submission Error!"
														ExitCode = 0
														Exit
													)
												
												-- Copy Maps to their new locations
												
												status1.text = ("MapFile:" + Map_name)
												if overwritemaps.checked == false do
													(
														if (getfiles (New_Maps_location + Map_name)).count == 0 do
															CopyFile o.mat.diffusemap.filename (New_Maps_location + Map_name)
														if (getfiles (New_Source_location + Map_name)).count == 0 do
															CopyFile o.mat.diffusemap.filename (New_Source_location + Map_name)
													)
												
												if overwritemaps.checked == true do
												(
													CopyFile (o.mat.diffusemap.filename) (Temp_dir + Map_name)
													o.mat.diffusemap.filename = (Temp_dir + Map_name);
													DeleteFile (New_Maps_location + Map_name)
													CopyFile (o.mat.diffusemap.filename) (New_Maps_location + Map_name)
													DeleteFile (New_Source_location + Map_name)
													CopyFile (o.mat.diffusemap.filename) (New_Source_location + Map_name)
												)
												-- Reset Map Path to point to correct Network location and delete Temp files
												o.mat.diffusemap.filename = (New_Maps_location + Map_name);
												deletefile (Temp_dir + Map_name)
											)
										
										
										-- Process Opacity Map
										if classof o.mat.opacitymap == bitmaptexture do
											(
												map_name = (FileNameFromPath o.mat.opacitymap.filename);
			
												New_Maps_location = undefined
												if map_name[1] == "V" do (New_Maps_location = Vegetation_Maps_dir)
												if map_name[1] == "v" do (New_Maps_location = Vegetation_Maps_dir)
												if map_name[1] == "T" do (New_Maps_location = Terrain_Maps_dir)
												if map_name[1] == "t" do (New_Maps_location = Terrain_Maps_dir)
												if map_name[1] == "A" do (New_Maps_location = Animal_Maps_dir)
												if map_name[1] == "a" do (New_Maps_location = Animal_Maps_dir)
												if map_name[1] == "P" do (New_Maps_location = Props_Maps_dir)
												if map_name[1] == "p" do (New_Maps_location = Props_Maps_dir)
												if map_name[1] == "S" do (New_Maps_location = Structure_Maps_dir)
												if map_name[1] == "s" do (New_Maps_location = Structure_Maps_dir)
			
												New_Source_location = undefined
												if map_name[1] == "V" do (New_Source_location = Vegetation_Source_dir)
												if map_name[1] == "v" do (New_Source_location = Vegetation_Source_dir)
												if map_name[1] == "T" do (New_Source_location = Terrain_Source_dir)
												if map_name[1] == "t" do (New_Source_location = Terrain_Source_dir)
												if map_name[1] == "A" do (New_Source_location = Animal_Source_dir)
												if map_name[1] == "a" do (New_Source_location = Animal_Source_dir)
												if map_name[1] == "P" do (New_Source_location = Props_Source_dir)
												if map_name[1] == "p" do (New_Source_location = Props_Source_dir)
												if map_name[1] == "S" do (New_Source_location = Structure_Source_dir)
												if map_name[1] == "s" do (New_Source_location = Structure_Source_dir)
												
												if New_Maps_Location == undefined do 
													(
														messagebox ("Bitmap \"" + map_name + "\" is not named correctly\n Please rename your map and try again\n\n Your map MUST begin with one of these letters:\n\n                     A    V    P    S    T \n\n             Asset Submission is exiting...") title:"Trespasser Asset Submission Error!"
														ExitCode = 0
														Exit
													)
												
												-- Copy Maps to their new locations
												status1.text = ("MapFile:" + Map_name)
												if overwritemaps.checked == false do
													(
														if (getfiles (New_Maps_location + Map_name)).count == 0 do
															CopyFile (o.mat.opacitymap.filename) (New_Maps_location + Map_name)
														if (getfiles (New_Source_location + Map_name)).count == 0 do
															CopyFile (o.mat.opacitymap.filename) (New_Source_location + Map_name)
													)
												
												if overwritemaps.checked == true do
													(
														CopyFile (o.mat.opacitymap.filename) (Temp_dir + Map_name)
														o.mat.opacitymap.filename = (Temp_dir + Map_name);
														DeleteFile (New_Maps_location + Map_name)
														CopyFile (o.mat.opacitymap.filename) (New_Maps_location + Map_name)
														DeleteFile (New_Source_location + Map_name)
														CopyFile (o.mat.opacitymap.filename) (New_Source_location + Map_name)
													)
												-- Reset Map Path to point to correct Network location and delete Temp files
												o.mat.opacitymap.filename = (New_Maps_location + Map_name);
												deletefile (Temp_dir + Map_name)
											)
											
										
										-- Process Bump Map
										if classof o.mat.bumpmap == bitmaptexture do
											(
												map_name = (FileNameFromPath o.mat.bumpmap.filename);
			
												New_Maps_location = undefined
												if map_name[1] == "V" do (New_Maps_location = Vegetation_Maps_dir)
												if map_name[1] == "v" do (New_Maps_location = Vegetation_Maps_dir)
												if map_name[1] == "T" do (New_Maps_location = Terrain_Maps_dir)
												if map_name[1] == "t" do (New_Maps_location = Terrain_Maps_dir)
												if map_name[1] == "A" do (New_Maps_location = Animal_Maps_dir)
												if map_name[1] == "a" do (New_Maps_location = Animal_Maps_dir)
												if map_name[1] == "P" do (New_Maps_location = Props_Maps_dir)
												if map_name[1] == "p" do (New_Maps_location = Props_Maps_dir)
												if map_name[1] == "S" do (New_Maps_location = Structure_Maps_dir)
												if map_name[1] == "s" do (New_Maps_location = Structure_Maps_dir)
			
												New_Source_location = undefined
												if map_name[1] == "V" do (New_Source_location = Vegetation_Source_dir)
												if map_name[1] == "v" do (New_Source_location = Vegetation_Source_dir)
												if map_name[1] == "T" do (New_Source_location = Terrain_Source_dir)
												if map_name[1] == "t" do (New_Source_location = Terrain_Source_dir)
												if map_name[1] == "A" do (New_Source_location = Animal_Source_dir)
												if map_name[1] == "a" do (New_Source_location = Animal_Source_dir)
												if map_name[1] == "P" do (New_Source_location = Props_Source_dir)
												if map_name[1] == "p" do (New_Source_location = Props_Source_dir)
												if map_name[1] == "S" do (New_Source_location = Structure_Source_dir)
												if map_name[1] == "s" do (New_Source_location = Structure_Source_dir)
			
												if New_Maps_Location == undefined do 
													(
														messagebox ("Bitmap \"" + map_name + "\" is not named correctly\n Please rename your map and try again\n\n Your map MUST begin with one of these letters:\n\n                     A    V    P    S    T \n\n             Asset Submission is exiting...") title:"Trespasser Asset Submission Error!"
														ExitCode = 0
														Exit
													)
												
												-- Copy Maps to their new locations
												status1.text = ("MapFile:" + Map_name)
												if overwritemaps.checked == false do
												(
													if (getfiles (New_Maps_location + Map_name)).count == 0 do
														CopyFile (o.mat.bumpmap.filename) (New_Maps_location + Map_name)
													if (getfiles (New_Source_location + Map_name)).count == 0 do
														CopyFile (o.mat.bumpmap.filename) (New_Source_location + Map_name)
												)
												
												if overwritemaps.checked == true do
													(
														CopyFile o.mat.bumpmap.filename (Temp_dir + Map_name)
														o.mat.bumpmap.filename = (Temp_dir + Map_name);
														DeleteFile (New_Maps_location + Map_name)
														CopyFile o.mat.bumpmap.filename (New_Maps_location + Map_name)
														DeleteFile (New_Source_location + Map_name)
														CopyFile o.mat.bumpmap.filename (New_Source_location + Map_name)
													)
												-- Reset Map Path to point to correct Network location and delete Temp files
												o.mat.bumpmap.filename = (New_Maps_location + Map_name);
												deletefile (Temp_dir + Map_name)
											)
										
										)
								)
						)

						New_Meshes_location = undefined
						MNew_Meshes_location = undefined
						if MaxFilename[1] == "V" do New_Meshes_location = Vegetation_Meshes_dir
						if MaxFilename[1] == "v" do New_Meshes_location = Vegetation_Meshes_dir
						if MaxFilename[1] == "T" do	New_Meshes_location = Terrain_Meshes_dir
						if MaxFilename[1] == "t" do	New_Meshes_location = Terrain_Meshes_dir
						if MaxFilename[1] == "A" do	New_Meshes_location = Animal_Meshes_dir
						if MaxFilename[1] == "a" do	New_Meshes_location = Animal_Meshes_dir
						if MaxFilename[1] == "P" do	New_Meshes_location = Props_Meshes_dir
						if MaxFilename[1] == "p" do	New_Meshes_location = Props_Meshes_dir
						if MaxFilename[1] == "S" do	New_Meshes_location = Structure_Meshes_dir
						if MaxFilename[1] == "s" do	New_Meshes_location = Structure_Meshes_dir

						if MaxFilename[1] == "V" do MNew_Meshes_location = MVegetation_Meshes_dir
						if MaxFilename[1] == "v" do MNew_Meshes_location = MVegetation_Meshes_dir
						if MaxFilename[1] == "T" do	MNew_Meshes_location = MTerrain_Meshes_dir
						if MaxFilename[1] == "t" do	MNew_Meshes_location = MTerrain_Meshes_dir
						if MaxFilename[1] == "A" do	MNew_Meshes_location = MAnimal_Meshes_dir
						if MaxFilename[1] == "a" do	MNew_Meshes_location = MAnimal_Meshes_dir
						if MaxFilename[1] == "P" do	MNew_Meshes_location = MProps_Meshes_dir
						if MaxFilename[1] == "p" do	MNew_Meshes_location = MProps_Meshes_dir
						if MaxFilename[1] == "S" do	MNew_Meshes_location = MStructure_Meshes_dir
						if MaxFilename[1] == "s" do	MNew_Meshes_location = MStructure_Meshes_dir

						if New_Meshes_location == undefined do 
							(
								messagebox ("File \"" + MaxFilename + "\" is not named correctly\n Please rename your MAX File and try again\n\n Your file MUST begin with one of these letters:\n\n                     A    V    P    S    T \n\n             Asset Submission is exiting...") title:"Trespasser Asset Submission Error!"
								ExitCode = 0
								Exit
							)
						
						if ExitCode != 0 do
							(
								status1.text = "Saving MAX File"
								if SubmitType.state == 4 then
									(
										mfn = ((substring maxFileName 1 (maxFileName.count - 4)) + "_D.max")
										SaveMaxFile (New_Meshes_Location + mfn);
									) else (
										SaveMaxFile (New_Meshes_Location + maxFileName)
										if SubmitType.state == 3 do SaveMaxFile (MNew_Meshes_Location + maxFileName)
									)
							)
						if generateLogFile.checked do
							(						
								format "\n" to:f
								format "Artist:\t\t%\n" ArtistName to:f
								format "GameArea:\t%\n" GameArea to:f
								format "Date/Time:\t% \n" LocalTime to:f
								format "Description:\t%\n" Description to:f
								if handpacked.checked then
									format "Maps will be hand packed\n" to:f
								else
									format "Maps will NOT be hand packed, so check me on my map sizes!\n" to:f

								if SubmitType.state == 1 do format "Submitted as ART\n" to:f
								if SubmitType.state == 2 do format "Submitted as PHYSICS\n" to:f
								if SubmitType.state == 3 do format "Submitted as SOUND\n" to:f
								if SubmitType.state == 4 do format "Submitted as ART (Detail Reduction)\n" to:f

								format "\n" to:f2
								format "Artist:\t\t%\n" ArtistName to:f2
								format "GameArea:\t%\n" GameArea to:f2
								format "Date/Time:\t% \n" LocalTime to:f2
								format "Description:\t%\n" Description to:f2
								if handpacked.checked then
									format "Maps will be hand packed\n" to:f2
								else
									format "Maps will NOT be hand packed, so check me on my map sizes!\n" to:f2
								if SubmitType.state == 1 do format "Submitted as ART\n" to:f2
								if SubmitType.state == 2 do format "Submitted as PHYSICS\n" to:f2
								if SubmitType.state == 3 do format "Submitted as SOUND\n" to:f2
								if SubmitType.state == 4 do format "Submitted as ART (Detail Reduction)\n" to:f2
								close f
								close f2
							)
						
						if ExitCode == 0 then 
							(
								-- Failure!!!
								status2.text = "Errors! Submit Failed"
								status1.text = ""
							) else (
								-- Success!!!
								status1.text = "           Complete"
								if generateLogFile.checked then
									status2.text = "      Log file generated"
								else
									status2.text = ""
							)
						
						-- ******************************************************************************
						-- Thumbnails
						-- ******************************************************************************
						
						if generateThumbnail.checked do
						(
						ThumbExitCode = #good
						
						-- setup save paths
						
						Props_Thumbnail_dir = "\\\JPII_PC\Trespass\Art\\Thumbnails\\"
						Structure_Thumbnail_dir = "\\\JPII_PC\Trespass\Art\\Thumbnails\\"
						Terrain_Thumbnail_dir = "\\\JPII_PC\Trespass\Art\\Thumbnails\\"
						Animal_Thumbnail_dir = "\\\JPII_PC\Trespass\Art\\Thumbnails\\"
						Vegetation_Thumbnail_dir = "\\\JPII_PC\Trespass\Art\\Thumbnails\\"
						
						for o in objects do hide o -- hide everything
						
						ObjNameList = for o in objects collect o.name
						ObjList = for o in objects collect o
						TOC = ObjNameList.count
						UniqueObjNameList =#()
						UniqueObjList =#()
						for i = 1 to TOC do
							(
								CObjName = ObjNameList[i]
								CObj = ObjList[i]
								-- find the dash
								DashPos = -1
								for i = 1 to CObjName.count do
									(
										if substring CObjName i 1 == "-" do DashPos = i
									)
								-- if dash exists grab the substring
								if DashPos != -1 then
									(
										CObjNamePrefix = substring CObjName 1 (DashPos - 1)
										if finditem UniqueObjNameList CObjNamePrefix == 0 do 
											(
												Append UniqueObjNameList CObjNamePrefix
												append UniqueObjList CObj
											)
									) else (
										-- if not, then stick it into the Unique Array
										Append UniqueObjNameList CObjName
										append UniqueObjList CObj
									)
						)
						
								ProgressStart "Generating Thumbnails...."
								progscale = 100.0 / UniqueObjList.count
								for i = 1 to UniqueObjList.count do
									(
										o = UniqueObjList[i]
										ObjName = o.name
										if (Thumb_Worthy o) == true do
											(
											GoodObj = false
											if ObjName[1] == "V" do (New_Thumbs_location = Vegetation_Thumbnail_dir; goodObj = true)
											if ObjName[1] == "v" do (New_Thumbs_location = Vegetation_Thumbnail_dir; goodObj = true)
											if ObjName[1] == "T" do (New_Thumbs_location = Terrain_Thumbnail_dir; goodObj = true)
											if ObjName[1] == "t" do (New_Thumbs_location = Terrain_Thumbnail_dir; goodObj = true)
											if ObjName[1] == "A" do (New_Thumbs_location = Animal_Thumbnail_dir; goodObj = true)
											if ObjName[1] == "a" do (New_Thumbs_location = Animal_Thumbnail_dir; goodObj = true)
											if ObjName[1] == "P" do (New_Thumbs_location = Props_Thumbnail_dir; goodObj = true)
											if ObjName[1] == "p" do (New_Thumbs_location = Props_Thumbnail_dir; goodObj = true)
											if ObjName[1] == "S" do (New_Thumbs_location = Structure_Thumbnail_dir; goodObj = true)
											if ObjName[1] == "s" do (New_Thumbs_location = Structure_Thumbnail_dir; goodObj = true)
											if goodObj == true then
												( 
													unhide o
													theta = 46.826
													d = distance o.max o.min
													x = (d / (2 * sin(theta / 2))) * 1.0
													NewCamera = targetCamera()
													NewCamera.fov = 60
													NewCamera.target = targetObject()
													SourceCenter = o.center
													NewCamera.target.pos = SourceCenter
													NewCamera.pos = [(SourceCenter.x + 0.5), (SourceCenter.y - .75), (SourceCenter.z + 0.25)]
													NewDir = normalize (NewCamera.pos - NewCamera.target.pos)
													NewCamera.pos = NewCamera.target.pos + NewDir * x
													b = (render camera:NewCamera outputwidth:640 outputheight:480 vfb:off)
													b.filename = (New_Thumbs_location + "\\" + o.name + ".jpg")
													save b
													delete NewCamera.target
													delete NewCamera
													hide o
													if ProgressUpdate (i * progscale) == false then exit
												) else (
													ThumbExitCode = #bad
												)
											)
									)
								for o in objects do unhide o
								ProgressEnd()		
								if ThumbExitCode == #bad do (MessageBox "Some objects were NOT thumbnailed because of improper names")
							)
					) else (
						MessageBox "You must choose your name from the Artists list!"
					)
			) else (
				MessageBox "You must enter a description at least\n25 characters long for your asset!"
			)	
	) else (
		MessageBox "You must define a game area first!"
	)
	) -- end submit
)
