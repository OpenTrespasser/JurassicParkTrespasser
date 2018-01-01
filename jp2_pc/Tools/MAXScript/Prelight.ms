-- *******************************************************************************************
-- *
-- * Copyright © DreamWorks Interactive, 1997
-- *
-- * Contents:
-- *           Implementation of Prelight.ms
-- * Bugs:
-- *
-- * To do:
-- *
-- * Notes:
-- *		This script requires MAX2.0 to run.
-- *
-- *******************************************************************************************
fn StripInstance objName = 
	(
		DashPos = 0
		for i = 1 to objName.count do
			if objName[i] == "-" do
				DashPos = i
		if DashPos == 0 then	
			(
				return undefined
			) else (
				n = substring objName 1 (DashPos - 1)
				return n
			)
	)

fn TotalReset obj = 
	(
		local CObj = obj
		ConvertToMesh CObj
		CObj.pivot = CObj.center
		Props = GetUserPropBuffer CObj
		
		local VertArray = for i = 1 to CObj.numverts collect (getvert CObj i)
		local FaceArray = for i = 1 to CObj.numfaces collect (getface CObj i)
		local MatIDArray = for i = 1 to CObj.numfaces collect (GetFaceMatID CObj i)
		local TVertArray = #()
		
		local i = 0
		for i = 1 to FaceArray.count do
			(
				CFace = (GetTVFace CObj i)
				if FindItem TVertArray CFace.x == 0 do (append TVertArray CFace.x)
				if FindItem TVertArray CFace.y == 0 do (append TVertArray CFace.y)
				if FindItem TVertArray CFace.z == 0 do (append TVertArray CFace.z)
			)
		TVertUVArray = (for i = 1 to TVertArray.count collect (GetTvert CObj TVertArray[i]))
		local m = mesh vertices:VertArray faces:FaceArray MaterialIDs:MatIDArray
		for i = 1 to CObj.numfaces do
			(
				SetEdgeVis m i 1 (GetEdgeVis CObj i 1)
				SetEdgeVis m i 2 (GetEdgeVis CObj i 2)
				SetEdgeVis m i 3 (GetEdgeVis CObj i 3)
			)
		for i = 1 to CObj.numfaces do
			SetFaceSmoothGroup m i (getFaceSmoothGroup CObj i)
		addmodifier m (uvwMap maptype:1)
		ConvertToMesh m
		
		local Ntv = getNumTVerts CObj
		SetNumTverts m Ntv true
		
		for i = 1 to CObj.numfaces do
			setTVFace m i (GetTVFace CObj i)
		
		update m
		for i = 1 to Ntv do
			SetTvert m i (GetTvert CObj i)
		m.name = (CObj.name + "_regen")
		m.pivot = m.center
		m.wirecolor = CObj.wirecolor
		
		update m
		if CObj.mat != undefined do
			m.mat = CObj.mat
		
		local oname = CObj.name
		delete CObj
		m.name = oname
		setUserPropBuffer m Props
		gc()		
		return m;
	)

fn ClearBitmap Bmap c = 
	(
		CRow = for x = 1 to Bmap.width collect c
		for y = 0 to (Bmap.height - 1) do setPixels Bmap [0,y] CRow
		return Bmap
	)
	
fn GridBitmap Bmap = 
	(
		for y = 0 to (Bmap.height - 1) do
			if mod y 16 == 0 then
				(
					CRow = for x = 1 to Bmap.width collect (color 50 50 50)
					setPixels Bmap [0,y] CRow
				) else (
					CRow = #()
					for x = 1 to Bmap.width do
						if mod x 16 == 0 then
							append CRow (color 50 50 50)
						else 
							append CRow (color 0 0 0)
					setPixels Bmap [0,y] CRow
				)
		return Bmap
	)

fn round x = (x + 0.5) as integer

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

-- takes an array of 3xn bitmaps and strips out the center column from each,
-- stacking them from left to right into a new bitmap.
fn spliceMaps BMapArray = 
	(
		width = BMapArray.count
		height = BMapArray[1].height
		b = bitmap width height
		for i = 1 to height do
			(
				CRow = (for j = 1 to width collect (getpixels BMapArray[j] [1, (i - 1)] 1)[1])
				SetPixels b [0, (i - 1)] CRow
			)
		return b
	)

-- *****************
-- * Start Utility * 
-- *****************
Utility Prelight "PreLight"
(

local 	WorkingFolder, SCObject, SRObjects, SunDir, SCObjectMat,
				SRObjectMat, NewCamera, CameraPath, PreLightRollout
				
rollout PreLightRollout "Scene Prelighting"
	(
		local 	SourceObject, Sun, NewColor, CLight, Flt, NewCamera,
						MapSizes = #(),
						TextureMapArray = #(),
						BumpMapArray = #(),
						TrunkArray = #(),
						TrunkArrayNames = #(),
						TrunkMatArray = #(),
						SCObjectArray = #(),
						SRObjectArray = #(),
						DBitmap = (bitmap 128 64),
						SunArray = #([0.163287,-0.71436,0.680461], [0.150228,-0.623607,0.767167], [0.13906,-0.522188,0.841417], [0.129977,-0.411837,0.90194], [0.123134,-0.294445,0.947703], [0.118652,-0.17202,0.977922], [0.116608,-0.0466583,0.992081], [0.11704,0.0794943,0.989941], [0.11994,0.204279,0.971537], [0.125262,0.32556,0.937187], [0.132916,0.441262,0.88748])

		group "Game Area"
			(
				dropDownList GameArea items:#("Beach", "Jungle Road", "Platation House", "Industrial Jungle", "InGEN Town", "Plains", "PineValley", "Shore", "InGEN Lab", "Ascent", "Summit") selection:2
			)

		group "Initialize Settings"
			(
				label SCLabel "Shadow Casting Object:" align:#left
				pickbutton GetSCObject width:140 offset:[60,-20]
				Label srLabel "Shadow Recieving Objects:" align:#left
				button AddTrunk "Add Selection" align:#left width:120
				button RemoveTrunk "Remove Object" align:#left width:120
				listBox TrunksList height:3 width:140 offset:[138,-50]
				edittext FileStr "Map Substring:" text:"PrelitBitmap"
			)

		group "Rendering Options"
			(
				spinner NumViews "# Unique Rotations" range:[1,4,1] type:#integer fieldwidth:35 align:#left
				radiobuttons SuggestPowOfTwo "Power of two map sizes:" labels:#("Hi", "Lo", "Near") align:#right default:2 offset:[0,-27]
				colorpicker AmbColor "Ambient Light Color:" color:[140,140,140] offset:[-50,0]
				colorpicker LightColor "SpotLight Color:" color:[255,255,255] offset:[90,-25]
				spinner Mult "Light Multiplier" range:[.1,5,1.2] fieldwidth:40 align:#left
				spinner shadowmapsize "Shadow Map Size:" range:[64,1024,512] type:#integer fieldwidth:40 align:#right offset:[0,-20]
				spinner BitmapWidth "Output Width:" range:[8,256,64] type:#integer fieldwidth:40 align:#left
				spinner BitmapHeight "Output Height:" range:[8,256,64] type:#integer fieldwidth:40 align:#right offset:[0,-20]
				checkbox AttachObjects "Auto-Attach all trunks together" checked:true
				button MakePof2 "Make Sizes Power of 2" align:#right offset:[0,-20] height:18
			)

		group "Bitmap Darkening"
			(
				checkbox DarkenBitmap "Darken Trunk Base"
				spinner DarkenValue "Darkening %: " range:[5,95,30] fieldwidth:45 offset:[0,0] align:#left enabled:false
				spinner DarkenPos "Darkening Start %:" range:[1,100,80] fieldwidth:45 offset:[0,0] align:#left enabled:false
				bitmap DarkBitMapGraph width:128 height:64 align:#right offset:[-10,-64]
			)
		button renderMap "Prelight" width:315 height:40

		label status1
		label status2
		label status3


		-- **********************************************************************************************
		-- Make Power of two
		-- **********************************************************************************************
		on MakePof2 pressed do
			(
				BitmapWidth.value = NearPowOfTwo BitmapWidth.value SuggestPowOfTwo.state
				BitmapHeight.value = NearPowOfTwo BitmapHeight.value SuggestPowOfTwo.state
				MapSizes[TrunksList.selection] = [BitmapWidth.value, BitmapHeight.value]
				if MapSizes[TrunksList.selection].x > 256 do MapSizes[TrunksList.selection].x = 256
				if MapSizes[TrunksList.selection].y > 256 do MapSizes[TrunksList.selection].y = 256
				if MapSizes[TrunksList.selection].x < 8 do MapSizes[TrunksList.selection].x = 8
				if MapSizes[TrunksList.selection].y < 8 do MapSizes[TrunksList.selection].y = 8
			)

		-- **********************************************************************************************
		-- BitMap Height spinner changing state
		-- **********************************************************************************************
		on BitmapHeight changed state do
			(
				MapSizes[TrunksList.selection] = [BitmapWidth.value, BitmapHeight.value]
				if MapSizes[TrunksList.selection].x > 256 do MapSizes[TrunksList.selection].x = 256
				if MapSizes[TrunksList.selection].y > 256 do MapSizes[TrunksList.selection].y = 256
				if MapSizes[TrunksList.selection].x < 8 do MapSizes[TrunksList.selection].x = 8
				if MapSizes[TrunksList.selection].y < 8 do MapSizes[TrunksList.selection].y = 8
			)

		-- **********************************************************************************************
		-- BitMap Width spinner changing state
		-- **********************************************************************************************
		on BitmapWidth changed state do
			(
				MapSizes[TrunksList.selection] = [BitmapWidth.value, BitmapHeight.value]
				if MapSizes[TrunksList.selection].x > 256 do MapSizes[TrunksList.selection].x = 256
				if MapSizes[TrunksList.selection].y > 256 do MapSizes[TrunksList.selection].y = 256
				if MapSizes[TrunksList.selection].x < 8 do MapSizes[TrunksList.selection].x = 8
				if MapSizes[TrunksList.selection].y < 8 do MapSizes[TrunksList.selection].y = 8
			)

		-- **********************************************************************************************
		-- Trunks list changing state
		-- **********************************************************************************************
		on TrunksList selected s do
			(
				MAX select none
				selectmore TrunkArray[s]
				BitmapWidth.value = MapSizes[s].x
				BitmapHeight.value = MapSizes[s].y
				CObj = TrunkArray[TrunkArray.count]
				SRObjectScale = (CObj.max - CObj.min)
				MAXDiameter = SRObjectScale.x
				if SRObjectScale.y > MAXDiameter do MAXDiameter = SRObjectScale.y
				GCircumference = MAXDiameter * pi
				GHeight = SRObjectScale.z
				MHeight = BitmapHeight.value
				BitmapWidth.value = MapSizes[s].x
				BitmapHeight.value = MapSizes[s].y
				format "s:% %\n" s MapSizes
			)

		-- **********************************************************************************************
		-- Remove a trunk from the list
		-- **********************************************************************************************
		on RemoveTrunk pressed do
			(
				if (TrunkArray.count > 0) do
					(
						s = TrunksList.selection
						deleteitem TrunkArray s
						deleteitem TrunkArrayNames s
						deleteItem MapSizes s
						deleteItem TrunkMatArray s
						TrunksList.items = TrunkArrayNames
					)
			)

		-- **********************************************************************************************
		-- Add a trunk to the list
		-- **********************************************************************************************
		on AddTrunk pressed do
			(
				ObjArray = for obj in selection collect obj
				for obj in ObjArray do
					(
						if findItem TrunkArray obj == 0 do
							(
								append TrunkArray obj
								append TrunkArrayNames obj.name
								append TrunkMatArray obj.mat
								obj.mat = copy obj.mat
								TrunksList.items = TrunkArrayNames
								CObj = TrunkArray[TrunkArray.count]
								SRObjectScale = (CObj.max - CObj.min)
								MAXDiameter = SRObjectScale.x
								if SRObjectScale.y > MAXDiameter do MAXDiameter = SRObjectScale.y
								GCircumference = MAXDiameter * pi
								GHeight = SRObjectScale.z
								MHeight = BitmapHeight.value
								BitmapWidth.value = NearPowOfTwo (MHeight * (GCircumference / GHeight)) SuggestPowOfTwo.state
								MapSizes[TrunkArray.count] = [BitmapWidth.value, BitmapHeight.value]
							)
					)
			)

	-- **********************************************************************************************
	-- *                                                                                            *
	-- *                                                                                            *
	-- *                              << Render Maps >>                                             *
	-- *                                                                                            *
	-- *                                                                                            *
	-- **********************************************************************************************
	on RenderMap pressed do
		(
		
			for i = 1 to TrunkArrayNames.count do
				for o in objects do
					if o.name == TrunkArrayNames[i] do
						TrunkArray[i] = o

			for o in objects do
				if o.name == GetSCObject.text do
					SCObject = o
					
			-- Determine the game Area for Object Naming purposes
			AreaKey = "XX"
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
			freeSceneBitmaps()
			BitMapCounter = 0
			-- setup the array's for multiple tree rotations
			SCObjectArray = #()
			SRObjectArray = #()
			ObjectsMatArray = #()
			RotationOffset = (360.0 / NumViews.value as float)
			format "Rotation Offset:%\n" RotationOffset
			CommonCenter = SCObject.center
			for i = 1 to NumViews.value do
				(
					CWireColor = random (color 50 50 50) (color 200 200 200)
					append SCObjectArray (copy SCObject)
					-- strip the instance tag off of the name, if it's there
					if (CName = StripInstance SCObjectArray[i].name) != undefined do
						SCObjectArray[i].name = CName
					SCObjectArray[i].name = (SCObjectArray[i].name + "~" + AreaKey + (i as integer) as string + "0-00")
					SCObjectArray[i].mat = copy SCObjectArray[i].mat
					SCObjectArray[i].wirecolor = CWireColor
					append SRObjectArray #()
					for j = 1 to TrunkArray.count do
						(
							append SRObjectArray[i] (copy TrunkArray[j])

							-- define the area name
							if (CName = StripInstance SRObjectArray[i][j].name) != undefined do
								SRObjectArray[i][j].name = CName
							SRObjectArray[i][j].name = (SRObjectArray[i][j].name + "~" + AreaKey + (i as integer) as string + "0-00")
							SRObjectArray[i][j].wirecolor = CWireColor
							SRObjectArray[i][j].mat = copy SRObjectArray[i][j].mat
							-- Append the materials
							append ObjectsMatArray TrunkMatArray[j]
						)
					max select none
					selectmore SCObjectArray[i]
					for j = 1 to SRObjectArray[i].count do selectmore SRObjectArray[i][j]
					about CommonCenter rotate selection (RotationOffset * (i - 1)) z_axis
					in coordsys world move selection [(i * 50),0,0]

					-- Do Total Resets on the objects
					SCObjectArray[i] = TotalReset SCObjectArray[i]
					for j = 1 to SRObjectArray[i].count do SRObjectArray[i][j] = TotalReset SRObjectArray[i][j]
				)

			-- *******************************************
			-- * Start Looping through the different Views
			-- *******************************************
			for z = 1 to NumViews.value do
				(
					gc()
					Status1.text = ("Rendering Object View " + z as string + " of " + NumViews.value as string)
					SCObject = SCObjectArray[z]
					
					DarkenObj = undefined
					if darkenBitMap.checked do
						(
							DarkenObj = SRObjectArray[z][1]
							for i = 2 to TrunksList.items.count do
								if SRObjectArray[z][i].center.z < DarkenObj.center.z do
									DarkenObj = SRObjectArray[z][i]
						)
					fov = 0.00100
					max wire facet
					SCObjectMat = copy SCObject.mat
		
					if classof SCObject.mat == StandardMaterial do
						(
							SCObject.mat.DiffuseMapEnable = false
							SCObject.mat.BumpMapEnable = false
							SCObject.mat.diffuse = [255,255,255] as color
							SCObject.mat.ambient = [255,255,255] as color
							SCObject.mat.shinestrength = 0.0
							SCObject.mat.selfIllum = 100
						)
					if classof SCObject.mat == MultiMaterial do
						(
							ns  = SCObject.mat.numsubs
							for i = 1 to ns do
								(
									SCObject.mat[i].DiffuseMapEnable = false
									SCObject.mat[i].BumpMapEnable = false
									SCObject.mat[i].diffuse = [255,255,255] as color
									SCObject.mat[i].ambient = [255,255,255] as color
									SCObject.mat[i].shinestrength = 0.0
									SCObject.mat[i].selfIllum = 100
								)
						)
		
					NumTrunks = TrunksList.items.count
					TempObj = copy SRObjectArray[z][1]
					for j = 2 to TrunkArray.count do attach TempObj (copy SRObjectArray[z][j])
					TempObj.pivot = TempObj.center
					TempObj = totalReset TempObj
					CLight = freespot()
					CLight.dir = SunArray[GameArea.selection]
					CLight.color = LightColor.color
					CLight.multiplier = Mult.value
					CLight.HotSpot = 60
					SunDir = CLight.dir
					theta = 46.826
					d = distance TempObj.max TempObj.min
					x = (d / (2 * sin(theta / 2))) * 2.0
					CLight.pos = (TempObj.center + SunDir * x)
					CLight.projector = off -- turn off the projector light
					-- Set the background color to black
					CBackgroundColor = backgroundColor
					backgroundColor = [0,0,0] as color
					-- set the Ambient Light Level
					CAmbient = ambientColor
					ambientColor = AmbColor.color
					-- Turn off the Environment map, if it's on
					em = false
					if useEnvironmentMap == true do 
						(
							useEnvironmentMap = false
							em = true
						)
					Status3.text = ("Rendering Shadow Map...")
					for o in objects do hide o
					unhide SCObject
					ProgressStart "Rendering Shadow Map..."
					ShadowMap = (render camera:CLight outputwidth:shadowmapsize.value outputheight:shadowmapsize.value vfb:off)
					Width = ShadowMap.width
					Height = shadowmap.height
					for y = 0 to (Height - 1) do
						(
							ProgressValue = ((y as float / Height as float) * 100)
							if (ProgressUpdate ProgressValue) == false then
								(
									ExitCode = 1
									Exit
								)
							CRow = getpixels Shadowmap [0,y] Width
							for i = 1 to CRow.count do
								(
									CPixel = CRow[i]
									Lum = CPixel.v
									if Lum > 255 do Lum = 255
									Lum = 255 - Lum
									CPixel = [Lum, Lum, Lum]
									CRow[i] = CPixel
								)
							setpixels Shadowmap [0,y] CRow
						)
					ShadowMap.filename = (WorkingFolder + TempObj.name + ".bmp")
					save ShadowMap
					ProgressEnd()
					CLight.projector = on
					CLight.ProjectorMap = bitmaptexture()
					CLight.projectormap.filename = ShadowMap.filename
					delete TempObj
					TempObj = undefined
					-- ******************
					-- * rendering loop *
					-- ******************
					for s = 1 to NumTrunks do
						(
							BitMapCounter += 1
							Status2.text = ("Rendering Trunk Object " + s as string + " of " + NumTrunks as string)
							SRObject = SRObjectArray[z][s]
							CameraPath = circle()
							CameraPath.pos = SRObject.center
							rotate CameraPath 90 Z_Axis
							theta = fov
							d = (SRObject.max.z - SRObject.min.z)
							h = (d / (2 * sin(theta / 2)))
							x = h * (cos(theta))
							NewCamera = targetCamera()
							NewCamera.fov = fov
							NewCamera.target = targetObject()
							SourceCenter = SRObject.center
							NewCamera.target.pos = SourceCenter
							NewDir = normalize (NewCamera.pos - NewCamera.target.pos)
						 	NewCamera.pos = NewCamera.target.pos + NewDir * x
							NewCamera.pos.controller = path()
							NewCamera.pos.controller.path = CameraPath
							DistanceOffset = (SRObject.max - SRObject.min)
							DistanceOffset2 = (DistanceOffset.x + DistanceOffset.y) / 4.0
							CameraPath.radius = (x * (3.0 / BitmapHeight.value)) + DistanceOffset2
							ExitCode = 0

							-- setup Camera Animation
							deletekeys NewCamera.pos.controller #allkeys	-- delete all the keys from the camera's path controller			
							c = NewCamera.pos.controller
							addNewKey c 0
							addNewKey c MapSizes[s].x
							t = MapSizes[s].x
		
							animate on -- start setting the keyframes
								(
									at time 0 c.percent = 0.0
									at time t c.percent = 100.0
								)
							for o in objects do hide o 
		
							-- hide SRObject
							if ExitCode != 1 do
								(
									unhide SRObject
									hide SCObject
									str = (substring SRObject.name 1 4)
									ProgressStart "Rendering New Texture Map..."
									format "Rendering Texture Map @:% %\n" MapSizes[s].x MapSizes[s].y
									Status3.text = ("Rendering Texture Map...")
									for t = 0 to (MapSizes[s].x - 1) by 1 do
									(
										ProgressValue = ((t as float / MapSizes[s].x) * 100)
										if (ProgressUpdate (ProgressValue)) == false then
											(
												ExitCode = 1
												Exit
											)
										NewSlice = (render camera:NewCamera outputwidth:3 outputHeight:MapSizes[s].y vfb:off frame:t)
										Append TextureMapArray NewSlice
									)
									ProgressEnd()
		
									NewTextureMap = (spliceMaps TextureMapArray)
									format "NewTextureMapSize = % %\n" NewTextureMap.width NewTextureMap.height
									-- *****************
									-- * Darken Bitmap *
									-- *****************
									if (DarkenObj == SRObject) and DarkenBitmap.checked do
										(
											Status3.text = ("Darkening Texture Map...")
											format "Darkening base tree bitmap...\n"
											DPixelSize = (DarkenObj.max.z - DarkenObj.min.z) / (NewTextureMap.height as float)
											NumDarkPixels = ((DarkenPos.value * 0.01) * (DarkenObj.max.z - DarkenObj.min.z))  / (DPixelSize as float)
											if NumDarkPixels > NewTextureMap.height do NumDarkPixels = NewTextureMap.height
											YStartPos = round (NewTextureMap.height - NumDarkPixels)
											DarkenIncrement = (DarkenValue.value / NumDarkPixels as float)
											NewDarkenIncrement = 0
											Dcount = 0
											for y = YStartPos to (NewTextureMap.height - 1) do
												(
													Dcount += 1
													NewDarkenIncrement = (100 - (Dcount * DarkenIncrement)) * .01
													CRow = getPixels NewTextureMap [0,y] NewTextureMap.width
													NRow = #()
													for a = 1 to CRow.count do
														(
															CColor = CRow[a]
															append NRow (color (CColor.r * NewDarkenIncrement) (CColor.g * NewDarkenIncrement) (CColor.b * NewDarkenIncrement))
														)
													setPixels NewTextureMap [0,y] NRow
												)
											format "NewTextureMapSize After Darkening = % %\n" NewTextureMap.width NewTextureMap.height
										)
		
									NewTextureMap.filename = (WorkingFolder + FileStr.text + BitMapCounter as string + "t2.bmp")
									save NewtextureMap
									format "NewTextureMapSize After saving = % %\n" NewTextureMap.width NewTextureMap.height
		
									if ExitCode != 1 do
										(
											-- do we need to render a bump map for this guy too?
											DoBump = false
											if classof SRObject.mat == standardmaterial do (if classof SRObject.mat.bumpmap == bitmaptexture do DoBump = true)
		
											if classof SRObject.mat == MultiMaterial do 
												(
													ns = SRObject.mat.numsubs
													for i = 1 to ns do (if classof SRObject.mat[i].bumpmap == bitmaptexture do DoBump = true)
												)
		
											BFileName = undefined
											if DoBump == true then
												(
													format "DoBump = TRUE!!!!\n"
													BmpObj = copy SRObject
													hide SRObject
													-- start rendering a bump map
													-- build a new material that has the bump map in the diffuse map slot
													SRObjectMat = BmpObj.mat  --first, grab the original material into SRObjectMat
													-- if multi material
													if classof BmpObj.mat == MultiMaterial do
														(
															ns = BmpObj.mat.numsubs
															for i = 1 to ns do
																(
																	if classof BmpObj.mat[i].bumpmap == bitmaptexture do
																		(
																			BmpObj.mat[i].diffusemap = BmpObj.mat[i].bumpmap
																			BmpObj.mat[i].selfillum = 100.00
																		)
																)
														)
													-- If standard material
													if classof BmpObj.mat == StandardMaterial do
														(
															if classof BmpObj.mat.bumpmap == bitmaptexture do
																(
																	BmpObj.mat.diffusemap = BmpObj.mat.bumpmap
																	BmpObj.mat.selfillum = 100.00
																)
														)
													-- setup lighting to render the bump map
													for l in lights do (l.on = false)
													Status3.text = ("Rendering Bump Map...")

		--										ProgressStart "Rendering New BumpMap..."
													format "Rendering Bump Map @:% %\n" BitmapWidth.value Bitmapheight.value
													for t = 0 to (MapSizes[s].x - 1) do
														(
		--												ProgressValue = ((t as float / MapSizes[s].x as float) * 100)
		--												if (ProgressUpdate ProgressValue) == false then
		--													(
		--														ExitCode == 1
		--														Exit
		--													)
											 				NewSlice = (render camera:NewCamera outputwidth:3 outputHeight:MapSizes[s].y vfb:off frame:t)
															Append BumpMapArray NewSlice
														)
		--										ProgressEnd()
													for l in lights do l.on = true
													delete BmpObj
													BmpObj = undefined
													unhide SCObject
													unhide SRObject
													NewBumpMap = (spliceMaps BumpMapArray)
													BFilename = (WorkingFolder + FileStr.text + BitMapCounter as string + "b8.bmp")
													NewBumpMap.filename = (WorkingFolder + "BTemp" + BitMapCounter as string + ".bmp")
													NewBumpMapTempName = (WorkingFolder + "BTemp_" + BitMapCounter as string + ".bmp")
													save NewBumpMap
													doscommand ("alchemy -w -o -b -X" + (MapSizes[s].x as integer) as string + " -Y" + (MapSizes[s].y as integer) as string + " " + NewBumpMap.filename + " " + NewBumpMapTempName)
													deleteFile BFilename
													copyfile NewBumpMapTempName BFilename
												) else (
													NewBumpMap = undefined
													format "Bumpmap is NOT being rendered\n" 
												)
											if ExitCode != 1 do
												(
													-- delete all the camera stuff
													delete NewCamera
													NewCamera = undefined
													delete CameraPath
													CameraPath = undefined
												) -- Third ExitCode
										) -- Second ExitCode
								) -- First ExitCode
							-- Display the texture maps
							display NewTextureMap
							if NewBumpMap != undefined do display NewBumpMap
							TextureMapArray = #()
							BumpMapArray = #()
		
							NMat = standardMaterial()
							NMat.diffusemap = bitmapTexture()
							NMat.diffusemap.filename = NewTextureMap.filename
							if BFileName != undefined do
								(
									NMat.bumpmap = bitmaptexture()
									NMat.bumpmap.filename = BFilename
								)
							convertToMesh SRObject
							addmodifier SRObject (UVWMap())					
							SRObject.modifiers[1].maptype = 1					
							convertToMesh SRObject
							for i = 1 to SRObject.numfaces do SetFaceMatID SRObject i 1
							update SRObject
							SRObject.mat = NMat
							unhide SCObject
							gc()
						)
					-- **********************
					-- * End Rendering Loop *
					-- **********************
					delete CLight
					CLight = undefined
					-- set the material definition of the Shadow casting object back to it's original
					SCObject.mat = SCObjectMat
		
					for f in (getfiles Shadowmap.filename) do deletefile f
				)
			for o in objects do unhide o
			freeSceneBitmaps()
			status1.text = "Complete"
			status2.text = ""
			status3.text = ""

			if AttachObjects.checked do
				for i = 1 to NumViews.value do
					(
						for j = 2 to TrunkArray.count do attach SRObjectArray[i][1] SRObjectArray[i][j]
						SRObjectArray[i][1].pivot = SRObjectArray[i][1].center
						SRObjectArray[i][1] = totalReset SRObjectArray[i][1]
					)
	
			Messagebox "Done!"

		)
		
		-- **********************************************************************************************
		-- Get Shadow Casting Object
		-- **********************************************************************************************
		on GetSCObject picked obj do
			(
				SCObject = obj
				GetSCObject.text = SCObject.name
			)
		
		-- *************************************************************************************************
		-- * Interface changes
		-- *************************************************************************************************
		on Mult changed state do
			if CLight != undefined do
				CLight.multiplier = Mult.value
		
		on LightColor changed state do 
			if CLight != undefined do
				CLight.color = LightColor.color


		-- **********************************************************************************************
		-- Darken Bitmap Interface stuff
		-- **********************************************************************************************
		on DarkenBitmap changed state do
			(
				if DarkenBitmap.checked then
					(
						DarkenValue.enabled = DarkenPos.enabled = true
						DBitmap = ClearBitmap DBitmap (color 128 128 128)
						DarkBitMapGraph.bitmap = DBitmap
					) else (
						DarkenValue.enabled = DarkenPos.enabled = false
					)
			)

		-- *****************************
		-- * DarkenPos changed state * 
		-- *****************************
		on DarkenPos changed state do
			(
				nc = 255 * ((100 - DarkenValue.value) * 0.01)
				nc = (color nc nc nc)
				lc = (color 255 0 0)
				BitMapArray = #()
				-- #clear = a clear pixel
				-- #line = line color pixel

				BitMapArray = for x = 1 to DBitmap.width collect (for y = 1 to DBitmap.height collect #clear)
				-- BitMapArray is [x][y]

				XPoint = round(((100 - DarkenPos.value) * 0.01) * DBitmap.width)
				StartPoint = [XPoint, 1]
				EndPoint = [128, round((DarkenValue.value * 0.01) * DBitmap.height)]
				for x = 1 to BitMapArray.count do
					(
						if x < Xpoint then
							(
								BitMapArray[x][1] = #line
							) else (
								m = (EndPoint.y / (EndPoint.x - StartPoint.x))
								b = (1 - (m * Xpoint))
								y = (round ((m * x) + b))
								if y > 0 and y <= DBitmap.height do
									BitMapArray[x][y] = #line
							)
					)
				-- write the array into the bitmap
				for y = 1 to BitMapArray[1].count do
					(
						CRow = #()
						for x = 1 to BitMapArray.count do
							(
								if BitMapArray[x][y] == #clear do
									append CRow nc
								if BitMapArray[x][y] == #line do
									append CRow lc
							)
						setPixels DBitmap [0,(y - 1)] CRow
					)
				DarkBitMapGraph.bitmap = DBitmap
			)

		-- *****************************
		-- * DarkenValue changed state *
		-- *****************************
		on DarkenValue changed state do
			(
				nc = 255 * ((100 - DarkenValue.value) * 0.01)
				nc = (color nc nc nc)
				lc = (color 255 0 0)
				BitMapArray = #()
				-- #clear = a clear pixel
				-- #line = line color pixel

				BitMapArray = for x = 1 to DBitmap.width collect (for y = 1 to DBitmap.height collect #clear)
				-- BitMapArray is [x][y]

				XPoint = round(((100 - DarkenPos.value) * 0.01) * DBitmap.width)
				StartPoint = [XPoint, 1]
				EndPoint = [128, round((DarkenValue.value * 0.01) * DBitmap.height)]
				for x = 1 to BitMapArray.count do
					(
						if x < Xpoint then
							(
								BitMapArray[x][1] = #line
							) else (
								m = (EndPoint.y / (EndPoint.x - StartPoint.x))
								b = (1 - (m * Xpoint))
								y = (round ((m * x) + b))
								if y > 0 and y <= DBitmap.height do
									BitMapArray[x][y] = #line
							)
					)
				-- write the array into the bitmap
				for y = 1 to BitMapArray[1].count do
					(
						CRow = #()
						for x = 1 to BitMapArray.count do
							(
								if BitMapArray[x][y] == #clear do
									append CRow nc
								if BitMapArray[x][y] == #line do
									append CRow lc
							)
						setPixels DBitmap [0,(y - 1)] CRow
					)
				DarkBitMapGraph.bitmap = DBitmap
			)
)

-- ****************
-- * Utility Open *
-- ****************
on PreLight open do
	(
		if (getfiles "C:\PreLight.txt").count == 0 then
			(
				if (WorkingFolder = (GetSavePath caption:"Select the location of a temp folder")) != undefined do
					(
						f = createfile "C:\PreLight.txt"
						openfile "PreLight.txt"
						WorkingFolder = (WorkingFolder + "\\")
						format ("Working Location: " + WorkingFolder) to:f
						close f
						format "Working Folder initialized to: %\n" WorkingFolder
					)
			) else (
				f = openfile "C:\PreLight.txt"
				CLine = readline f
				for i = 1 to CLine.count do
					(
						if cLine[i] == ":" do
							(
								WorkingFolder = (substring CLine (i + 2) (CLine.count - i - 1))
								format "Working Location initialized to: %\n" WorkingFolder
								Exit
							)
					)
				close f
			)
		-- purge all the global variables
		SCObject = SRObject = SunDir = SCObjectMat = SRObjectMat = NewCamera = CameraPath = undefined
		Flt = NewRolloutFloater "Scene Pre-Lighting" 350 610
		addrollout PreLightRollout Flt
	)

-- Interface
label prelightLabel "Scene Prelighting"
label prelightLabel2 "1/12/98"
	) -- end Utility
