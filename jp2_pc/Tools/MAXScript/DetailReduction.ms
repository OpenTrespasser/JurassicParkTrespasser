fn PowOfLimit x y = 
	(
		if x > y then
			(
				SclDn = 0
				while x > 256 do	
					(
						x *= 0.5
						SclDn += 1
					)
				if SClDn > 0 do
					for i = 1 to SClDn do y *= 0.5
			) else (
				SclDn = 0
				while y > 256 do	
					(
						y *= 0.5
						SclDn += 1
					)
				if SClDn > 0 do
					for i = 1 to SClDn do x *= 0.5
			)
		if x < 8 do x = 8
		if y < 8 do y = 8
		return [x,y]
	)

-- returns a power of two number based on user input of higher, lower, or nearest
fn NearPowOfTwo num weight = 
	(
		-- Highest
		if weight == 1 do
			(
				num2 = num
				counter = 0
				while num >= 1.0 do
					(
						num = (num * 0.5)
						counter = counter + 1
					)
				p2 = 2^(counter)
				return p2
			)

		-- Lowest
		if weight == 2 do
			(
				num2 = num
				counter = 0
				while num >= 1.0 do
					(
						num = (num * 0.5)
						counter = counter + 1
					)
				p2 = 2^(counter - 1)
				return p2	
			)

		-- Nearest
		if weight == 3 do
			(
				num2 = num
				counter = 0
				while num >= 1.0 do
					(
						num = (num * 0.5)
						counter = counter + 1
					)
				p2 = 2^(counter - 1)
				if (num2 > p2 * 1.5) then
					return (p2 * 2)
				else 
					return p2	
			)
	)

-- returns the index of the vertex with the lowest pos.z value
fn LowestVert obj =
	(
		local LowVertPos = obj.max.z
		local LowestVert = -1
		nv = obj.numverts
		for i = 1 to nv do
			(	
				local Cvert = getvert obj i
				if Cvert.z < LowVertPos do
				(
					LowVertPos = Cvert.z
					LowestVert = i
				)
			)	
		return LowestVert
	)

fn trunkCenter CObj thresh = 
	(
		-- first, we find the lowest vertex in the model
		LowVert = lowestVert CObj
		-- searching from the bottom
		BottomPoint = CObj.min.z
		SetVertSelection CObj #()
		update CObj
		nv = CObj.numverts
		Verts = for i = 1 to nv collect (getvert CObj i)
		-- first we find the vertex distance furthest from the center of the object
		FarVert = 0.0
		VertexDistances = #()
		-- we put all the vertex distances into an array, and find the one furthest from the center of the mesh
		for i = 1 to nv do
			(
				append VertexDistances Verts[i].z
				if VertexDistances[i] < FarVert do FarVert = VertexDistances[i]
			)
		-- now we flag any vert within the threshold and put it  into an array
		VertSelection = #()
		for i = 1 to nv do
				if VertexDistances[i] <= (BottomPoint + thresh) do (append VertSelection i)
		SetVertSelection CObj VertSelection
		verts = [0,0,0]
		for i = 1 to VertSelection.count do
			Verts = Verts + (getVert CObj VertSelection[i])
		
		VertCenter = [(Verts.x / VertSelection.count), (Verts.y / VertSelection.count), (Verts.z / VertSelection.count)]
		return VertCenter
	)

-- crops a bitmap to new supplied dimensions
fn CropBitmap SourceImage StartPos NewImageSize = 
	(
		width = SourceImage.width
		height = SourceImage.height
		bmap = BitMap NewImageSize.x NewImageSize.y
		NewY = 0
		for y = StartPos.y to (StartPos.y + NewImageSize.y) do
			(
				CRow = getPixels SourceImage [StartPos.x, y] NewImageSize.x
				setPixels bmap [0,NewY] CRow
				NewY = NewY + 1
			)
		return bmap
	)

-- takes a bitmap and returns an array of the following:
-- [1] a new bitmap that only contains black or white (on/off transparencey)
-- [2] the start point [x,y] for cropping
-- [3] the area of a closest fit rectangle about the opaque area of the opacity map from the start point at [2]
fn ThresholdBitmap bmap Thresh = 
	(
		local NMap = copy BMap
		StartPoint = [0,0]
		NewSize = [NMap.width, NMap.height]
		width = NMap.width
		height = NMap.height
		AreaArray = #()
		for y = 1 to height do
			(
				AreaArray[y] = #()
				CRow = getpixels NMap [0, (y - 1)] width
				-- convert the row of color pixels to values
				LumRow = for i = 1 to CRow.count collect (CRow[i].value)
				-- convert this row based on the threshold value
				for i = 1 to LumRow.count do
					if LumRow[i] > Thresh then
						(
							AreaArray[y][i] = true
							LumRow[i] = 255
						) else (
							AreaArray[y][i] = false
							LumRow[i] = 0
						)
				-- convert the values back into colors
				for i = 1 to LumRow.count do
					CRow[i] = (color LumRow[i] LumRow[i] LumRow[i])
				-- write the row back into the bitmap
				setpixels NMap [0, (y - 1)] CRow
			)

		maxX = 0
		minX = 256
		maxY = 0
		minY = 256

		for y = 1 to AreaArray.count do
			(
				CRow = AreaArray[y]
				for x = 1 to CRow.count do
					if CRow[x] == true do
						(
							if x > maxX do maxX = x
							if y > maxY do maxY = y
							if x < minX do minX = x
							if y < minY do minY = y
						)
			)
		NewStartPoint = [(minX - 1), (minY - 1)]
		NewMapSize = [(maxX - minX) + 1, (maxY - minY) + 1]
		if NewStartPoint == [255,255] or NewMapSize == [-255,-255] do
			(
				format "Bad OpacityMap data\n"
				NewStartPoint = [0,0]
				NewMapSize = NewSize
			)
		return #(NMap, NewStartPoint, NewMapSize)
	)

-- ************************************************************************************************
-- * Start Utility
-- ************************************************************************************************
Utility DetailReduction "Detail Reduction"
	(
		local DRRollout, WorkingFolder, AttachedTrunk, SingleObject


		rollout DRRollout "Detail  Reduction"
			(
				local CObj, CTrunk, CBranches, 
							SunArray = #([0.163287,-0.71436,0.680461], [0.150228,-0.623607,0.767167], [0.13906,-0.522188,0.841417], [0.129977,-0.411837,0.90194], [0.123134,-0.294445,0.947703], [0.118652,-0.17202,0.977922], [0.116608,-0.0466583,0.992081], [0.11704,0.0794943,0.989941], [0.11994,0.204279,0.971537], [0.125262,0.32556,0.937187], [0.132916,0.441262,0.88748])

				group "General Options"
					(
						checkbox DebugMode "Debug Mode" checked:false
						dropDownList GameArea "Game Area Lighting:" items:#("Beach", "Jungle Road", "Platation House", "Industrial Jungle", "InGEN Town", "Plains", "PineValley", "Shore", "InGEN Lab", "Ascent", "Summit") selection:2 height:12
						checkBox renderSeperate "Detail reduce branches seperate from trunk"
						colorpicker AmbientCol "Ambient: " color:(color 90 90 90) align:#right offset:[120,-20]
						checkbox CenterToBase "Center camera based on trunk bottom" checked:false
						spinner CMult "Multiplier: " range:[.1,5,.7] fieldwidth:35 offset:[0,-20]
						spinner TrunkCenterThresh "Threshold:" range:[0,10,0.5] fieldWidth:45 align:#left enabled:false
					)

				group "Generated Texture Map Options"
					(
						spinner OpacityThresh "Opacity Map Threshold: " range:[0,255,127] type:#integer fieldwidth:45 offset:[0,0]
						Checkbox RescaleBitmaps "Rescale bitmaps" offset:[0,-20] checked:true
						radiobuttons sizeMaps labels:#("Higher power of 2", "Lower power of 2", "Nearest power of 2") default:1 columns:3 align:#left
					)

				group "Trunk Options"
					(
						checkbox GenTrunk "Create" checked:true
						checkbox CameraAtEyeHeight "Camera at eye level" align:#center offset:[0,-20]
						spinner BitMapSize "Map Size:" range:[8,256,256] type:#integer align:#left fieldwidth:35
						spinner NumViews "# Views: " range:[2,9,5] type:#integer fieldwidth:35 offset:[0,-21] align:#center
						spinner DRLevel "Detail Level: " range:[1,4,1] type:#integer fieldwidth:25 offset:[0,-21] align:#right
						checkbox OverRideBitmapSize "Overide Output Bitmap Size"
						spinner TrunkWidthOveride "Width:" range:[8,256,256] type:#integer fieldwidth:45 align:#left enabled:false
						spinner TrunkHeightOveride "Height:" range:[8,256,256] type:#integer fieldwidth:45 offset:[-150,-20] enabled:false
					)

				group "Branch Options"
					(
						checkbox GenBranches "Create" enabled:false checked:true
						checkbox BranchCameraAtEyeHeight "Camera at eye level" align:#center offset:[0,-20] enabled:false checked:true
						spinner BranchBitMapSize "Map Size:" range:[8,256,128] type:#integer align:#left fieldwidth:35 enabled:false
						spinner BranchNumViews "# Views: " range:[2,9,5] type:#integer fieldwidth:35 align:#center offset:[0,-21] enabled:false
						spinner BranchDRLevel "Detail Level: " range:[1,4,1] type:#integer fieldwidth:25 offset:[0,-21] align:#right enabled:false
						checkbox BranchOverRideBitmapSize "Overide Output Bitmap Size" enabled:false
						spinner BranchWidthOveride "Width:" range:[8,256,256] type:#integer fieldwidth:45 align:#left enabled:false
						spinner BranchHeightOveride "Height:" range:[8,256,256] type:#integer fieldwidth:45 offset:[-150,-20] enabled:false
					)

				button InitializeObjects "Initialize Selected Objects" height:30 align:#left width:180 offset:[-10,0]
				Button CreateDRObject "Create Detail Reduced Version" height:30 align:#right offset:[10,-35] width:180

				on OverRideBitmapSize changed state do
					if OverRideBitmapSize.checked then
						TrunkWidthOveride.enabled = TrunkHeightOveride .enabled = true
					else					
						TrunkWidthOveride.enabled = TrunkHeightOveride .enabled = false

				on BranchOverRideBitmapSize changed state do
					if BranchOverRideBitmapSize.checked then
						BranchWidthOveride.enabled = BranchHeightOveride .enabled = true
					else					
						BranchWidthOveride.enabled = BranchHeightOveride .enabled = false

				on DRLevel changed state do BranchDRLevel.value = DRLevel.value

				on BranchDRLevel changed state do DRLevel.value = BranchDRLevel.value

				on renderSeperate changed state do
					if renderSeperate.checked then
						BranchBitMapSize.enabled = BranchNumViews.enabled = BranchDRLevel.enabled = GenBranches.enabled = BranchCameraAtEyeHeight.enabled = BranchOverRideBitmapSize.enabled = true
					else
						BranchBitMapSize.enabled = BranchNumViews.enabled = BranchDRLevel.enabled = GenBranches.enabled = BranchCameraAtEyeHeight.enabled = BranchOverRideBitmapSize.enabled = BranchWidthOveride.enabled = BranchHeightOveride.enabled = false

				on CenterToBase changed state do
					if CenterToBase.checked then
						TrunkCenterThresh.enabled = true
					else
						TrunkCenterThresh.enabled = false

-- *******************************************************************************************************
-- * Object Initialization
-- *******************************************************************************************************
				on InitializeObjects pressed do
					(
						SingleObject = false
						AttachedTrunk = false
						MagicIndex = undefined
						ObjArray = for obj in selection collect obj
						if ObjArray.count == 1 or ObjArray.count == 2 then
							(
								if DebugMode.checked do
									format "********************************\nInitializing Objects\n********************************\n"
								if renderSeperate.checked then
									(
										if ObjArray.count == 1 then
											(
												CTrunk = ObjArray[1]
											) else ( 
												if ObjArray[1].pivot.z < ObjArray[2].pivot.z then
													(
														CTrunk = ObjArray[1]
														CBranches = ObjArray[2]
													) else (
														CTrunk = ObjArray[2]
														CBranches = ObjArray[1]
													)
											)
									) else (
										if ObjArray.count <= 2 and ObjArray.count != 0 then
											(
												if ObjArray.count == 1 then
													(
														CTrunk = copy ObjArray[1]
														CTrunk.mat = copy ObjArray[1].mat
														SingleObject = true
														hide ObjArray[1]
													) else ( 
														if ObjArray[1].pivot.z < ObjArray[2].pivot.z then
															(
																CTrunk = copy ObjArray[1]
																CTrunk.mat = copy CTrunk.mat
																CBranches = copy ObjArray[2]
																CBranches.mat = copy CBranches.mat
															) else (
																CTrunk = copy ObjArray[2]
																CTrunk.mat = copy CTrunk.mat
																CBranches = copy ObjArray[1]
																CBranches.mat = copy CBranches.mat
															)
														hide ObjArray[1]
														hide ObjArray[2]
														attach CTrunk CBranches
														CBranches = undefined
														AttachedTrunk = true
													)
											) else (
												MessageBox "Feed this tool 1 or 2 objects only"
											)
									)
								for i = 1 to CTrunk.name.count do
									if CTrunk.name[i] == "~" do
										(
											MagicIndex = i
											if debugMode.checked do
												format "\"~\" found at position: %\n" MagicIndex
										)
								-- find the area from the name of the trunk object
								Area = ""
								RotationValue = 0
								if MagicIndex != undefined then
									(
										Area = substring CTrunk.name (Magicindex + 1) 2
										if Area == "BE" do GameArea.selection = 1
										if Area == "JR" do GameArea.selection = 2
										if Area == "PH" do GameArea.selection = 3
										if Area == "IJ" do GameArea.selection = 4
										if Area == "TO" do GameArea.selection = 5
										if Area == "PL" do GameArea.selection = 6
										if Area == "PV" do GameArea.selection = 7
										if Area == "SH" do GameArea.selection = 8
										if Area == "LA" do GameArea.selection = 9
										if Area == "AS" do GameArea.selection = 10
										if Area == "SU" do GameArea.selection = 11
										-- get the rotation value from the object and convert it to an integer
										RotationValue = CTrunk.name[MagicIndex + 3] as integer
										DetailReductionValue = DRLevel.value
										if debugMode.checked do
											(
												format "Area: %\n" Area
												format "Rotation Value: %\n" RotationValue
												format "detail Reduction Value: %\n" DetailReductionValue
											)
									) else (
										MessageBox "Please make sure you have\nthe area information defined!\n\ni.e. \"VMyPlant~IJ10-00\""
									)
								if DebugMode.checked do
									format "********************************\nObject Initializing Done!\n********************************\n"
							) else (
								MessageBox "Feed this tool 1 or 2 objects only"
							)
					)

-- *******************************************************************************************************
-- * Generate Detail Reduced version of the object
-- *******************************************************************************************************
				on CreateDRObject pressed do
					(
						if CTrunk != undefined then
							(
								for i = 1 to 2 do
									(
										for o in objects do hide o
										if i == 1 do unhide CTrunk
										if CBranches != undefined do
											if i == 2 do unhide CBranches
		
										if renderSeperate.checked and i == 2 then
											NumVs = BranchNumViews.value as integer
										else
											NumVs = NumViews.value as integer
		
										CObj = undefined
										if i == 1 then
											if CTrunk != undefined do
												CObj = CTrunk
										else
											if CBranches != undefined do
												CObj = CBranches
		
										-- these arrays will hold the final scaled/cropped map data
										FinalOpacities = #()
										FinalTextures = #()
										InTexNameArray = #()
										OutTexNameArray = #()
										InOpNameArray = #()
										OutOpNameArray = #()
										
										if CObj != undefined then
											(
												if (GenTrunk.checked and i == 1) or (GenBranches.checked and i == 2) do
													(
														OriginalMat = copy CObj.mat
														-- setup Area object info for tracking
														AreaKey = ""
														if GameArea.selection == 1 do (AreaKey = "BE")
														if GameArea.selection == 2 do (AreaKey = "JR")
														if GameArea.selection == 3 do (AreaKey = "PH")
														if GameArea.selection == 4 do (AreaKey = "IJ")
														if GameArea.selection == 5 do (AreaKey = "TO")
														if GameArea.selection == 6 do (AreaKey = "PL")
														if GameArea.selection == 7 do (AreaKey = "PV")
														if GameArea.selection == 8 do (AreaKey = "SH")
														if GameArea.selection == 9 do (AreaKey = "LA")
														if GameArea.selection == 10 do (AreaKey = "AS")
														if GameArea.selection == 11 do (AreaKey = "SU")
														-- DL is the detail level
														DL = (DRLevel.value as integer) as string
														if renderSeperate.checked and i == 2 do
															DL = (BranchDRLevel.value as integer) as string
														-- initialize some arrays
														BitMapArray = #()
														OpacityMapArray = #()
														DRObjectsArray = #()
														-- Set the Background color to black
														backgroundColor = (color 0 0 0)
														-- grab and set the Ambient light color to what's specified in the interface
														AmbientColor = AmbientCol.color
														-- setup field of vision
														fov = 10
														CObjSize = (CObj.max - CObj.min)
														-- create a cricle for the camera path
														CameraPath = circle()
		
														-- if center to base is checked, we determine the center of the lowest set of
														-- vertices in the mesh (the bottom of the trunk) and set the x,y center position to that.
														-- This way, we can make sure that the base of the trunk in each image view lies at the same position.
														-- Otherwise, we just use the object center
														if CenterToBase.checked then
															(
																if i == 2 then
																	(
																		ObjCenter = CObj.center
																	) else (
																		ObjCenter = trunkCenter CObj TrunkCenterThresh.value
																		ObjCenter = [ObjCenter.x, ObjCenter.y, CObj.center.z]
																	)
															) else (
																ObjCenter = CObj.center
															)
														CameraPath.pos = ObjCenter
														rotate CameraPath 90 Z_Axis
														theta = fov
														offsetHeight = false
														-- here, we find the largest distance of either the height,
														-- or the distance between the x,y coordinates of the .min and .max (thus the maximun diameter)
														if (distance [CObj.max.x,CObj.max.y] [CObj.min.x,CObj.min.y]) > (CObj.max.z - CObj.min.z) then
															(
																d = (distance [CObj.max.x,CObj.max.y] [CObj.min.x,CObj.min.y])
																offsetHeight = true
																if DebugMode.checked do format "Using objects maximum DIAMETER for largest dimension...\n"
															) else (
																d = (CObj.max.z - CObj.min.z)
																if DebugMode.checked do format "Using objects maximum HEIGHT for largest dimension...\n"
															)
		
														-- setup the camera and it's position so that the object lies square in it's viewport
														h = (d / (2 * sin(theta / 2)))
														x = h * (cos(theta))
														NewCamera = targetCamera()
														NewCamera.fov = fov
														NewCamera.target = targetObject()
														NewCamera.target.pos = ObjCenter
														NewDir = normalize (NewCamera.pos - NewCamera.target.pos)
													 	NewCamera.pos = NewCamera.target.pos + NewDir * x
														CameraDist = distance NewCamera.pos Newcamera.target.pos
														CameraPath.radius = CameraDist
														NewCamera.pos.controller = path()
														NewCamera.pos.controller.path = CameraPath
		
														BoxDimension = ((distance NewCamera.pos NewCamera.target.pos) * (tan(fov * 0.5))) * 2
		
														-- if we're using the largest diameter of the object, we need to change the object height so
														-- that the base of the new detail reduced object matches the base of th object itself.
														if (offsetHeight == true) and (i != 2)  do
															(
																Objheight = (CObj.max.z - CObj.min.z) * 0.5
																ViewHeight = (((distance NewCamera.pos NewCamera.target.pos) * (tan(fov * 0.5))) * 2) * 0.5
																HeightOffset = abs(Objheight - ViewHeight)
																CameraPath.pos = [CameraPath.pos.x, CameraPath.pos.y, CObj.center.z]
		--													CameraPath.pos = [CameraPath.pos.x, CameraPath.pos.y, CameraPath.pos.z + HeightOffset]
																NewCamera.target.pos = CameraPath.pos
																if DebugMode.checked do format "Camera Offset height:%\n" HeightOffset
															)
		
														-- set the camera height to be at eye level from the base of the tree
														-- eye level is 1.65 Meters
														if CameraAtEyeHeight.checked and i == 1 do
															(
																newCameraHeight = (CObj.center.z - (CObj.max.z - CObj.min.z) * 0.5) + 1.65
																CameraPath.pos = [CameraPath.pos.x, CameraPath.pos.y, newCameraHeight]
															)
		
														if BranchCameraAtEyeHeight.checked and i == 2 do
															(
																-- we base eye height for the branches off the dimensions of the trunk
																newCameraHeight = (CTrunk.center.z - (CTrunk.max.z - CTrunk.min.z) * 0.5) + 1.65
																CameraPath.pos = [CameraPath.pos.x, CameraPath.pos.y, newCameraHeight]
															)
		
														-- setup the animation of the camera about the path based on the number of views the user wants to use
														deletekeys NewCamera.pos.controller #allkeys
														c = NewCamera.pos.controller
														addNewKey c 0
														addNewKey c NumVs
														animate on
															(
																at time 0 c.percent = 0.0
																at time NumVs c.percent = 100.0
															)
														-- makes sure any and all lights are deleted
														for l in lights do delete l
														CLight = freespot()
														CLight.dir = SunArray[GameArea.selection]
														CLight.HotSpot = 60
														CLight.color = (color 255 255 255)
														CLight.Multiplier = CMult.value
														SunDir = CLight.dir
														theta = CLight.HotSpot
														d = distance CObj.max CObj.min
														x = (d / (2 * sin(theta / 2)))
														CLight.pos = (CObj.center + SunDir * 1000)
		
														-- determine the bitmap size
														if renderSeperate.checked and i == 2 then
															BitMapSizeValue = BranchBitMapSize.value as integer
														else
															BitMapSizeValue = BitMapSize.value as integer
		
														-- Render all the texture maps
														for j = 1 to NumVs do
															(
																Append BitMapArray (render camera:NewCamera outputwidth:BitMapSizeValue outputheight:BitMapSizeValue frame:j vfb:off)
																if debugMode.checked do display BitMaparray[j]
															)

														if classof CObj.mat == StandardMaterial do
															(
																CObj.mat.diffuse = (color 255 255 255)
																CObj.mat.DiffuseMapEnable = false
																CObj.mat.BumpMapEnable = false
																CObj.mat.shinestrength = 0.0
																CObj.mat.selfIllum = 100
															)
		
														if classof CObj.mat == multiMaterial do
															(
																ns  = CObj.mat.numsubs
																for j = 1 to ns do
																	(
																		CObj.mat[j].diffuse = (color 255 255 255)
																		CObj.mat[j].DiffuseMapEnable = false
																		CObj.mat[j].BumpMapEnable = false
																		CObj.mat[j].shinestrength = 0.0
																		CObj.mat[j].selfIllum = 100
																	)
															)
														
														-- render all the opacity maps
														if renderSeperate.checked and i == 2 then
															BitMapSizeValue = BranchBitMapSize.value as integer
														else
															BitMapSizeValue = BitMapSize.value as integer
														for j = 1 to NumVs do
															(
																Append OpacityMapArray (render camera:NewCamera outputwidth:BitMapSizeValue outputheight:BitMapSizeValue frame:j vfb:off)
																if debugMode.checked do display OpacityMaparray[j]
															)

														CObj.mat = OriginalMat

														if classof CObj.mat == StandardMaterial do
															(
																CObj.mat.DiffuseMapEnable = true
																CObj.mat.BumpMapEnable = true
																CObj.mat.shinestrength = 0.0
																CObj.mat.selfIllum = 0.0
															)

														if classof CObj.mat == multiMaterial do
															(
																ns  = CObj.mat.numsubs
																for j = 1 to ns do
																	(
																		CObj.mat[j].DiffuseMapEnable = true
																		CObj.mat[j].BumpMapEnable = true
																		CObj.mat[j].shinestrength = 0.0
																		CObj.mat[j].selfIllum = 0.0
																	)
															)

														CNamePrefix = ""
														CNameSuffix = ""

														for j = 1 to CObj.name.count do
															if CObj.name[j] == "~" do
																MagicIndex = j
														if MagicIndex != undefined then
															(
																CNamePrefix = substring CObj.name 1 (MagicIndex - 1)
																CNameSuffix = ((substring CObj.name (MagicIndex + 1) 3) + ((DRLevel.value as integer) as string))
															) else (
																for j = 1 to CObj.name.count do
																	if CObj.name[j] == "-" do
																		MagicIndex = j
		
																if magicIndex == undefined do
																	MagicIndex = CObj.name.count
		
																CNamePrefix = substring CObj.name 1 (MagicIndex - 1)
																CNameSuffix = ((substring CObj.name (MagicIndex + 1) 3) + ((DRLevel.value as integer) as string))
															) 

														ProgressStart "Generating Views..."
														for t = 1 to NumVs do
															(
																at time t
																	(
																		ProgressUpdate ((t/NumVs as float) * 100)
																		-- ************************************************************
																		-- ** Process the bitmaps**************************************
																		-- ************************************************************
																		-- process the bitmaps for size/scale
																		if DebugMode.checked do format "Processing Bitmaps...\n"
																		BitmapInfo = ThresholdBitmap OpacityMapArray[t] (OpacityThresh.value as integer)
																		format "%\n" BitmapInfo
																		display OpacityMapArray[t]
																		-- calculate the width and height scalar values for the geometry based on the new cropped areas
																		WidthScalar = BitmapInfo[3].x as float / OpacityMapArray[t].width as float
																		HeightScalar = BitmapInfo[3].y as float / OpacityMapArray[t].height as float
		
																		-- crop the bitmaps
																		OpacityMapArray[t] = CropBitmap BitmapInfo[1] BitmapInfo[2] BitmapInfo[3]
																		BitMapArray[t] = CropBitmap BitmapArray[t] BitmapInfo[2] BitmapInfo[3]

																		NewBitMapHeight = BitmapInfo[3].y
																		NewBitMapWidth = BitmapInfo[3].x
																		
																		-- Extra checking for illegal map sizes (> 256, < 8)
																		if NewBitMapHeight > 256 do NewBitMapHeight = 256
																		if NewBitMapWidth > 256 do NewBitMapWidth = 256
																		if NewBitMapHeight < 8 do NewBitMapHeight = 8
																		if NewBitMapWidth < 8 do NewBitMapWidth = 8

																		if RescaleBitmaps.checked do
																			(
																				reCalc = true
																				if i == 1 and OverRideBitmapSize.checked do
																						(
																							NewBitMapWidth = TrunkWidthOveride.value
																							NewBitMapHeight = TrunkHeightOveride.value
																							reCalc = false
																						)

																				if i == 2 and BranchOverRideBitmapSize.checked do
																						(
																							NewBitMapWidth = BranchWidthOveride.value
																							NewBitMapHeight = BranchHeightOveride.value
																							reCalc = false
																						)

																				if reCalc == true do
																					(
																						-- calculate the new bitmap dimensions to the nearest power of two size
																						NewBitMapWidth = NearPowOfTwo NewBitMapWidth sizeMaps.state
																						NewBitMapHeight = NearPowOfTwo NewBitMapHeight sizeMaps.state
																						NSize = (PowOfLimit NewBitMapWidth NewBitMapHeight)
																						NewBitMapWidth = NSize.x as integer
																						NewBitMapHeight = NSize.y as integer
																					)
																			)

																		if DebugMode.checked do
																			format "Initial BitmapSize: %\t%\nNew Bitmap size: %\t%\n" BitmapInfo[3].x BitmapInfo[3].y NewBitMapWidth NewBitMapHeight

																		FinalOpName = (WorkingFolder + CNamePrefix + t as string + "~" + CNameSuffix + "o8.bmp")
																		FinalTexName = (WorkingFolder + CNamePrefix + t as string + "~" + CNameSuffix + "t2.bmp")
																		InTexName = (WorkingFolder + "t" + t as string + ".bmp")
																		OutTexName = (WorkingFolder + "t_" + t as string + ".bmp")
																		InOpName = (WorkingFolder + "o" + t as string + ".bmp")
																		OutOpName = (WorkingFolder + "o_" + t as string + ".bmp")

																		-- set the filenames...
																		NewOp = bitmap NewBitMapWidth NewBitMapHeight
																		NewTex = bitmap NewBitMapWidth NewBitMapHeight
																		
																		try
																			(
																				copy OpacityMapArray[t] NewOp
																				copy BitMapArray[t] NewTex
																			) catch (
																				MessageBox "Something fucked up.\nGo get Kyle."
																			)
																		
																		NewOp.filename = InOpName
																		NewTex.filename = InTexName

																		-- Delete the files (if they're already there)
																		DeleteFile NewOp.filename
																		DeleteFile NewTex.filename

																		-- and save them...
																		save NewTex
																		save NewOp

																		-- if DebugMode.checked do format "Starting Alchemy...\n"
																		-- Process Maps via Alchemy
																		doscommand ("alchemy -w -o -b " + InOpName + " " + OutOpName)
																		doscommand ("alchemy -w -o " + InTexName + " " + OutTexName)

																		-- doscommand ("alchemy -w -o -b -X" + NewBitMapWidth as string + " -Y" + NewBitMapHeight as string + " " + InOpName + " " + OutOpName)
																		-- doscommand ("alchemy -w -o -X" + NewBitMapWidth as string + " -Y" + NewBitMapHeight as string + " " + InTexName + " " + OutTexName)
																		format "Alchemy Done!...\nProcessing Bitmaps Finished!\n"
		
																		-- Delete files that may already be there
																		deleteFile FinalOpName
																		deleteFile FinalTexName
		
																		-- Copy the processed bitmaps to their final name/location
																		copyFile OutOpName FinalOpName
																		copyFile OutTexName FinalTexName
																		-- ************************************************************
																		-- ** Create the new mesh *************************************
																		-- ************************************************************
																		if DebugMode.checked do
																			format "Building Geometry...\n"
		
																		BoxDimension = ((distance NewCamera.pos NewCamera.target.pos) * (tan(fov * 0.5))) * 2
																		PixelSize = (BoxDimension as float / BitMapSizeValue as float)
																		if debugMode.checked do format "PixelSize:%\n" PixelSize
		
																		-- calculate the width and height offsets for the new geometry
																		w1 = BitmapInfo[2].x
																		w2 = BitmapInfo[1].width - (w1 + BitmapInfo[3].x)
																		AvgFreeWidth = (BitmapInfo[1].width - BitmapInfo[3].x) * 0.5
																		if w2 >= w1 then
																			ObjectOffsetW = (AvgFreeWidth - w1) * PixelSize
																		else
																			ObjectOffsetW = -(AvgFreeWidth - w2) * PixelSize
		
																		h1 = BitmapInfo[2].y
																		h2 = BitmapInfo[1].height - (h1 + BitmapInfo[3].y)
																		AvgFreeHeight = (BitmapInfo[1].height - BitmapInfo[3].y) * 0.5
																		if h2 >= h1 then
																			ObjectOffsetV = (AvgFreeHeight - h1) * PixelSize
																		else
																			ObjectOffsetV = -(AvgFreeHeight - h2) * PixelSize

																		if DebugMode.checked do
																			(
																				format "w1:% w2:% h1:% h2:%\n" w1 w2 h1 h2
																				format "Height Offset:%\nWidth Offset:%\nAvg Free: % %\n" ObjectOffsetV ObjectOffsetW AvgFreeWidth AvgFreeHeight
																			)

																		NewHeight = BoxDimension as float * HeightScalar
																		NewWidth = BoxDimension as float * WidthScalar
																		b = box()
																		b.width = NewWidth
																		b.height = BoxDimension
																		b.length = NewHeight
																		b.pos = NewCamera.target.pos
																		b.dir = -NewCamera.dir
																		if debugMode.checked do
																			format "Camera Dir vector @ %: %\n" t -NewCamera.dir
																		b.mapcoords = true
																		convertToMesh b
																		for j = 1 to 4 do deleteVert b b.numverts
																		if NewCamera.dir.x < 0.0 do
																			in coordsys local rotate b 180 z_axis
																		in coordsys local rotate b 180 z_axis
																		in coordsys local move b [ObjectOffsetW, ObjectOffsetV, 0]
																		-- flip the UV's along U
																		if (getTvert b 4).x == 0.0 do
																		for j = 1 to b.numtverts do
																			(
																				CTvert = (getTvert b j)
																				if CTvert.x == 1.0 then
																					CTvert.x = 0.0
																				else
																					if CTvert.x == 0.0 do (CTvert.x = 1.0)
																				setTvert b j CTvert
																			)
																		update b
																		if DebugMode.checked do
																			format "Geometry done!\n"
		
																		-- ************************************************************
																		-- ** Generate a new material *********************************
																		-- ************************************************************
																		-- make sure all face IDs are set to 1
																		if DebugMode.checked do
																			format "Building new material...\n"
																		for j = 1 to b.numfaces do setFaceMatID b j 1
																		NMat = standardMaterial()
																		-- give it a random color
																		NMat.diffuse = random (color 20 20 20) (color 220 220 220)
																		NMat.diffuseMap = bitmaptexture()
																		NMat.OpacityMap = bitmaptexture()
																		-- specify the bitmap names
																		NMat.diffuseMap.filename = FinalTexName
																		NMat.OpacityMap.filename = FinalOpName
																		NMat.shinestrength = 0.0
																		NMat.name = substring OutTexName 1 (OutTexName.count - 3)
																		NMat.selfIllum = 100
																		b.mat = NMat
																		append DRObjectsArray b
																		if DebugMode.checked do
																			format "Finished building new material!\n"
																	)
															)
														ProgressEnd()
														-- Attach the detail reduced objects together
														for j = DRObjectsArray.count to 2 by -1 do
															attach DRObjectsArray[1] DRObjectsArray[j]
														DRObject = DRObjectsArray[1]
														DRObject.mat.name = (CObj.name + "dr")
		
														if i == 1 then
															DRL = DRLevel.value as integer
														else
															DRL = BranchDRLevel.value as integer
															
														DRObject.name = ("$" + (substring CObj.name 1 (CObj.name.count - 4)) + DRL as string + "-00")
														DRObject.pivot = CObj.pivot
														delete CLight
														CLight = undefined
														if debugMode.checked != true do
															(
																delete NewCamera
																NewCamera = undefined
																delete CameraPath
																CameraPath = undefined
															)
														CObj.mat = OriginalMat
													)
											) else (
												if renderSeperate.checked do
													MessageBox "No Objects Defined!"
											)
										gc() -- collect some garbage
									)
									-- Lonnie, comment the next two lines out "--"
									if SingleObject == true do
										delete CTrunk
		
								-- unhide everything at the end...
								for o in objects do unhide o
								CTrunk = CBranches = undefined
							) else (
								MessageBox "You must initialize the objects first"
							)
					)
				on DRRollout open do
					(
						MagicIndex = undefined
						offsetHeight = false
						HeightOffset = 0.0
					)
			)

		-- ******************************************************************************************************************************
		-- * Utility Open
		-- ******************************************************************************************************************************
		on DetailReduction open do
			(
				if (getfiles "C:\DR.txt").count == 0 then
					(
						if (WorkingFolder = (GetSavePath caption:"Select the location of a temporary folder to use for keeping texturemaps")) != undefined do
							(
								f = createfile "C:\DR.txt"
								openfile "DR.txt"
								WorkingFolder = (WorkingFolder + "\\")
								format ("Working Location: " + WorkingFolder) to:f
								close f
								format "Working Folder initialized to: %\n" WorkingFolder
							)
					) else (
						f = openfile "C:\DR.txt"
						CLine = readline f
						for i = 1 to CLine.count do
							(
								if cLine[i] == ":" do
									(
										WorkingFolder = (substring CLine (i + 2) (CLine.count - i - 1))
										if substring WorkingFolder WorkingFolder.count 1 != "\\" do
											WorkingFolder = WorkingFolder + "\\"
										format "Working Location initialized to: %\n" WorkingFolder
										Exit
									)
							)
						close f
					)

				Flt = NewRolloutFloater "Detail  Reduction" 400 550
				addrollout DRRollout Flt
				gc()
			)
		label DRLabel1 "Detail Reduction"
		label DRLabel2 "1/12/98"
		
	)