-- ObjectProps Rollouts
-- ****************************************************************************************************
-- Settings Rollout
-- ****************************************************************************************************
rollout SettingsRollout "Object Settings"
	(
		group "Alpha Colors"
			(
				dropDownList AlphaColorList items:#("Alpha Channel 0", "Alpha Channel 1", "Alpha Channel 2", "Alpha Channel 3", "Alpha Channel 4", "Alpha Channel 5", "Alpha Channel 6", "Alpha Channel 7", "Alpha Channel 8", "Alpha Channel 9", "Alpha Channel 10","Alpha Channel 11","Alpha Channel 12","Alpha Channel 13","Alpha Channel 14","Alpha Channel 15")
				pickbutton DeriveAlpha "Derive settings from object"
				colorpicker AlphaColor "Color:"color:[222,230,255]
				spinner AlphaLevel "Alpha:" range:[0,1,0.25] type:#float fieldwidth:35
			)
		
		button close "Close"

		on DeriveAlpha picked obj do
			(
				CObj = obj
				if classOf CObj.mat == standardMaterial then
					(
						CMat = cObj.mat
						AlphaColor.color = CMat.Diffuse
						AlphaLevel.value = (CMat.Opacity as float / 100.0)
					) else (
						MessageBox "Object Must have a standard material assigned to it!"
					)
			)
		
		on close pressed do
			removeRollout SettingsRollout
	)

-- ***********************
-- Terrain Texture Rollout
-- ***********************
rollout TerrainTextureRollout "Terrain Texture"
	(
		DropDownList SoundPropertiesList "Sound Materials :"
		spinner TextureLevel "Texture Level: " range:[1,20,1] type:#integer fieldwidth:45 enabled:true
		button close "Close"

		on TerrainTextureRollout open do
			if (TerrainMaterialTypesArray = (LoadSoundTypes())) != undefined do (SoundPropertiesList.items = TerrainMaterialTypesArray)
		
		on close pressed do
			removeRollout TerrainTextureRollout
	)

-- ***********************
-- Rendering Properties Rollout
-- ***********************

rollout RenderingRollout "Rendering Properties"
	(
		-- these are the material type default values
		-- (cos (0.3 * 180 / pi))
		local DefaultMat = #(1.0, 0.0, 1.0, false, false, 1.0, 0),
					ShinyMat = #(1, 1.9, 0.980067, false, false, 1, 0),  -- 0.2
					MetalMat = #(0.2, 1.10, 0.955337, false, false, 1, 0), -- 0.3
					WaterMat = #(1, 1.8, 1.0, true, true, 1.33, 0),
					MirrorMat = #(0, 2, 1.0, true, false, 1, 0),
					PrelitMat = #(0, 0, 1.0, false, false, 1, 1.0),
					DinoSkinMat = #(1, 1.3, 0.98, false, false, 1, 1.0),
					CurrentDDLObjectArray

		local RenderDefaults = 	#(#(1,1,true,0.025,true,true,false,0.0,false,24,0,0,0,0,1,1,0,false,false))

		checkbox IgnorePhysics "Ignore Physics Props"

		group "Rendering Defaults"
			(
				DropDownList DefaultsList items:#("Initial Interface Settings")
				button GenerateDefaults "Generate Defaults" width:140
			)

		group "Surface Type"
			(
				DropDownList SurfaceFlags items:#("Default", "Shiny", "Metal", "Water", "Mirror", "PreLit", "Dino Skin")
			)

		group "Surface Parameters"
			(
				DropDownList SurfaceCurvatureFlags "Surface Curvature:" items:#("MAX Normals","Flat Shaded", "Average Normals (curved)", "Infer Normals") selection:4
				checkbox SurfaceCurved "Curved"
				checkbox SurfaceBumpMaps "Use Bump Maps" checked:true
				spinner SurfaceBumpiness "Bump Strength:" range:[0,10,0.025] fieldwidth:45 align:#left
				checkbox SurfaceMipMap "MipMap" checked:true
				checkbox SurfaceCacheIntersecting "Cache Intersect"
				checkbox SurfaceVisible "Visible" checked:true
				checkbox SurfaceCastsShadows "Casts Shadows" checked:true
				checkbox SurfacePlanted "Planted"
				spinner SurfacePlantedHeight "Height:" enabled:false fieldwidth:45
				spinner CullDistance "Object Culling:" range:[0,2000,0] type:#float fieldwidth:45
				spinner ShadowCullDistance "Shadow Culling:" range:[0,2000,0] type:#float fieldwidth:45
				checkbox SurfaceAlwaysFace "Always Face"
			)
		
		group "Mesh Loading"
			(
				checkbox Wrap "Wrap" checked:true
				checkbox Merge "Merge" checked:true offset:[48,-20]
				checkbox Split "Split" offset:[100,-20]
			)

		group "Animation"
			(
				checkbox UseAnimatedTextures "Use Animated Textures" checked:false
				button GetIFL "Load IFL File" width:140 enabled:false
				spinner fps "fps:" range:[0.1,30,24] type:#float enabled:false
				label animblank0 "UV Coordinate Animation:"
				spinner DeltaX "Delta U (per sec): " range:[0,50,0] fieldwidth:45
				spinner DeltaY "Delta V (per sec): " range:[0,50,0] fieldwidth:45
			)

		group "Advanced Surface Params"
			(
				checkButton EnableAdvancedParams "Enable" align:#left
				spinner MaterialDiffuse "Diffuse:" range:[0,2,1.0] type:#float fieldwidth:35 enabled:false offset:[0,-23]
				spinner MaterialSpecular "Specular:" range:[0,2,0]  type:#float fieldwidth:35 enabled:false
				spinner MaterialSpecularAngle "Specular Angle:" range:[0,1,1]  type:#float fieldwidth:35 enabled:false
				spinner MaterialRefractIndex "Refract Index:" range:[1,100,1] type:#float fieldwidth:35 enabled:false
				spinner MaterialEmissive "Emissive:" range:[0,1,0] type:#float fieldwidth:35 enabled:false
				checkbox MaterialReflect "Reflect" enabled:false align:#left
				checkbox MaterialRefract "Refract" enabled:false align:#right offset:[0,-20]
			)

		group "Detail Reduction"
			(
				spinner DetailSwitchPoint "Switch at: " range:[1,600,120] type:#integer align:#left fieldwidth:45
				RadioButtons DetailAxis "Axis" labels:#("X","Y","Z") columns:3 align:#left default:3
				DropDownList DetailReductionObjectList "Object List:" items:#()
				button AddDetailReductionObject "Add" width:70 offset:[-37,0]
				button RemoveDetailReductionObject "Remove" width:70 offset:[37,-26]
			)
			
		button closeRollout "Close"

		on DefaultsList selected s do
			(
				SurfaceFlags.selection = RenderDefaults[s][1]
				SurfaceCurvatureFlags.selection = RenderDefaults[s][2]
				SurfaceBumpMaps.checked = RenderDefaults[s][3]
				SurfaceBumpiness.value = RenderDefaults[s][4]
				SurfaceVisible.checked = RenderDefaults[s][5]
				SurfaceCastsShadows.checked = RenderDefaults[s][6]
				SurfacePlanted.checked = RenderDefaults[s][7]
				SurfacePlantedHeight.value = RenderDefaults[s][8]
				UseAnimatedTextures.checked = RenderDefaults[s][9]
				fps.value = RenderDefaults[s][10]
				DeltaX.value = RenderDefaults[s][11]
				DeltaY.value = RenderDefaults[s][12]
				MaterialDiffuse.value = RenderDefaults[s][13]
				MaterialSpecular.value = RenderDefaults[s][14]
				MaterialSpecularAngle.value = RenderDefaults[s][15]
				MaterialRefractIndex.value = RenderDefaults[s][16]
				MaterialEmissive.value = RenderDefaults[s][17]
				MaterialReflect.checked = RenderDefaults[s][18]
				MaterialRefract.checked = RenderDefaults[s][19]
			)

		on GenerateDefaults pressed do
			(
				CDefaults = #()
				append CDefaults SurfaceFlags.selection
				append CDefaults SurfaceCurvatureFlags.selection
				append CDefaults SurfaceBumpMaps.checked
				append CDefaults SurfaceBumpiness.value
				append CDefaults SurfaceVisible.checked
				append CDefaults SurfaceCastsShadows.checked
				append CDefaults SurfacePlanted.checked
				append CDefaults SurfacePlantedHeight.value
				append CDefaults UseAnimatedTextures.checked
				append CDefaults fps.value
				append CDefaults DeltaX.value
				append CDefaults DeltaY.value
				append CDefaults MaterialDiffuse.value
				append CDefaults MaterialSpecular.value
				append CDefaults MaterialSpecularAngle.value
				append CDefaults MaterialRefractIndex.value
				append CDefaults MaterialEmissive.value
				append CDefaults MaterialReflect.checked
				append CDefaults MaterialRefract.checked
				format "\n\n***********************************************************************************************\n"
				format "***********************************************************************************************\n"
				format "Rendering Defaults:\n"
				format "%\n" CDefaults
				format "***********************************************************************************************\n"
				format "***********************************************************************************************\n\n\n"
			)

		on DetailReductionObjectList selected s do
			(
				if DRData[s] != undefined do
					DetailSwitchPoint.value = DRData[s]
			)

		on DetailSwitchPoint changed state do
			(
				s = DetailReductionObjectList.selection
					if CurrentDDLObjectArray[s] != undefined and CurrentDDLObjectArray[s] != "" do
						DRData[s] = DetailSwitchPoint.value as integer
			)

		on AddDetailReductionObject pressed do
			(
				CurrentDDLObjectArray = DetailReductionObjectList.items
				ObjArray = for obj in selection collect obj
				for i = 1 to ObjArray.count do
					(
						if i >= 1 and i <= 4 do
							(
								MagicIndex = -1
								CObj = ObjArray[i]
								for i = 1 to CObj.name.count do if CObj.name[i] == "~" do MagicIndex = i
								if MagicIndex != -1 then
									(
										DRValue = (substring CObj.name (MagicIndex + 4) 1) as integer
										if (DRValue >= 1) and (DRValue <= 4) do
											CurrentDDLObjectArray[DRValue] = CObj.name
									)
								else
									MessageBox ("Unable to resolve\nDetail Reduction Level\nfrom object \"" + CObj.name + "\".")
							)
					)
				for i = 1 to CurrentDDLObjectArray.count do
					if CurrentDDLObjectArray[i] == undefined do CurrentDDLObjectArray[i] = ""
				DetailReductionObjectList.items = CurrentDDLObjectArray
			)
	
		on RemoveDetailReductionObject pressed do
			(
				CurrentSelection = DetailReductionObjectList.selection
				if CurrentDDLObjectArray.count >= 1 do
					(
						CurrentDDLObjectArray = DetailReductionObjectList.items
						deleteitem CurrentDDLObjectArray CurrentSelection
						DetailReductionObjectList.items = CurrentDDLObjectArray
						DetailReductionObjectList.selection = 1
					)
			)

		on SurfacePlanted changed state do
			if SurfacePlanted.checked then
				SurfacePlantedHeight.enabled = true
			else
				SurfacePlantedHeight.enabled = false

		on closeRollout pressed do
			removeRollout RenderingRollout

		on GetIFL pressed do
			(
				if (IFLFile = getopenFileName caption:"Choose Image File List") != undefined do
					(
						IFL = #()
						f = openFile IFLFile
						while not eof f do
							(
								CLine = readline f
								if CLine[CLine.count] == "\n" do (CLine = (substring CLine 1 (CLine.count - 1)))
								if CLine.count > 4 do
									if (substring CLine (CLine.count - 3) 1) == "." do
										append IFL CLine
							)
						close f
						if (IFL.count <= 32) and (IFL.count > 0) then
							(
								GetIFL.text = (FilenameFromPath IFLFile)
								messageBox (IFL.count as string + " frames loaded.")
							) else (
								messageBox "More than 32 frames loaded!\nThat's too many frames!\n\nTry again."
								IFL = #()
								GetIFL.text = "Load IFL File"
							)
					)
			)

		on UseAnimatedTextures changed state do
			if UseAnimatedTextures.checked then
				GetIFL.enabled = fps.enabled = true
			else
				GetIFL.enabled = fps.enabled = false

		on EnableAdvancedParams changed state do
			(
				if EnableAdvancedParams.checked == true then
					(
						if (QueryBox "Changing these values from their\ndefaults will cause more cluts to be created.\n\nDo you want to continue?") == true then
							MaterialDiffuse.enabled = MaterialSpecular.enabled = MaterialSpecularAngle.enabled = MaterialReflect.enabled = MaterialRefract.enabled = MaterialRefractIndex.enabled = MaterialEmissive.enabled = true
						else
							EnableAdvancedParams.checked = false
					) else (
						MaterialDiffuse.enabled = MaterialSpecular.enabled = MaterialSpecularAngle.enabled = MaterialReflect.enabled = MaterialRefract.enabled = MaterialRefractIndex.enabled = MaterialEmissive.enabled = false
					)
			)

		on SurfaceFlags selected s do
			(
				if s == 1 do
					(
						MaterialDiffuse.value = DefaultMat[1]
						MaterialSpecular.value = DefaultMat[2]
						MaterialSpecularAngle.value = DefaultMat[3]
						MaterialReflect.checked = DefaultMat[4]
						MaterialRefract.checked = DefaultMat[5]
						MaterialRefractIndex.value = DefaultMat[6]
						MaterialEmissive.value = DefaultMat[7]
					)

				if s == 2 do
					(
						MaterialDiffuse.value = ShinyMat[1]
						MaterialSpecular.value = ShinyMat[2]
						MaterialSpecularAngle.value = ShinyMat[3]
						MaterialReflect.checked = ShinyMat[4]
						MaterialRefract.checked = ShinyMat[5]
						MaterialRefractIndex.value = ShinyMat[6]
						MaterialEmissive.value = ShinyMat[7]
					)

				if s == 3 do
					(
						MaterialDiffuse.value = MetalMat[1]
						MaterialSpecular.value = MetalMat[2]
						MaterialSpecularAngle.value = MetalMat[3]
						MaterialReflect.checked = MetalMat[4]
						MaterialRefract.checked = MetalMat[5]
						MaterialRefractIndex.value = MetalMat[6]
						MaterialEmissive.value = MetalMat[7]
					)

				if s == 4 do
					(
						MaterialDiffuse.value = WaterMat[1]
						MaterialSpecular.value = WaterMat[2]
						MaterialSpecularAngle.value = WaterMat[3]
						MaterialReflect.checked = WaterMat[4]
						MaterialRefract.checked = WaterMat[5]
						MaterialRefractIndex.value = WaterMat[6]
						MaterialEmissive.value = WaterMat[7]
					)

				if s == 5 do
					(
						MaterialDiffuse.value = MirrorMat[1]
						MaterialSpecular.value = MirrorMat[2]
						MaterialSpecularAngle.value = MirrorMat[3]
						MaterialReflect.checked = MirrorMat[4]
						MaterialRefract.checked = MirrorMat[5]
						MaterialRefractIndex.value = MirrorMat[6]
						MaterialEmissive.value = MirrorMat[7]
					)

				if s == 6 do
					(
						MaterialDiffuse.value = PrelitMat[1]
						MaterialSpecular.value = PrelitMat[2]
						MaterialSpecularAngle.value = PrelitMat[3]
						MaterialReflect.checked = PrelitMat[4]
						MaterialRefract.checked = PrelitMat[5]
						MaterialRefractIndex.value = PrelitMat[6]
						MaterialEmissive.value = PrelitMat[7]
					)

				if s == 7 do
					(
						MaterialDiffuse.value = DinoSkinMat[1]
						MaterialSpecular.value = DinoSkinMat[2]
						MaterialSpecularAngle.value = DinoSkinMat[3]
						MaterialReflect.checked = DinoSkinMat[4]
						MaterialRefract.checked = DinoSkinMat[5]
						MaterialRefractIndex.value = DinoSkinMat[6]
						MaterialEmissive.value = DinoSkinMat[7]
					)
			)
	)

-- ***********************
-- Physics Parameters Rollout
-- ***********************
rollout PhysicsRollout "Physics Properties"
(
	local PhysicsDefaults = #(#(1,false,false,1,5,1,.5,.25,1,true))

	checkBox IgnoreRendering "Ignore Render Params"
	group "Physics Defaults"
		(
				DropDownList DefaultsList items:#("Initial Interface Defaults")
				button GenerateDefaults "Generate Defaults" width:140
		)
	
	group "Sound Material Types"
		(
			DropDownList SoundPropertiesList
		)

	group "Physics Flags"
		(
			checkbox PhysicsPropsTangible "Tangible"
			checkbox PhysicsPropsMoveable "Moveable" offset:[70,-20]
		)
	
	group "Physics Parameters"
		(
			spinner PhysicsPropsDensity "Density:" range:[0,1000,0.1] type:#float fieldwidth:45 offset:[-35,0]
			spinner PhysicsPropsFriction "Friction:" range:[0,10,5] type:#float fieldwidth:45 offset:[-35,0]
			spinner PhysicsPropsElasticity "Elasticity:" range:[0,1000,0.2] type:#float fieldwidth:45 offset:[-35,0]
		)
	
	group "Water Disturbances"
		(
			spinner WDStrength "Strength:" range:[0,20,0.5] type:#float
			spinner WDInterval "Interval:" range:[0.01,50,0.25] type:#float
			spinner WDRadius "Radius:" range:[0.01,20,1] type:#float enabled:false
			checkbox UseObjRadius "Use Object Radius" checked:true
		)

	group "Physics Bounding Boxes"
		(
			dropdownList PhysicsBoundingBoxList "Bounding Boxes"
			pickbutton AddPhysicsBoundingBox "Add" width:70 offset:[-37,0]
			button RemovePhysicsBoundingBox "Remove" width:70 offset:[37,-26]
			button AddPhysicsBoundingBoxSelection "Add Sel" width:70 offset:[-37,0]
			button ClearList "Clear List" width:70 offset:[37,-26]
		)
	
	button close "Close"
	
	on DefaultsList selected s do
		(
			SoundPropertiesList.selection = PhysicsDefaults[s][1]
			PhysicsPropsTangible.checked = PhysicsDefaults[s][2]
			PhysicsPropsMoveable.checked = PhysicsDefaults[s][3]
			PhysicsPropsDensity.value = PhysicsDefaults[s][4]
			PhysicsPropsFriction.value = PhysicsDefaults[s][5]
			PhysicsPropsElasticity.value = PhysicsDefaults[s][6]
			WDStrength.value = PhysicsDefaults[s][7]
			WDInterval.value = PhysicsDefaults[s][8]
			WDRadius.value = PhysicsDefaults[s][9]
			UseObjRadius.checked = PhysicsDefaults[s][10]
		)
	
	on GenerateDefaults pressed do
		(
			CDefaults = #()
			append CDefaults SoundPropertiesList.selection
			append CDefaults PhysicsPropsTangible.checked
			append CDefaults PhysicsPropsMoveable.checked
			append CDefaults PhysicsPropsDensity.value
			append CDefaults PhysicsPropsFriction.value
			append CDefaults PhysicsPropsElasticity.value
			append CDefaults WDStrength.value
			append CDefaults WDInterval.value
			append CDefaults WDRadius.value
			append CDefaults UseObjRadius.checked
			format "\n\n***************************************************\n"
			format "***************************************************\n"
			format "Physics Defaults:\n"
			format "%\n" CDefaults
			format "***************************************************\n"
			format "***************************************************\n\n\n"
		)

	on close pressed do
		removeRollout PhysicsRollout
	
	on PhysicsRollout open do
		if (MaterialTypesArray = (LoadSoundTypes())) != undefined do (SoundPropertiesList.items = MaterialTypesArray)
--	if (MaterialTypesArray = (ReadAudioTxtFile 2)) != undefined do (SoundPropertiesList.items = MaterialTypesArray)

	on SoundPropertiesList selected s do
		CIndex = s

	on UseObjRadius	changed state do
		if state then WDRadius.enabled = false else WDRadius.enabled = true

	on ClearList pressed do
		PhysicsBoundingBoxList.items = #()

	on AddPhysicsBoundingBoxSelection pressed do
		(
			ObjArray = for obj in selection collect obj
			oc = ObjArray.count
			if oc <= 10 then
				(
					for i = 1 to oc do
						(
							CObj = ObjArray[i]
							CurrentPhyBBoxArray = PhysicsBoundingBoxList.items
							if  CurrentPhyBBoxArray.count != 0 do (RemovePhysicsBoundingBox.enabled = true)
							if IsBox CObj != true do (MessageBox "Picked object is not a Box!")
							if findItem CurrentPhyBBoxArray CObj.name != 0 then
								(
									messageBox ("Object " + CObj.name + " is already in the list")
								) else (
									append CurrentPhyBBoxArray CObj.name
									PhysicsBoundingBoxList.items = CurrentPhyBBoxArray
									RemovePhysicsBoundingBox.enabled = true
									PhysicsBoundingBoxList.selection = CurrentPhyBBoxArray.count
								)
						)
				) else (
					messageBox "You can't assign more than 10\nPhysics bounding boxes!"
				)
		)
	
	on AddPhysicsBoundingBox picked obj do
	(
		CObj = obj
		CurrentPhyBBoxArray = PhysicsBoundingBoxList.items
		if  CurrentPhyBBoxArray.count != 0 do (RemovePhysicsBoundingBox.enabled = true)
		if IsBox CObj != true do (MessageBox "Picked object is not a Box!")
		if findItem CurrentPhyBBoxArray CObj.name != 0 then
			(
				messageBox ("Object " + CObj.name + " is already in the list")
			) else (
				append CurrentPhyBBoxArray CObj.name
				PhysicsBoundingBoxList.items = CurrentPhyBBoxArray
				RemovePhysicsBoundingBox.enabled = true
				PhysicsBoundingBoxList.selection = CurrentPhyBBoxArray.count
			)
	)
	
	on RemovePhysicsBoundingBox pressed do
	(
		NewBoxSelection = #()
		CurrentSelection = PhysicsBoundingBoxList.selection
		CurrentPhyBBoxArray = PhysicsBoundingBoxList.items
		for i = 1 to CurrentPhyBBoxArray.count do
			if i != CurrentSelection do
				append NewBoxSelection CurrentPhyBBoxArray[i]
		PhysicsBoundingBoxList.items = NewBoxSelection
		PhysicsBoundingBoxList.selection = 1
	)
	
	on LoadPhysicsProps pressed do 
		(
			if (PhysicsPropsOpenFilename = getOpenFilename caption:"Choose Object Physics Properties File [.OPP]") != undefined do
				(
					f = openfile PhysicsPropsOpenFilename
					MagicNumber = -1
					while not eof f do
						(
							CLine = readline f
							nc = CLine.count
							-- find the colon :
							for i = 1 to nc do
								if substring CLine i 1 == ":" do MagicNumber = i
							-- if : was found start parsing the line			
							if MagicNumber != -1 then
								(
									CParam = substring CLine 1 (MagicNumber - 1)
									CSuffix = (substring CLine (MagicNumber + 1) ((nc - MagicNumber) - 1))
									if CSuffix == "true" then CState = true
									if CSuffix == "false" then CState = false		
									format "Suffix = %\n" CSuffix		
									if CParam == "Static" do (PhysicsPropertiesStatic.checked = CState)
									if CParam == "Intangible" do (PhysicsPropertiesIntangible.checked = CState)
									if CParam == "Moveable" do (PhysicsPropertiesMoveable.checked = CState)
									if CParam == "Mass" do (PhysicsPropertiesMass.value = CSuffix as float)
									if CParam == "Density" do (PhysicsPropertiesDensity.value = CSuffix as float)
									if CParam == "Elasticity" do (PhysicsPropertiesElasticity.value = CSuffix as float)
									if CParam == "Friction" do (PhysicsPropertiesFriction.value = CSuffix as float)
								) else (
									messageBox "Error - no : found"
								)
						)
				)
			close f
		)
	
	on SavePhysicsProps pressed do 
		(
			PhysicsPropsSaveFilename = getsavefilename caption:"Choose Object Physics Properties File [.OPP]"
			f = createfile PhysicsPropsSaveFilename
			format "Static:%\n" PhysicsPropertiesStatic.state to:f
			format "Intangible:%\n" PhysicsPropertiesIntangible.state to:f
			format "Moveable:%\n" PhysicsPropertiesMoveable.state to:f
			format "Mass:%\n" PhysicsPropertiesMass.value to:f
			format "Density:%\n" PhysicsPropertiesDensity.value to:f
			format "Elasticity:%\n" PhysicsPropertiesElasticity.value to:f
			format "Friction:%\n" PhysicsPropertiesFriction.value to:f
			close f
		)
)

-- ***********************
-- Magnet Properties Rollout
-- ***********************

rollout MagnetsRollout "Magnet Properties"
	(
		local MagnetDefaults = #(#(false,false, 0, false, false, false))

		group "Magnet Defaults"
			(
				DropDownList DefaultsList items:#("Initial Interface Defaults")
				button GenerateDefaults "Generate Defualts" width:140
			)

		group "Basic Magnet Parameters"
			(
				radiobuttons MagnetType "MagnetType" labels:#("General", "Hand Pickup", "Shoulder Hold") align:#left
				checkbox MagnetBreakable "Breakable"
				spinner MagnetBreakStrength "Break Strength:" range:[0,1000,0] type:#float enabled:false fieldwidth:45
				checkbox MagnetInstance "Instance"
			)

		group "Degrees of Freedom"
			(
				checkbox MagnetDOF_X "X Axis"
				checkbox MagnetDOF_Y "Y Axis"
				checkbox MagnetDOF_Z "Z Axis"
			)
		
		group "Master/Slave Relationships"
			(
				label MasterLabel "Master:" align:#left offset:[-4,0]
				pickbutton GetMasterObject offset:[0,-20] width:100 align:#right
				label SlaveLabel "Slave:" align:#left offset:[-4,0]
				pickbutton GetSlaveObject offset:[0,-20] width:100 align:#right
				button ClearObjects "Clear Master/Slave Objects" width:140
			)
		button close "Close"
		
		on DefaultsList selected s do
			(
				MagnetPickup.checked = MagnetDefaults[s][1]
				MagnetBreakable.checked = MagnetDefaults[s][2]
				MagnetBreakStrength.value = MagnetDefaults[s][3]
				MagnetDOF_X.checked = MagnetDefaults[s][4]
				MagnetDOF_Y.checked = MagnetDefaults[s][5]
				MagnetDOF_Z.checked = MagnetDefaults[s][6]
			)

		on GenerateDefaults pressed do
			(
				CDefaults = #()
				append CDefaults MagnetPickup.checked
				append CDefaults MagnetBreakable.checked
				append CDefaults MagnetBreakStrength.value
				append CDefaults MagnetDOF_X.checked
				append CDefaults MagnetDOF_Y.checked
				append CDefaults MagnetDOF_Z.checked
				format "\n\n***************************************************\n"
				format "***************************************************\n"
				format "Magnet Defaults:\n"
				format "%\n" CDefaults
				format "***************************************************\n"
				format "***************************************************\n\n\n"
			)

		on ClearObjects pressed do GetMasterObject.text = GetSlaveObject.text = ""

		on GetMasterObject picked obj do
			GetMasterObject.text = obj.name

		on GetSlaveObject picked obj do
			(
				GetSlaveObject.text = obj.name
				if GetMasterObject.text == "" do
					MessageBox "If no master object is defined,\nthe slave will be magneted to the world"
			)

		
		on close pressed do
			removeRollout MagnetsRollout

		on MagnetBreakable changed state do
			(
				if MagnetBreakable.checked then
					MagnetBreakStrength.enabled = true
				else
					MagnetBreakStrength.enabled = false
			)
	)

-- ***********************
-- Trigger Properties Rollout
-- ***********************

rollout TriggersRollout "Trigger Properties"
	(
		local TriggerDefaults = #(#(1,2,0,1,false,100,0,0,"","",0,false,true,false,false,false,false,false,false,false))

		group "Trigger Defaults"
			(
				DropDownList DefaultsList items:#("Initial Interface Defaults")
				button GenerateDefaults "Generate Defaults" width:140
			)

		group "Trigger Options"
			(
				dropdownList TriggerBoundingVolume "Bounding Volume Type:" items:#("Spherical", "Cube")
				DropDownList TriggerActionTypeList "Action Type:" items:#("Voice Over", "Ambient", "Music", "Fade Music", "OverLay", "Fog", "Renderer", "Terrain", "Image Cache", "AI", "Physics", "Substitute Mesh", "Depth Sort", "Sky") selection:2
				label blank3 ""
				spinner TriggerFireDelay "Fire Delay:" range:[0,100,0] type:#float fieldwidth:35 offset:[-40,0]
				spinner TriggerFireCount "Fire Count:" range:[1,100,1] type:#integer fieldwidth:35 offset:[-40,0]
				checkbutton TriggerFireInfinite "Infinity" offset:[50,-23] width:38
				spinner TriggerProbability "Probability:" range:[1,100,100] type:#integer fieldwidth:35 offset:[-40,0]
				spinner CreatureEnterCount "Creature Enter Cnt:" range:[0,10,0] type:#integer fieldwidth:25
				spinner CreatureLeaveCount "Creature Leave Cnt:" range:[0,10,0] type:#integer fieldwidth:25
				
				dropDownList Identifier "Identifier:"
				
				label PickTriggerActionObjectlabel "Object:" offset:[-50,0]
				pickbutton PickTriggerActionObject offset:[23,-20] width:90
				label ExpressionLabel "Expression:" align:#left
				editText Expression
				spinner TriggerAlphaChannel "Alpha Channel: " range:[0,15,0] type:#integer fieldwidth:35
				label blank4 ""
				CheckButton PlayerInTrigger "Player In Trigger" width:145
				CheckButton PlayerEnterTrigger "Player Enter Trigger" width:145 checked:true
				CheckButton PlayerLeaveTrigger "Player Leave Trigger" width:145
				CheckButton CreatureInTrigger "Creature In Trigger" width:145 offset:[0,10]
				CheckButton CreatureEnterTrigger "Creature Enter Trigger" width:145
				CheckButton CreatureLeaveTrigger "Creature Leave Trigger" width:145
				CheckButton ObjectInTrigger "Object In Trigger" width:145 offset:[0,10]
				CheckButton ObjectEnterTrigger "Object Enter Trigger" width:145
				CheckButton ObjectLeaveTrigger "Object Leave Trigger" width:145
				pickButton ChooseActiveObject "Choose Active Object" width:145 offset:[0,20] enabled:false
				label ActiveObjectLabel
			)
		
		button close "Close "
		
		on DefaultsList selected s do
			(
				TriggerBoundingVolume.selection = TriggerDefaults[s][1]
				TriggerActionTypeList.selection = TriggerDefaults[s][2]
				TriggerFireDelay.value = TriggerDefaults[s][3]
				TriggerFireCount.value = TriggerDefaults[s][4]
				TriggerFireInfinite.checked = TriggerDefaults[s][5]
				TriggerProbability.value = TriggerDefaults[s][6]
				CreatureEnterCount.value = TriggerDefaults[s][7]
				CreatureLeaveCount.value = TriggerDefaults[s][8]
				GetTriggerActionFile.text = TriggerDefaults[s][9]
				Expression.text = TriggerDefaults[s][10]
				TriggerAlphaChannel.value = TriggerDefaults[s][11]
				PlayerInTrigger.checked = TriggerDefaults[s][12]
				PlayerEnterTrigger.checked = TriggerDefaults[s][13]
				PlayerLeaveTrigger.checked = TriggerDefaults[s][14]
				CreatureInTrigger.checked = TriggerDefaults[s][15]
				CreatureEnterTrigger.checked = TriggerDefaults[s][16]
				CreatureLeaveTrigger.checked = TriggerDefaults[s][17]
				ObjectInTrigger.checked = TriggerDefaults[s][18]
				ObjectEnterTrigger.checked = TriggerDefaults[s][19]
				ObjectLeaveTrigger.checked = TriggerDefaults[s][20]
			)		

		on GenerateDefaults pressed do
			(
				CDefaults = #()
				append CDefaults TriggerBoundingVolume.selection
				append CDefaults TriggerActionTypeList.selection
				append CDefaults TriggerFireDelay.value
				append CDefaults TriggerFireCount.value
				append CDefaults TriggerFireInfinite.checked
				append CDefaults TriggerProbability.value
				append CDefaults CreatureEnterCount.value
				append CDefaults CreatureLeaveCount.value
				append CDefaults GetTriggerActionFile.text
				append CDefaults Expression.text
				append CDefaults TriggerAlphaChannel.value
				append CDefaults PlayerInTrigger.checked
				append CDefaults PlayerEnterTrigger.checked
				append CDefaults PlayerLeaveTrigger.checked
				append CDefaults CreatureInTrigger.checked
				append CDefaults CreatureEnterTrigger.checked
				append CDefaults CreatureLeaveTrigger.checked
				append CDefaults ObjectInTrigger.checked
				append CDefaults ObjectEnterTrigger.checked
				append CDefaults ObjectLeaveTrigger.checked
				format "\n\n*****************************************************************************************************\n"
				format "*****************************************************************************************************\n"
				format "Trigger Defaults:\n"
				format "%\n" CDefaults
				format "*****************************************************************************************************\n"
				format "*****************************************************************************************************\n\n\n"
			)

		on close pressed do
			removeRollout TriggersRollout

		on TriggersRollout open do
			if (MaterialTypesArray = (ReadAudioTxtFile 1)) != undefined do (Identifier.items = MaterialTypesArray)

		on TriggerActionTypeList selected s do
			(
				if s == 1 do
					(
						addRollout SoundRollout
						RemoveRollout AIRollout
						RemoveRollout FadeMusicRollout
						RemoveRollout FogRollout
						RemoveRollout RendererRollout
						RemoveRollout TerrainActionsRollout
						RemoveRollout PhysicsActionsRollout
						RemoveRollout SkyActionsRollout
						SoundRollout.SpatialType.enabled = SoundRollout.Volume.enabled = SoundRollout.Attenuation.enabled = SoundRollout.FrustumAngle.enabled = SoundRollout.OutSideVolume.enabled = SoundRollout.MaxDistance.enabled = true
						SoundRollout.LoopCount.enabled = SoundRollout.Looped.enabled = SoundRollout.EmitterLabel.enabled = SoundRollout.Emitter.enabled = false
						SoundRollout.Attenuation.value = 0
						SoundRollout.SpatialType.selection = 1
						RemoveRollout DepthSortRollout
						if (MaterialTypesArray = (ReadAudioTxtFile 3)) != undefined do (Identifier.items = MaterialTypesArray)
					)

				if s == 2 do
					(
						addRollout SoundRollout
						RemoveRollout AIRollout
						RemoveRollout FadeMusicRollout
						RemoveRollout FogRollout
						RemoveRollout RendererRollout
						RemoveRollout TerrainActionsRollout
						RemoveRollout PhysicsActionsRollout
						RemoveRollout SkyActionsRollout
						SoundRollout.SpatialType.enabled = SoundRollout.Looped.enabled = SoundRollout.Volume.enabled = SoundRollout.Attenuation.enabled = SoundRollout.FrustumAngle.enabled = SoundRollout.OutSideVolume.enabled = SoundRollout.MaxDistance.enabled = true
						SoundRollout.LoopCount.enabled = SoundRollout.EmitterLabel.enabled = SoundRollout.Emitter.enabled = false
						SoundRollout.Attenuation.value = 1
						SoundRollout.SpatialType.selection = 2
						RemoveRollout DepthSortRollout
						if (MaterialTypesArray = (ReadAudioTxtFile 1)) != undefined do (Identifier.items = MaterialTypesArray)
					)

				if s == 3 do
					(
						addRollout SoundRollout
						RemoveRollout AIRollout
						RemoveRollout FadeMusicRollout
						RemoveRollout FogRollout
						RemoveRollout RendererRollout
						RemoveRollout TerrainActionsRollout
						RemoveRollout PhysicsActionsRollout
						RemoveRollout SkyActionsRollout
						SoundRollout.Looped.enabled = SoundRollout.Volume.enabled = SoundRollout.Attenuation.enabled = SoundRollout.MaxDistance.enabled = true
						SoundRollout.LoopCount.enabled = SoundRollout.OutSideVolume.enabled = SoundRollout.FrustumAngle.enabled = SoundRollout.SpatialType.enabled = SoundRollout.EmitterLabel.enabled = SoundRollout.Emitter.enabled = false
						SoundRollout.Attenuation.value = 0
						SoundRollout.SpatialType.selection = 1
						RemoveRollout DepthSortRollout
						if (MaterialTypesArray = (ReadAudioTxtFile 3)) != undefined do (Identifier.items = MaterialTypesArray)
					)

				-- Fade Music
				if s == 4 do
					(
						RemoveRollout SoundRollout
						RemoveRollout AIRollout
						RemoveRollout FogRollout
						RemoveRollout RendererRollout
						RemoveRollout TerrainActionsRollout
						RemoveRollout PhysicsActionsRollout
						AddRollout FadeMusicRollout
						RemoveRollout DepthSortRollout
						RemoveRollout SkyActionsRollout
					)

				-- Overlay Action
				if s == 5 do
					(
						-- Not Hooked Up Yet
						RemoveRollout FadeMusicRollout
						RemoveRollout SoundRollout
						RemoveRollout AIRollout
						RemoveRollout FogRollout
						RemoveRollout RendererRollout
						RemoveRollout TerrainActionsRollout
						RemoveRollout PhysicsActionsRollout
						RemoveRollout DepthSortRollout
						RemoveRollout SkyActionsRollout
					)

				-- Fog
				if s == 6 do
					(
						RemoveRollout FadeMusicRollout
						RemoveRollout SoundRollout
						removeRollout AIRollout
						RemoveRollout RendererRollout
						RemoveRollout TerrainActionsRollout
						RemoveRollout PhysicsActionsRollout
						AddRollout FogRollout
						RemoveRollout DepthSortRollout
						RemoveRollout SkyActionsRollout
					)

				-- Renderer Rollout
				if s == 7 do
					(
						RemoveRollout FadeMusicRollout
						RemoveRollout SoundRollout
						RemoveRollout AIRollout
						RemoveRollout FogRollout
						RemoveRollout TerrainActionsRollout
						RemoveRollout PhysicsActionsRollout
						AddRollout RendererRollout
						RemoveRollout DepthSortRollout
						RemoveRollout SkyActionsRollout
					)

				-- Terrain Actions
				if s == 8 do
					(
						RemoveRollout FadeMusicRollout
						RemoveRollout SoundRollout
						RemoveRollout AIRollout
						RemoveRollout FogRollout
						RemoveRollout RendererRollout
						RemoveRollout PhysicsActionsRollout
						AddRollout TerrainActionsRollout rolledUp:true
						RemoveRollout DepthSortRollout
						RemoveRollout SkyActionsRollout
					)

				-- Image Cache Actions
				if s == 9 do
					(
						-- Not hooked up yet
						RemoveRollout FadeMusicRollout
						RemoveRollout SoundRollout
						RemoveRollout AIRollout
						RemoveRollout FogRollout
						RemoveRollout RendererRollout
						RemoveRollout TerrainActionsRollout
						RemoveRollout PhysicsActionsRollout
						RemoveRollout DepthSortRollout
						RemoveRollout SkyActionsRollout
					)

				-- AI
				if s == 10 do
					(
						RemoveRollout FadeMusicRollout
						RemoveRollout SoundRollout
						RemoveRollout FogRollout
						RemoveRollout RendererRollout
						RemoveRollout TerrainActionsRollout
						RemoveRollout PhysicsActionsRollout
						RemoveRollout DepthSortRollout
						AddRollout AIRollout
						RemoveRollout SkyActionsRollout
					)

				-- Physics Actions
				if s == 11 do
					(
						RemoveRollout FadeMusicRollout
						RemoveRollout SoundRollout
						RemoveRollout AIRollout
						RemoveRollout FogRollout
						RemoveRollout RendererRollout
						RemoveRollout TerrainActionsRollout
						RemoveRollout DepthSortRollout
						AddRollout PhysicsActionsRollout rolledUp:true
						RemoveRollout SkyActionsRollout
					)
				
				-- depth Sort Actions
				if s == 13 do
					(
						RemoveRollout FadeMusicRollout
						RemoveRollout SoundRollout
						RemoveRollout AIRollout
						RemoveRollout FogRollout
						RemoveRollout RendererRollout
						RemoveRollout TerrainActionsRollout
						RemoveRollout PhysicsActionsRollout
						AddRollout DepthSortRollout rolledUp:true
						RemoveRollout SkyActionsRollout
					)
				-- Sky Actions
				if s == 14 do
					(
						RemoveRollout FadeMusicRollout
						RemoveRollout SoundRollout
						RemoveRollout AIRollout
						RemoveRollout FogRollout
						RemoveRollout RendererRollout
						RemoveRollout TerrainActionsRollout
						RemoveRollout PhysicsActionsRollout
						RemoveRollout DepthSortRollout
						AddRollout SkyActionsRollout rolledUp:true
					)
			)

		on ObjectInTrigger changed state do
			(
				if ObjectEnterTrigger.checked or ObjectLeaveTrigger.checked or ObjectInTrigger.checked then
					ChooseActiveObject.enabled = true
				else
					ChooseActiveObject.enabled = false
			)

		on ObjectEnterTrigger changed state do
			(
				if ObjectEnterTrigger.checked or ObjectLeaveTrigger.checked or ObjectInTrigger.checked then
					ChooseActiveObject.enabled = true
				else
					ChooseActiveObject.enabled = false
			)

		on ObjectLeaveTrigger changed state do
			(
				if ObjectEnterTrigger.checked or ObjectLeaveTrigger.checked or ObjectInTrigger.checked then
					ChooseActiveObject.enabled = true
				else
					ChooseActiveObject.enabled = false
			)

		on ChooseActiveObject picked obj do
			(
				ActiveObject = obj.name
				ActiveObjectLabel.text = ActiveObject
			)

		on TriggerFireInfinite changed state do
			(
				if TriggerFireInfinite.checked == true then
					TriggerFireCount.enabled = false
				else
					TriggerFireCount.enabled = true
			)

		on PickTriggerActionObject picked obj do
			(
				TriggerActionObject = obj
				PickTriggerActionObject.text = TriggerActionObject.name
			)
	)

-- ****************************************************************************************************
-- VoiceOver Rollout
-- ****************************************************************************************************
rollout SoundRollout "Sound Attributes"
	(

		local SoundDefaults = #(#(1,0,1,360,-15,0,true,false))

		group "Sound Defaults"
			(
				DropDownList DefaultsList items:#("Initial Interface Defaults")
				button GenerateDefaults "Generate Defaults" width:140
			)

		DropDownList SpatialType "Spatial Type:" items:#("Stereo", "Pseudo 3D", "Real 3D")
		spinner Volume "Volume: " range:[-100,0,0] type:#float fieldwidth:45
		spinner Attenuation "Attenuation: " range:[0,100,1] type:#float fieldwidth:45
		spinner FrustumAngle "Frustum Angle: " range:[0,360,360] type:#float fieldwidth:45
		spinner OutSideVolume "Outside Volume: " range:[-100,0,-15] type:#float fieldwidth:45
		spinner MaxVolDistance "Max Vol Distance" range:[0,1,0] type:#float fieldwidth:45
		group ""
			(
				spinner MaxDistance "Max Distance:" range:[0,9999,0] fieldwidth:45 enabled:false
				checkbox UseSizeOfEmitter "Use Size of Emitter" checked:true
			)
		checkbox Looped "Looped"
		spinner LoopCount "Count" type:#integer range:[0,9999,0] align:#right offset:[0,-20] fieldwidth:35 enabled:false
		label EmitterLabel "Emitter: " align:#left offset:[-10,0] enabled:false
		PickButton Emitter " none " width:100 offset:[0,-20] align:#right enabled:false

		button close "Close"

		on Looped changed state do
			if state then
				LoopCount.enabled = true
			else
				LoopCount.enabled = false

		on DefaultsList selected s do
			(
				SpatialType.selection = SoundDefaults[s][1]
				Volume.value = SoundDefaults[s][2]
				Attenuation.value = SoundDefaults[s][3]
				FrustumAngle.value = SoundDefaults[s][4]
				OutSideVolume.value = SoundDefaults[s][5]
				MaxDistance.value = SoundDefaults[s][6]
				UseSizeOfEmitter.checked = SoundDefaults[s][7]
				Looped.checked = SoundDefaults[s][8]
			)
		
		on GenerateDefaults pressed do
			(
				Cdefaults = #()
				append CDefaults SpatialType.selection
				append CDefaults Volume.value
				append CDefaults Attenuation.value
				append CDefaults FrustumAngle.value
				append CDefaults OutSideVolume.value
				append CDefaults MaxDistance.value
				append CDefaults UseSizeOfEmitter.checked
				append CDefaults Looped.checked
				format "\n\n***************************************************\n"
				format "***************************************************\n"
				format "Sound Defaults:\n"
				format "%\n" CDefaults
				format "***************************************************\n"
				format "***************************************************\n\n\n"
			)

		on close pressed do
			removeRollout SoundRollout
	)

-- ***********************
-- Light Parameters Rollout
-- ***********************
rollout LightRollout "Light Parameters"
	(
		spinner Intensity "Intensity: " range:[0,1,1] type:#float
		button close "Close"
		
		on close pressed do
			removeRollout LightRollout
	)

-- ***********************
-- Gun Parameters Rollout
-- ***********************
rollout GunRollout "Gun Parameters"
	(
		local GunDefaults = #(
												#(1,2,100,100)\
												)

		group "Gun Defaults"
			(
				DropDownList DefaultsList items:#("Initial Interface Defaults")
				button GenerateDefaults "Generate Defaults" width:140
			)

		spinner GunROF "ROF:" range:[0.1, 100, 1] fieldwidth:45
		spinner GunDamage "Damage: " range:[0.1, 100, 2] fieldwidth:45
		spinner GunRange "Range: " range:[0.1, 1000, 100] fieldwidth:45
		spinner GunPush "Push: " range:[0.1, 1000, 100] fieldwidth:45
		checkbox GunAutoFire "Auto Fire"
		label BarrelLabel "Barrel:" align:#left offset:[-4,0]
		pickbutton GunBarrel width:100 align:#right offset:[0,-20]
		label MFlashOrgLabel "Flash Org:" align:#left offset:[-4,0]
		pickbutton MFlashOrg width:80 align:#right offset:[0,-20]
		label RecoilOrgLabel "Recoil Org:" align:#left offset:[-4,0]
		pickbutton RecoilOrg width:80 align:#right offset:[0,-20]
		DropDownList MFlashes "Muzzle Flash Objects:"
		button AddMF "Add" width:70 offset:[-37,0]
		button ClearMF "Clear" width:70 offset:[37,-26]
		spinner MFlashDuration "Flash Duration:" range:[0,10,0] type:#float
		spinner RecoilForce "Recoil Force:" range:[0,100,0] type:#float
		checkbox RndRot "Random Rotate"
		editText FireSample "Fire Sample:"
		editText EmptyClipSample "Empty Clip:"
		button close "Close"


		on AddMF pressed do
			(
				CObjNames = MFlashes.items
				sel = for obj in selection collect obj.name
				for i = 1 to sel.count do
					append CObjNames sel[i]
				MFlashes.items = CObjNames
			)

		on ClearMF pressed do
			MFlashes.items = #()

		on RecoilOrg picked obj do
			RecoilOrg.text = obj.name

		on MFlashOrg picked obj do
			MFlashOrg.text = obj.name

		on GunBarrel picked obj do
			GunBarrel.text = obj.name

		on DefaultsList selected s do
			(
				GunROF.value = GunDefaults[s][1]
				GunDamage.value = GunDefaults[s][2]
				GunRange.value = GunDefaults[s][3]
				GunPush.value = GunDefaults[s][4]
			)
		
		on generateDefaults pressed do
			(
				CDefaults = #()	
				append CDefaults GunROF.value
				append CDefaults GunDamage.value
				append CDefaults GunRange.value
				append CDefaults GunPush.value
				format "\n\n***************************************************\n"
				format "***************************************************\n"
				format "Gun Defaults:\n"
				format "%\n" CDefaults
				format "***************************************************\n"
				format "***************************************************\n\n\n"
			)

		on close pressed do
			removeRollout GunRollout
	)

-- ***********************
-- AI Parameters Rollout
-- ***********************
rollout AIRollout "AI Action Parameters"
	(
		local AIDefaults = #(#(0,false,0,false,0,false,0,false,0,false,0,false,0,false,0,false))
		
		group "AI Defaults"
			(
				DropDownList DefaultsList items:#("Initial Interface Defaults")
				button GenerateDefaults "Generate Defaults" width:140
			)

		label TargetLabel "Target:" align:#left offset:[-4,0]
		pickbutton AITarget width:100 align:#right offset:[0,-20]

		spinner AIFear "Fear: " range:[0,1,0] fieldwidth:35 offset:[-20,0]
		checkbox UseAIFear align:#right offset:[10,-20]
		spinner AILove "Love: " range:[0,1,0] fieldwidth:35 offset:[-20,0]
		checkbox UseAILove align:#right offset:[10,-20]
		spinner AIAnger "Anger: " range:[0,1,0] fieldwidth:35 offset:[-20,0]
		checkbox UseAIAnger align:#right offset:[10,-20]
		spinner AIHunger "Hunger: " range:[0,1,0] fieldwidth:35 offset:[-20,0]
		checkbox UseAIHunger align:#right offset:[10,-20]
		spinner AIThirst "Thirst: " range:[0,1,0] fieldwidth:35 offset:[-20,0]
		checkbox UseAIThirst align:#right offset:[10,-20]
		spinner AIFatigue "Fatigue: " range:[0,1,0] fieldwidth:35 offset:[-20,0]
		checkbox UseAIFatigue align:#right offset:[10,-20]
		spinner AIPain "Pain: " range:[0,1,0] fieldwidth:35 offset:[-20,0]
		checkbox UseAIPain align:#right offset:[10,-20]
		spinner AISolidity "Solidity: " range:[0,1,0] fieldwidth:35 offset:[-20,0]
		checkbox UseAISolidity align:#right offset:[10,-20]
		
		button close "Close"

		on AITarget picked obj do
			AITarget.text = obj.name
			
		on DefaultsList selected s do
			(
				AIFear.value = AIDefaults[s][1]
				UseAIFear.checked = AIDefaults[s][2]

				AILove.value = AIDefaults[s][3]
				UseAILove.checked = AIDefaults[s][4]

				AIAnger.value = AIDefaults[s][5]
				UseAIAnger.checked = AIDefaults[s][6]

				AIHunger.value = AIDefaults[s][7]
				UseAIHunger.checked = AIDefaults[s][8]

				AIThirst.value = AIDefaults[s][9]
				UseAIThirst.checked = AIDefaults[s][10]

				AIFatigue.value = AIDefaults[s][11]
				UseAIFatigue.checked = AIDefaults[s][12]

				AIPain.value = AIDefaults[s][13]
				UseAIPain.checked = AIDefaults[s][14]

				AISolidity.value = AIDefaults[s][15]
				UseAISolidity.checked = AIDefaults[s][16]
			)

		on generateDefaults pressed do
			(
				CDefaults = #()	
				append CDefaults AIFear.value
				append CDefaults UseAIFear.checked
				append CDefaults AILove.value
				append CDefaults UseAILove.checked
				append CDefaults AIAnger.value
				append CDefaults UseAIAnger.checked
				append CDefaults AIHunger.value
				append CDefaults UseAIHunger.checked
				append CDefaults AIThirst.value
				append CDefaults UseAIThirst.checked
				append CDefaults AIFatigue.value
				append CDefaults UseAIFatigue.checked
				append CDefaults AIPain.value
				append CDefaults UseAIPain.checked
				append CDefaults AISolidity.value
				append CDefaults UseAISolidity.checked
				format "\n\n***************************************************\n"
				format "***************************************************\n"
				format "AI Defaults:\n"
				format "%\n" CDefaults
				format "***************************************************\n"
				format "***************************************************\n\n\n"
			)
		
		on AIFear changed state do UseAIFear.checked = true
		on AILove changed state do UseAILove.checked = true
		on AIAnger changed state do UseAIAnger.checked = true
		on AIHunger changed state do UseAIHunger.checked = true
		on AIThirst changed state do UseAIThirst.checked = true
		on AIFatigue changed state do UseAIFatigue.checked = true
		on AIPain changed state do UseAIPain.checked = true
		on AISolidity changed state do UseAISolidity.checked = true

		on close pressed do removerollout AIRollout
	)

-- ***********************
-- Depth Sort Actions
-- ***********************
rollout DepthSortRollout "Depth Sort Actions"
	(
		group "Options"
			(
				spinner NearTol "Near Tol:" range:[0,1,0.0001] fieldwidth:35 offset:[-10,0]
				checkbox UseNearTol offset:[17,-21] align:#right
				spinner FarTol "Far Tol:" range:[0,1,0.078] fieldwidth:35 offset:[-10,0]
				checkbox UseFarTol offset:[17,-21] align:#right
				spinner NearZ "Near Z:" range:[0,1,.006] fieldwidth:35 offset:[-10,0]
				checkbox UseNearZ offset:[17,-21] align:#right
				spinner FarZ "Far Z:" range:[0,1,.33] fieldwidth:35 offset:[-10,0]
				checkbox UseFarZ offset:[17,-21] align:#right
				spinner FarZNo "Far Z No:" range:[0,1,.5] fieldwidth:35 offset:[-10,0]
				checkbox UseFarZNo offset:[17,-21] align:#right
				spinner SortPixTol "Sort Pixel Tol:" range:[0,1,0.64] fieldwidth:35 offset:[-10,0]
				checkbox UseSortPixTol offset:[17,-21] align:#right
				spinner MaxNumToSort "Max to Sort:" range:[100,2000,1100] type:#integer fieldwidth:35 offset:[-10,0]
				checkbox UseMaxNumToSort offset:[17,-21] align:#right
				spinner Sort2Part "Sort 2 Part at:" range:[100, 2000, 750] type:#integer fieldwidth:35 offset:[-10,0]
				checkbox UseSort2Part offset:[17,-21] align:#right
				spinner Sort4Part "Sort 4 Part at:" range:[100,2000, 1500] type:#integer fieldwidth:35 offset:[-10,0]
				checkbox UseSort4Part offset:[17,-21] align:#right

			)

		group "Terrain Tolerances"
			(
				spinner TerrainNearTol "Terrain Near Tol:" range:[0,1,.00001] fieldwidth:35 offset:[-10,0]
				checkbox UseTerrainNearTol offset:[17,-21] align:#right
				spinner TerrainFarTol "Terrain Far Tol:" range:[0,1,.21] fieldwidth:35 offset:[-10,0]
				checkbox UseTerrainFarTol offset:[17,-21] align:#right
				spinner TerrainNearZ "Terrain Near Z:" range:[0,1,.0001] fieldwidth:35 offset:[-10,0]
				checkbox UseTerrainNearZ offset:[17,-21] align:#right
				spinner TerrainFarZ "Terrain Far Z:" range:[0,1,.29] fieldwidth:35 offset:[-10,0]
				checkbox UseTerrainFarZ offset:[17,-21] align:#right
				checkbox SeperateTol "Use Seperate Tol" offset:[22,0]
				checkbox UseSeperateTol offset:[-3,-20] align:#left
			)

		button close "Close"
		on close pressed do removeRollout DepthSortRollout

		on NearTol changed state do UseNearTol.checked = true
		on FarTol changed state do UseFarTol.checked = true
		on NearZ changed state do UseNearZ.checked = true
		on FarZ changed state do UseFarZ.checked = true
		on FarZNo changed state do UseFarZNo.checked = true
		on SortPixTol changed state do UseSortPixTol.checked = true
		on MaxNumToSort changed state do UseMaxNumToSort.checked = true
		on Sort2Part changed state do UseSort2Part.checked = true
		on Sort4Part changed state do UseSort4Part.checked = true

		on TerrainNearTol changed state do UseTerrainNearTol.checked = true
		on TerrainFarTol changed state do UseTerrainFarTol.checked = true
		on TerrainNearZ changed state do UseTerrainNearZ.checked = true
		on TerrainFarZ changed state do UseTerrainFarZ.checked = true
		on SeperateTol changed state do UseSeperateTol.checked = true
	)

-- ***********************
-- Mesh Substitution Actions
-- ***********************
rollout MeshSubstRollout "Mesh Substitution Actions"
	(
		label Tl "Target: " align:#left offset:[-4,0]
		pickbutton Target width:100 offset:[0,-20] align:#right
		spinner Substitute "Substitute:" range:[0,20,0] type:#integer

		button close "Close"
		on close pressed do
			removeRollout MeshSubstRollout
	)

-- ***********************
-- Physics Actions
-- ***********************
rollout PhysicsActionsRollout "Physics Actions"
	(
		
		label Tl "Target: " align:#left offset:[-4,0]
		pickbutton PhysicsTarget width:100 offset:[0,-20] align:#right

		spinner XVel "X Velocity:" range:[-100,100,0] fieldwidth:35 offset:[-7,0]
		checkbox UseXVel offset:[20,-20] align:#right

		spinner YVel "Y Velocity:" range:[-100,100,0] fieldwidth:35 offset:[-7,0]
		checkbox UseYVel offset:[20,-20] align:#right

		spinner ZVel "Z Velocity:" range:[-100,100,0] fieldwidth:35 offset:[-7,0]
		checkbox UseZVel offset:[20,-20] align:#right

		Pickbutton ChooseTape "Choose Tape Object" width:140 filter:TpFltr
		
		label l1 "Values are in M per sec scale"

		on PhysicsTarget picked obj do
			PhysicsTarget.text = obj.name

		on ChooseTape picked obj do
			(
				vec = (obj.target.pos - obj.pos)
				XVel.value = vec.x
				YVel.value = vec.y
				ZVel.value = vec.z
				UseXVel.checked = true
				UseYVel.checked = true
				UseZVel.checked = true
			)

		on XVel changed state do UseXVel.checked = true
		on YVel changed state do UseYVel.checked = true
		on ZVel changed state do UseZVel.checked = true

		button close "Close"
		on close pressed do removeRollout PhysicsActionsRollout
	)

-- ***********************
-- Terrain Actions
-- ***********************
rollout TerrainActionsRollout "Terrain Actions"
	(
		spinner PixelTol "Pixel Tolerance:" range:[1,10,0] fieldwidth:35 offset:[-7,0]
		checkbox UsePixelTol offset:[20,-20] align:#right
		spinner PixelTolRelax "Pixel Tol Far:" range:[5,50,0] fieldwidth:35 offset:[-7,0]
		checkbox UsePixelTolRelax offset:[20,-20] align:#right
		spinner EvalDelayFactor "Eval Delay Factor:" range:[0,20,0] fieldwidth:35 offset:[-7,0]
		checkbox UseEvalDelayFactor offset:[20,-20] align:#right
		spinner TexelsPerMeter "Texels per Meter:" range:[0.25,128,32] fieldwidth:35 offset:[-7,0]
		checkbox UseTexelsPerMeter offset:[20,-20] align:#right
		spinner TexelsPerPixel "Texels per Pixel:" range:[0.01,2,0.4] fieldwidth:35 offset:[-7,0]
		checkbox UseTexelsPerPixel offset:[20,-20] align:#right
		spinner DistanceMultiply "Distance Multiply:" range:[0,100,1] fieldwidth:35 offset:[-7,0]
		checkbox UseDistanceMultiply offset:[20,-20] align:#right
		spinner MinNodeSize "Min Node Size:" range:[1,4096,8] fieldwidth:35 offset:[-7,0]
		checkbox UseMinNodeSize offset:[20,-20] align:#right
		spinner MaxSizeTexture "Max Size Texture:" range:[1,4096,512] fieldwidth:35 offset:[-7,0]
		checkbox UseMaxSizeTexture offset:[20,-20] align:#right
		spinner MaxSizeShadow "Max Size Shadow:" range:[1,4096,256] fieldwidth:35 offset:[-7,0]
		checkbox UseMaxSizeShadow offset:[20,-20] align:#right
		spinner MaxSizeMvShadow "Max Size Mv Shdw:" range:[1,4096,128] fieldwidth:35 offset:[-7,0]
		checkbox UseMaxSizeMvShadow offset:[20,-20] align:#right
		spinner TextureLifetime "Texture Lifetime:" range:[0,100,20] type:#integer fieldwidth:35 offset:[-7,0]
		checkbox UseTextureLifetime offset:[20,-20] align:#right

		checkbox Dynamic "Dynamic" offset:[20,0]
		checkbox UseDynamic offset:[-8,-20] align:#left
		checkbox TextureStatic "Texture Static" offset:[20,0]
		checkbox UseTextureStatic offset:[-8,-20] align:#left
		checkbox PerTriangle "Per Triangle" offset:[20,0]
		checkbox UsePerTriangle offset:[-8,-20] align:#left
		checkbox AdjustForOrientation "Adjust for Orientation" offset:[20,0]
		checkbox UseAdjustForOrientation offset:[-8,-20] align:#left
		checkbox AdjustForDistance "Adjust for Distance" offset:[20,0]
		checkbox UseAdjustForDistance offset:[-8,-20] align:#left
		checkbox PurgeDistance "Purge Distance" offset:[20,0]
		checkbox UsePurgeDistance offset:[-8,-20] align:#left

		on PixelTol changed state do UsePixelTol.checked = true
		on PixelTolRelax changed state do UsePixelTolRelax.checked = true
		on EvalDelayFactor changed state do UseEvalDelayFactor.checked = true
		on TexelsPerMeter changed state do UseTexelsPerMeter.checked = true
		on TexelsPerPixel changed state do UseTexelsPerPixel.checked = true
		on DistanceMultiply changed state do UseDistanceMultiply.checked = true
		on MinNodeSize changed state do UseMinNodeSize.checked = true
		on MaxSizeTexture changed state do UseMaxSizeTexture.checked = true
		on MaxSizeShadow changed state do UseMaxSizeShadow.checked = true
		on MaxSizeMvShadow changed state do UseMaxSizeMvShadow.checked = true
		on TextureLifetime changed state do UseTextureLifetime.checked = true

		on Dynamic changed state do UseDynamic.checked = true
		on TextureStatic changed state do UseTextureStatic.checked = true
		on PerTriangle changed state do UsePerTriangle.checked = true
		on AdjustForOrientation changed state do UseAdjustForOrientation.checked = true
		on AdjustForDistance changed state do UseAdjustForDistance.checked = true
		on PurgeDistance changed state do UsePurgeDistance.checked = true

		button close "Close"
		on close pressed do removeRollout TerrainActionsRollout
	)

-- ***********************
-- Image Cache Actions
-- ***********************
rollout ImageCacheRollout "Image Cache Actions"
	(
		spinner PixelRatio "Pixel Ratio: " fieldwidth:35 offset:[-20,5]
		checkbox UsePixelRatio offset:[10,-20] align:#right
		spinner MinPixels "Min Pixels: " type:#integer
		checkbox UseMinPixels offset:[10,-20] align:#right
		spinner CacheAge "Cache Age: " type:#integer
		checkbox UseCacheAge offset:[10,-20] align:#right
		
		checkbox cacheActive "Cache Active" checked:true
		checkbox cacheIntersect "Cache Intersect"

		button close "Close"
		on close pressed do
			removeRollout ImageCacheRollout
	)

-- ***********************
-- Renderer Actions
-- ***********************
rollout RendererRollout "Renderer Actions"
	(
		spinner AmbientLight "Ambient Light:" range:[0,1,.2] fieldwidth:35 offset:[-20,5]
		checkbox UseAmbientLight offset:[10,-20] align:#right
		spinner FarClip "Far Clip Plane:" range:[0.001,5000,1000] fieldwidth:35 offset:[-20,5]
		checkbox UseFarClip offset:[10,-20] align:#right
		spinner NearClip "Near Clip Plane:" range:[0.001,5000,0.1] fieldwidth:35 offset:[-20,5]
		checkbox UseNearClip offset:[10,-20] align:#right
		group "Object Culling"
			(
				spinner CullMaxDist "Distance:" range:[10,1000,800] fieldwidth:45 offset:[-20,5]
				checkbox UseCullMaxDist offset:[10,-20] align:#right
				spinner CullMaxRadius "Radius:" range:[0.1,10,8] fieldwidth:45 offset:[-20,5]
				checkbox UseCullMaxRadius offset:[10,-20] align:#right
				spinner CullMaxDistShadow "Dist Shdw:" range:[10,100,100] fieldwidth:45 offset:[-20,5]
				checkbox UseCullMaxDistShadow offset:[10,-20] align:#right
				spinner CullMaxRadiusShadow "Radius Shdw:" range:[0.1,10,4] fieldwidth:45 offset:[-20,5]
				checkbox UseCullMaxRadiusShadow offset:[10,-20] align:#right
			)

		button close "Close"

		on AmbientLight changed state do UseAmbientLight.checked = true
		on CullMaxDist changed state do UseCullMaxDist.checked = true
		on CullMaxRadius changed state do UseCullMaxRadius.checked = true
		on CullMaxDistShadow changed state do UseCullMaxDistShadow.checked = true
		on CullMaxRadiusShadow changed state do UseCullMaxRadiusShadow.checked = true
		on FarClip changed state do
			(
				UseFarClip.checked = true
				if FarClip.value < NearClip.value do
					NearClip.value = FarClip.value
			)
		on NearClip changed state do
			(
				UseNearClip.checked = true
				if NearClip.value > FarClip.value do
					FarClip.value = NearClip.value
			)
		on close pressed do removeRollout RendererRollout
	)

-- ***********************
-- Fog Actions Rollout
-- ***********************
rollout FogRollout "Fog Actions"
	(
		colorPicker FogColor "Fog Color"
		checkbox UseFogColor offset:[10,-22] align:#right
		radiobuttons FogType labels:#("Linear", "Exponential")
		checkbox UseFogType offset:[10,-28] align:#right
		spinner FogPower "Fog Power:" range:[0.001,5,0.001] fieldwidth:45 offset:[-20,5]
		checkbox UseFogPower align:#right offset:[10,-20]
		spinner FogHalf "Fog Half:" range:[0.001,5,0.001] fieldwidth:45 offset:[-20,0]
		checkbox UseFogHalf align:#right offset:[10,-20]
		
		button close "Close"

		on FogColor changed state do UseFogColor.checked = true
		on FogType changed state do UseFogType.checked = true
		on FogPower changed state do UseFogPower.checked = true
		on FogHalf changed state do UseFogHalf.checked = true

		on close pressed do
			removeRollout FogRollout
	
	)
-- ***********************
-- Overlay Rollout
-- ***********************
rollout OverlayRollout "Overlay Actions"
	(
		label BMLabel "Bitmap:" align:#left offset:[-4,0]
		button OverlayBitmap width:100 align:#right offset:[0,-20]
		checkbox OverlayCenter "Center"
		checkbox OverlayDiscard "Discard"
		spinner OverlayXPos "X Pos: " range:[0,1024,0] type:#integer
		spinner OverlayYPos "Y Pos: " range:[0,768,0] type:#integer
	
		button close "Close" 
		
		on close pressed do
			removeRollout	OverlayRollout
	
	)
-- ***********************
-- Fade Music Rollout
-- ***********************
rollout FadeMusicRollout "Fade Music Actions"
	(
		spinner VolFader "Vol Fader:" range:[-100,100,0]
		button close "Close"

		on close pressed do
			removeRollout FadeMusicRollout
	)

-- ***********************
-- Sky Rollout
-- ***********************
rollout SkyRollout "Sky"
	(
		spinner SkyFlatColorIndex "Flat Color Pal Index:" range:[0,63,0] type:#integer fieldwidth:35
		button close "Close"

		on close pressed do removeRollout SkyRollout
	)

-- ***********************
-- Sky Actions Rollout
-- ***********************
rollout SkyActionsRollout "Sky Actions"
	(
		spinner SkyActionHeight "Height:" range:[500,2500,1000] fieldwidth:45 offset:[-20,5]
		checkbox UseSkyActionHeight align:#right offset:[10,-20]
		spinner SkyActionScale "Scale:" range:[0.001, 0.2,0.055] fieldwidth:45 offset:[-20,5]
		checkbox UseSkyActionScale align:#right offset:[10,-20]
		spinner SkyActionNearFogBand "Near Fog Band" range:[0,1,0.75] fieldwidth:45 offset:[-20,5]
		checkbox UseSkyActionNearFogBand align:#right offset:[10,-20]
		spinner SkyActionFarFogBand "Far Fog Band" range:[0,1,0.95] fieldwidth:45 offset:[-20,5]
		checkbox UseSkyActionFarFogBand align:#right offset:[10,-20]
		spinner SkyActionWindSpeedX "Wind Speed X:" range:[0,1000,5] fieldwidth:45 offset:[-20,5]
		checkbox UseSkyActionWindSpeedX align:#right offset:[10,-20]
		spinner SkyActionWindSpeedY "Wind Speed Y:" range:[0,1000,5] fieldwidth:45 offset:[-20,5]
		checkbox UseSkyActionWindSpeedY align:#right offset:[10,-20]
		spinner SkyActionSubdivision "Subdivision:" range:[2,64,32] type:#integer fieldwidth:45 offset:[-20,5]
		checkbox UseSkyActionSubdivision align:#right offset:[10,-20]
		checkbox SkyActionTexture "Texture" offset:[20,0]
		checkbox UseSkyActionTexture offset:[-8,-20] align:#left
		checkbox SkyActionFillScreen "FillScreen" offset:[20,0]
		checkbox UseSkyActionFillScreen offset:[-8,-20] align:#left

		button close "Close"

		on SkyActionHeight changed state do UseSkyActionHeight.checked = true
		on SkyActionScale changed state do UseSkyActionScale.checked = true
		on SkyActionNearFogBand changed state do UseSkyActionNearFogBand.checked = true
		on SkyActionFarFogBand changed state do UseSkyActionFarFogBand.checked = true
		on SkyActionWindSpeedX changed state do UseSkyActionWindSpeedX.checked = true
		on SkyActionWindSpeedY changed state do UseSkyActionWindSpeedY.checked = true
		on SkyActionSubdivision changed state do UseSkyActionSubdivision.checked = true
		on SkyActionTexture changed state do UseSkyActionTexture.checked = true
		on SkyActionFillScreen changed state do UseSkyActionFillScreen.checked = true

		on close pressed do removeRollout SkyActionsRollout
	)
