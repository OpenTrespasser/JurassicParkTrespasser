-- **********************************************************************************************
-- *
-- * Copyright © DreamWorks Interactive, 1997
-- *
-- * Contents:
-- *						Implementation of Terrain Exporter
-- * Bugs:
-- *
-- * To do:
-- *
-- * Notes:
-- *
-- **********************************************************************************************
fn TerrainFlt obj = findstring obj.name "!Terrain" == 1

fn LoadTriFile TriFile = 
 ( 
  local VertArray, FaceArray, ObjectFileName, CVal 
  VertArray = #() 
  FaceArray = #() 
  f = openFile TriFile 
  CVal = readValue f 
  for i = 1 to CVal do 
   ( 
    CFace = [(readDelimitedString f " ") as integer + 1, (readDelimitedString f " ") as integer + 1, (readDelimitedString f "\n") as integer + 1] 
    append FaceArray CFace 
   ) 
  CVal = readValue f 
  for i = 1 to CVal do 
   ( 
    CVert = [(readDelimitedString f " ") as float, (readDelimitedString f " ") as float, (readDelimitedString f "\n") as float] 
		if CVert.z < 0.0 do CVert.z = 0
   	append VertArray CVert 
   ) 
  close f 
  m = mesh vertices:VertArray faces:FaceArray 
  m.name = ("!Terrain_" + localtime)
  m.pivot = m.center 
  max select none 
  selectmore m 
  max zoomext sel all 
 ) 

fn roundTo val n = 
	(
		local mult = 10.0 ^ n
		(floor ((val * mult) + 0.5)) / mult
	)

Utility TRRGenerator "TerrainExporter"
	(
		local UpdateFreq = 500, 
					GameAreaPositions = #([-1792,-768,0], [-1792,0,0], [-1024,256,0], [0,512,0], [0,-256,0], [256,-768,0], [768,1024,0], [768,1024,0], [1792,0,0], [1280, 0, 0])
				
		dropdownlist ExportResolution "Export Resolution:" items:#("1/4 Meter", "1/2 Meter", "1 Meter", "2 Meter", "4 Meter", "8 Meter", "16 Meter") selection:1
		dropdownlist ExportArea "Game Area:" items:#("Beach", "Jungle Road", "Plantation House", "Industrial Jungle", "InGEN Town", "Plains", "Pine Valley","InGEN Lab", "Ascent", "Summit", "<< undefined >>", "") selection:12 height:12

		group "Auto Conversion"
			(
				checkbox AutoConvert "Auto Convert TRR File"
				spinner QuantizationValue "Quantization bit depth:" range:[8,24,16] type:#integer fieldwidth:25
				spinner QuantErrorValue "Quantization Error:" range:[0, 1, 0.1] fieldwidth:35
				checkbox ReloadTerrain "Load Terrain back in" 
			)

		checkbox genTrnPlc "Generate TrnPlc Object" checked:false
		label blank1
		group "Export Status"
			(
				label status0
				label status1
				label status2
				label status3
				label status4
			)
		label blank0
		pickbutton ChooseTerrain "Choose Terrain" width:140 filter:TerrainFlt
		button LoadTerrain "Load TRI File" width:140
		
		on LoadTerrain pressed do
			(
				if (TriFileName = getOpenFilename "Choose .TRI File") != undefined do
					(
					  VertArray = #() 
					  FaceArray = #() 
					  f = openFile TriFileName 
					  CVal = readValue f
						progressStart "Reading Faces...."
					  for i = 1 to CVal do 
					   ( 
					   	ProgressUpdate ((i/CVal as float) * 100)
					    CFace = [(readDelimitedString f " ") as integer + 1, (readDelimitedString f " ") as integer + 1, (readDelimitedString f "\n") as integer + 1] 
					    append FaceArray CFace 
					   ) 
						ProgressEnd()
					  CVal = readValue f 
						progressStart "Reading Verts...."
					  for i = 1 to CVal do 
					   ( 
					   	ProgressUpdate ((i/CVal as float) * 100)
					    CVert = [(readDelimitedString f " ") as float, (readDelimitedString f " ") as float, (readDelimitedString f "\n") as float] 
						   append VertArray CVert 
					   ) 
						ProgressEnd()
					  close f 
					  m = mesh vertices:VertArray faces:FaceArray 
					  m.name = ("!Terrain_" + localtime)
					  m.pivot = m.center 
					  max select none 
					  selectmore m 
					  max zoomext sel all 
						messageBox (VertArray.count as string + " Vertices\n" + FaceArray.count as string + " Faces\n")
					)
			)


		on ChooseTerrain picked obj do
			(
				if ExportArea.selection != 12 then
					(
						if (saveFilename = getsavefilename caption:"Specify TRR File") != undefined do
							(
								Terrain = obj
								ConvertToMesh Terrain
								if ExportArea.selection != 11 do
									(
										tPos = terrain.pos
										gPos = GameAreaPositions[ExportArea.selection]
										terrain.pos = [gPos.x, gPos.y, tPos.z]
									)
								status0.text = status1.text = status2.text = status3.text = status4.text = ""
								if ExportResolution.selection == 1 do (TerrainRez = 0.25)
								if ExportResolution.selection == 2 do (TerrainRez = 0.5)
								if ExportResolution.selection == 3 do (TerrainRez = 1)
								if ExportResolution.selection == 4 do (TerrainRez = 2)
								if ExportResolution.selection == 5 do (TerrainRez = 4)
								if ExportResolution.selection == 6 do (TerrainRez = 8)
								if ExportResolution.selection == 7 do (TerrainRez = 16)
								if ExportResolution.selection == 8 do (TerrainRez = 32)
								if ExportResolution.selection == 9 do (TerrainRez = 64)
								nv = Terrain.numverts
								status0.text = (TerrainRez as string + "M Resolution")
								status1.text = (nv as string + " Vertices")
								for i = 1 to nv do
									(
										CVert = (getvert Terrain i)
										-- first the x coordinate...
										x = (roundTo (CVert.x as float / TerrainRez as float) 0) * TerrainRez
										y = (roundTo (CVert.y as float / TerrainRez as float) 0) * TerrainRez
										Cvert = [x,y,CVert.z]
										SetVert Terrain i CVert
									)
								update Terrain
								TerrainExtents = (Terrain.max - Terrain.min)
								NegZValues = false
								f = createfile savefilename
								for i = 1 to nv do
									(
								    if mod i UpdateFreq == 0 then (status3.text = ("Vert " + i as string + " of " + nv as string))
										CVert = getvert terrain i
										-- check for -z values
										if CVert.z < 0.0 do
											(
												NegZValues = true
												CVert.z = 0.0
												setVert terrain i CVert
											)
										format "% % %\n" (roundTo Cvert.x 2) (roundTo Cvert.y 2) Cvert.z to:f	
									)
								update Terrain
								status3.text = "Verts Done"
								if NegZValues == true do (MessageBox "Negative Z values found!\n said values set to 0.00")
								TRRFileName = (FileNamefromPath saveFilename)
								if AutoConvert.checked do
									(
										doscommand ("K:\\Art\\Tools\GuiApp\GuiApp.exe /c" + TRRFileNAme + ":" + ((QuantizationValue.value as integer) as string))
										-- load the quantized terrain back into MAX
										if ReloadTerrain.checked do
											(
												doscommand ("K:\\Art\\Tools\GuiApp\GuiApp.exe /c" + (substring TRRFileNAme 1 (TRRFileNAme.count - 4)) + ".wtd:" + QuantErrorValue.value as string)
												TriFilename = (substring TRRFileNAme 1 (TRRFileNAme.count - 4) + ".tri")
												LoadTriFile TriFilename
											)
									)
								if genTrnPlc.checked do
									(
										-- track down and delete any terrain placement objects in the scene.
										sel = #()
										for o in objects do
											(
												str = "string Class = \"TerrainPlacement\""
												CBuffer = getUserPropBuffer o
												if findString CBuffer str != undefined do append sel o
											)
										TRRFileName = substring TRRFileName 1 (TRRFileName.count - 4)
										b = box()
										b.width = b.height = b.length = 0.25
										b.pivot = b.center
										b.pos = [0,0,-10]
										b.mapcoords = true
										MaterialA = StandardMaterial()
										MaterialA.name = "Terrain Placement Object"
										MaterialA.diffusemap = bitmaptexture()
										if ExportArea.selection == 1 do bFilename = "\\\Jpii_pc\\Trespass\Art\\Terrain\\TerrainPlacementTextures\\TBEBaset2.bmp"
										if ExportArea.selection == 2 do bFilename = "\\\Jpii_pc\\Trespass\Art\\Terrain\\TerrainPlacementTextures\\TJRBaset2.bmp"
										if ExportArea.selection == 3 do bFilename = "\\\Jpii_pc\\Trespass\Art\\Terrain\\TerrainPlacementTextures\\TPHBaset2.bmp"
										if ExportArea.selection == 4 do bFilename = "\\\Jpii_pc\\Trespass\Art\\Terrain\\TerrainPlacementTextures\\TIJBaset2.bmp"
										if ExportArea.selection == 5 do bFilename = "\\\Jpii_pc\\Trespass\Art\\Terrain\\TerrainPlacementTextures\\TTOBaset2.bmp"
										if ExportArea.selection == 6 do bFilename = "\\\Jpii_pc\\Trespass\Art\\Terrain\\TerrainPlacementTextures\\TPLBaset2.bmp"
										if ExportArea.selection == 7 do bFilename = "\\\Jpii_pc\\Trespass\Art\\Terrain\\TerrainPlacementTextures\\TPVBaset2.bmp"
										if ExportArea.selection == 8 do bFilename = "\\\Jpii_pc\\Trespass\Art\\Terrain\\TerrainPlacementTextures\\TLABaset2.bmp"
										if ExportArea.selection == 9 do bFilename = "\\\Jpii_pc\\Trespass\Art\\Terrain\\TerrainPlacementTextures\\TASBaset2.bmp"
										if ExportArea.selection == 10 do bFilename = "\\\Jpii_pc\\Trespass\Art\\Terrain\\TerrainPlacementTextures\\TSUBaset2.bmp"
										if ExportArea.selection == 11 do bFilename = "\\\Jpii_pc\\Trespass\Art\PDummyMapt2.bmp"
										MaterialA.diffusemap.filename = bFilename
										b.mat = MaterialA
										setUserPropBuffer b ("string Class = \"TerrainPlacement\";\r\r\nbool MipMap = false;\r\r\nstring File = \"" + TRRFileName + "\";\r\r\n#quant_group(Terrain)\r\r\n#export_first\r\r\n")
										b.name = ("TrnPlacement-00")
										b.wirecolor = (color 255 0 0)
										convertToMesh b
										select sel
										MAX delete
									)
								close f
								-- gc()
								messageBox "Terrain Exporter Done!"
							)
					) else (
						MessageBox "You must choose a game area first."
					)
			)
	)