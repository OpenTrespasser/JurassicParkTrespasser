-- bar






                                
            
          
 
  
 

 
                                        

-- baz
fn ReadAudioTxtFile index = 
	(
		if index == 1 do
			filename = "\\\Trespasser\SiteB\AudioPackFiles\Ambient.txt"
		if index == 2 do
			filename = "\\\Trespasser\SiteB\AudioPackFiles\Effects.txt"
		if index == 3 do
			filename = "\\\Trespasser\SiteB\AudioPackFiles\Stream.txt"

		sampleArray = #("")
		f = openFile fileName
		while not eof f do
			(
				CLine = readline f
				if findString CLine "IDENTIFIER:" != undefined do
					if CLine[13] == "'" do
						(
							cnt = 0
							for i = 14 to CLine.count do
								if CLine[i] == "'" then
									(
										CIndex = i
										Exit
									) else (
										cnt += 1
									)
							Identifier = (substring CLine 14 cnt)
							append sampleArray Identifier
						)
			)
		close f
		return sampleArray
	)

fn PruneNewLines str = 
	(
		NotFound = false
		while NotFound == false do
			(
				if (CIndex = findstring str "\r\r\n") != undefined then
					(
						if CIndex != (str.count - 2) then
							str = ((substring str 1 (CIndex - 1)) + "\n" + (substring str (CIndex + 3) (str.count - (CIndex + 3))))
						else
							NotFound = true
					) else (
						NotFound = true
					)
			)
		return str
	)

	fn TpFltr obj = (ClassOf obj == Tape)

	fn OPWordArray textline = 
		(
			local c
			local Line = ""
			local Lines = #()
			for i = 1 to textline.count do
				(
					c = textline[i]
					if c != ";" then
						if (c != "\n") and (c != "\r") do
							Line = Line + c
					else
						(
							Line = Line + ";"
							append Lines Line
							Line = ""
						)
				)
			return Lines
		)

	fn FloatAsString f = 
		(
			CString = f as string
			Convert = true
			for i = 1 to CString.count do
				if CString[i] == "." do
					Convert = false
			if Convert == true do
				CString = (CString + ".0")
			return CString
		)
	
	fn IsBox Obj =
		(
			local exitCode = 1
			if obj.numfaces != 12 do (exitCode = -1)
			if obj.numverts != 8 do (exitCode = -1)
			if exitCode == 1 then return true else return false
		)

	fn LoadSoundTypes = 
		(
			local MaterialTypesArray = #("")
			if (f = openfile "\\\JPII_PC\JPII_PC\Users\Brady\\Test Scenes\Material Source Type\Material Types.txt") != undefined then
				(
					while not eof f do
						(
							MatName = readline f
							if MatName.count >=3 and MatName[1] != ";" do
								(
									if MatName[MatName.count] == "\n" then MatName = replace MatName MatName.count 1 ""
									append MaterialTypesArray MatName
								)
						)
					close f
					return MaterialTypesArray
				) else (
					MessageBox "Sound types file not found!"
					return undefined
				)
		)

global 	MaterialTypesArray = #(), TriggerActionObject, ActiveObject,
				AmbientSamplesFileName = "\\Trespasser\SiteB\AudioPackFiles\Ambient.txt",
				EffectsSamplesFileName = "\\Trespasser\SiteB\AudioPackFiles\Effects.txt",
				StreamSamplesFileName = "\\Trespasser\SiteB\AudioPackFiles\Stream.txt"

-- ****************************************************************************************************
-- Utility Start
-- ****************************************************************************************************
Utility ObjProps "Object Properties"
(

local RenderingRollout, PhysicsRollout, MagnetsRollout, TriggersRollout, PVARollout, AIRollout,
			OverlayRollout, RendererRollout, GunRollout, TerrainTextureRollout, SettingsRollout,
			PhysicsActionsRollout, SoundRollout, LightRollout, FadeMusicRollout, FogRollout,
			TerrainActionsRollout, ImageCacheRollout, MeshSubstRollout, DepthSortRollout,
			MaterialTypesArray, PhysicsTangibleString, PhysicsMoveableString, TerrainMaterialTypesArray,
			PhysicsDensityString, PhysicsFrictionString, PhysicsElasticityString,PhysicsSoundMaterialString,
			MasterPropBuffer, ObjPropsEditing,CIndex, IFL, DRData = #(120,60,30,15), SkyRollout,
			SkyActionsRollout, TeleporterRollout

	group "Object Class"
		(
			ListBox GeneralObjectType items:#("CInstance Object", "Player", "Terrain Texture Object", "Terrain Extents Object", "Terrain Placement Object", "Animal","Magnet","Trigger", "Settings", "Water Object", "Water Disturbance", "Light", "Sky", "Gun", "Backdrop Object", "Marker", "Teleporter") height:5
			-- 1:		CInstance Object
			-- 2:		Player
			-- 3:		Terrain Texture Object
			-- 4:		Terrain Extents Object
			-- 5:		Terrain Placement Object
			-- 6:		Animal
			-- 7:		Magnet
			-- 8:		Trigger
			-- 9:		Settings
			-- 10:	Water Object
			-- 11:	Water Disturbance
			-- 12:	Light
			-- 13:	Sky
			-- 14:	Gun
			-- 15:	Backdrop Object
			-- 16:	Marker Object
			-- 17:	Teleporter Object
		)

	button WriteObjectProps "Write Props to Selection" width:140
	checkbox Preview "Preview Properties"
	checkbox AppendALLProps "Append with Obj Class"
	checkbox AppendProps "Append w/o Obj Class"
	checkbox CleanInterface "Reset Interface"
	
	on Preview changed state do
		if Preview.checked then
			(
				AppendALLProps.enabled = AppendProps.enabled = false
				WriteObjectProps.text = "Write Props to Listener"
			) else (
				AppendALLProps.enabled = AppendProps.enabled = true
				WriteObjectProps.text = "Write Props to Selection"
			)
	
	group "Copy Properties Options"
		(
			PickButton GetMaster "Choose Master Object" width:140
			button ApplyToSelected "Apply to Selected" width:140
		)
	
	group "Obj Prop Utils"
		(
			pickbutton getCount "Get Count" width:140
			button countSelection "Count Selection" width:140
			button PreviewSelection "Preview Selection" width:140
			checkbox PreviewNoProps "Preview w/o Props"
		)

	on AppendALLProps changed state do
		if AppendALLProps.checked do
			AppendProps.checked = false

	on AppendProps changed state do
		if AppendProps.checked do
			AppendALLProps.checked = false

	on getcount picked obj do
		(
			count = (getUserPropBuffer obj).count
			MessageBox (count as string + " characters long.")
		)

	on PreviewSelection pressed do
		(
		
			Sel = Selection as array
			for i = 1 to sel.count do
				if PreviewNoProps.checked then
					(
						NewBuffer = PruneNewLines CBuffer
						format "\n\n--------------------------- \"%\" ---------------------------------\n" sel[i].name
						format "%\n----------------------------------------------------------------------------------\n" NewBuffer
					) else (
						if (CBuffer = getUserPropBuffer Sel[i]) != "" do
							(
								NewBuffer = PruneNewLines CBuffer
								format "\n\n--------------------------- \"%\" ---------------------------------\n" sel[i].name
								format "%\n----------------------------------------------------------------------------------\n" NewBuffer
							)
					)
		)

	on countSelection pressed do
		(
			TotalCount = 0
			ObjArray = for obj in selection collect obj
			for i = 1 to ObjArray.count do
				(
					count = (getUserPropBuffer ObjArray[i]).count
					if count != 0 do
						(
							if count > 500 then
								format "%: %\t\t<<OVER LIMIT>>\n" ObjArray[i].name (count as integer)
							else 
								format "%: %\n" ObjArray[i].name (count as integer)

							TotalCount = count + TotalCount
						)
				)
			format "TOTAL: %\n" (TotalCount as integer)
		)

	on ApplyToSelected pressed do
		(
			if MasterPropBuffer != undefined do
				(
					ObjArray = for obj in selection collect obj
					for i = 1 to ObjArray.count do
						SetUserPropBuffer ObjArray[i] MasterPropBuffer
				)
		)

	on GetMaster picked obj do
		MasterPropBuffer = GetUserPropBuffer obj

	on GeneralObjectType selected s do
		(
			-- CInstance Type
			if s == 1 do
				(
					AddRollout RenderingRollout rolledup:true
					AddRollout PhysicsRollout rolledup:true
					RemoveRollout MagnetsRollout
					RemoveRollout TriggersRollout
					RemoveRollout TerrainTextureRollout
					RemoveRollout SettingsRollout
					RemoveRollout SoundRollout
					RemoveRollout LightRollout
					RemoveRollout GunRollout
					RemoveRollout AIRollout
					RemoveRollout FadeMusicRollout
					RemoveRollout SkyRollout
					RemoveRollout SkyActionsRollout
					RemoveRollout TeleporterRollout
				)

			-- Terrain Texture Rollout
			if s == 3 do
				(
					AddRollout TerrainTextureRollout rolledup:true
					RemoveRollout RenderingRollout
					RemoveRollout PhysicsRollout
					RemoveRollout MagnetsRollout
					RemoveRollout TriggersRollout
					RemoveRollout SettingsRollout
					RemoveRollout SoundRollout
					RemoveRollout LightRollout
					RemoveRollout GunRollout
					RemoveRollout AIRollout
					RemoveRollout FadeMusicRollout
					RemoveRollout SkyRollout
					RemoveRollout SkyActionsRollout
					RemoveRollout TeleporterRollout
				)

			-- Types without options
			if s == 2 or s == 4 or s == 5 or s == 6 or s == 10 or s == 15 or s == 16 do
				(
					RemoveRollout TerrainTextureRollout
					RemoveRollout RenderingRollout
					RemoveRollout PhysicsRollout
					RemoveRollout MagnetsRollout
					RemoveRollout TriggersRollout
					RemoveRollout SettingsRollout
					RemoveRollout SoundRollout
					RemoveRollout LightRollout
					RemoveRollout GunRollout
					RemoveRollout AIRollout
					RemoveRollout FadeMusicRollout
					RemoveRollout SkyRollout
					RemoveRollout SkyActionsRollout
					RemoveRollout TeleporterRollout
				)

			-- Magnets
			if s == 7 do
				(
					AddRollout MagnetsRollout rolledup:true
					RemoveRollout TerrainTextureRollout
					RemoveRollout RenderingRollout
					RemoveRollout PhysicsRollout
					RemoveRollout TriggersRollout
					RemoveRollout SettingsRollout
					RemoveRollout SoundRollout
					RemoveRollout LightRollout
					RemoveRollout GunRollout
					RemoveRollout AIRollout
					RemoveRollout FadeMusicRollout
					RemoveRollout SkyRollout
					RemoveRollout SkyActionsRollout
					RemoveRollout TeleporterRollout
				)

			-- Triggers
			if s == 8 do
				(
					AddRollout TriggersRollout rolledup:true
					AddRollout SoundRollout rolledup:true
					RemoveRollout TerrainTextureRollout
					RemoveRollout RenderingRollout
					RemoveRollout PhysicsRollout
					RemoveRollout MagnetsRollout
					RemoveRollout SettingsRollout
					RemoveRollout LightRollout
					RemoveRollout GunRollout
					RemoveRollout AIRollout
					RemoveRollout FadeMusicRollout
					RemoveRollout SkyRollout
					RemoveRollout SkyActionsRollout
					RemoveRollout TeleporterRollout
				)

			-- Settings
			if s == 9 do
				(
					AddRollout SettingsRollout rolledup:true
					RemoveRollout TerrainTextureRollout
					RemoveRollout RenderingRollout
					RemoveRollout PhysicsRollout
					removeRollout MagnetsRollout
					RemoveRollout TriggersRollout
					RemoveRollout SoundRollout
					RemoveRollout LightRollout
					RemoveRollout GunRollout
					RemoveRollout AIRollout
					RemoveRollout FadeMusicRollout
					RemoveRollout SkyRollout
					RemoveRollout SkyActionsRollout
					RemoveRollout TeleporterRollout
				)
				
			-- Water Disturbances
			if s == 11 do
				(
				
					AddRollout PhysicsRollout rolledUp:true
					RemoveRollout TerrainTextureRollout
					RemoveRollout RenderingRollout
					RemoveRollout MagnetsRollout
					RemoveRollout TriggersRollout
					RemoveRollout SettingsRollout
					RemoveRollout SoundRollout
					RemoveRollout LightRollout
					RemoveRollout GunRollout
					RemoveRollout AIRollout
					RemoveRollout FadeMusicRollout
					RemoveRollout SkyRollout
					RemoveRollout SkyActionsRollout
					RemoveRollout TeleporterRollout
				)

			-- Light
			if s == 12 do
				(
					AddRollout LightRollout rolledUp:true
					RemoveRollout TerrainTextureRollout
					RemoveRollout RenderingRollout
					RemoveRollout PhysicsRollout
					RemoveRollout MagnetsRollout
					RemoveRollout TriggersRollout
					RemoveRollout SettingsRollout
					RemoveRollout SoundRollout
					RemoveRollout GunRollout
					RemoveRollout AIRollout
					RemoveRollout FadeMusicRollout
					RemoveRollout SkyRollout
					RemoveRollout SkyActionsRollout
					RemoveRollout TeleporterRollout
				)

			-- Sky
			if s == 13 do
				(
					AddRollout SkyRollout
					RemoveRollout GunRollout
					RemoveRollout TerrainTextureRollout
					RemoveRollout RenderingRollout
					RemoveRollout PhysicsRollout
					RemoveRollout MagnetsRollout
					RemoveRollout TriggersRollout
					RemoveRollout SettingsRollout
					RemoveRollout SoundRollout
					RemoveRollout LightRollout
					RemoveRollout AIRollout
					RemoveRollout FadeMusicRollout
					RemoveRollout SkyActionsRollout
					RemoveRollout TeleporterRollout
				)

			-- Gun
			if s == 14 do
				(
					AddRollout GunRollout rolledUp:true
					RemoveRollout TerrainTextureRollout
					RemoveRollout RenderingRollout
					RemoveRollout PhysicsRollout
					RemoveRollout MagnetsRollout
					RemoveRollout TriggersRollout
					RemoveRollout SettingsRollout
					RemoveRollout SoundRollout
					RemoveRollout LightRollout
					RemoveRollout AIRollout
					RemoveRollout FadeMusicRollout
					RemoveRollout SkyRollout
					RemoveRollout SkyActionsRollout
					RemoveRollout TeleporterRollout
				)

			-- Teleporter
			if s == 17 do
				(
					RemoveRollout GunRollout
					RemoveRollout TerrainTextureRollout
					RemoveRollout RenderingRollout
					RemoveRollout PhysicsRollout
					RemoveRollout MagnetsRollout
					RemoveRollout TriggersRollout
					RemoveRollout SettingsRollout
					RemoveRollout SoundRollout
					RemoveRollout LightRollout
					RemoveRollout AIRollout
					RemoveRollout FadeMusicRollout
					RemoveRollout SkyRollout
					RemoveRollout SkyActionsRollout
					AddRollout TeleporterRollout rolledUp:true
				)
		)

-- Write out properties
on WriteObjectProps pressed do
	(
		local		ObjTypeString = "", ObjParamsString = "", ObjParamsString2 = "", ObjPhysicsString = "", PhysicsTangibleString = "",
						PhysicsMoveableString = "", PhysicsDensityString = "", PhysicsFrictionString = "", PhysicsElasticityString = "", PhysicsSoundMaterialString = "",
						PhysicsBoundingBoxString = "", AnimTexString = "", AnimTexRateString = "", SurfaceBumpinessString = "", SurfaceBumpMapsString = "", SurfaceVisibleString = "",
						SurfaceCastsShadowsString = "", SurfaceCurvatureString = "", SurfaceDiffuseString = "", SurfaceSpecularString = "", SurfaceSpecularAngleString = "",
						SurfaceRefractIndexString = "", SurfaceEmissiveString = "", SurfaceReflectString = "", SurfaceRefractString = "", SurfaceCullingString = "",
						DeltaXString = "", DeltaYString = "", DROString = ""
						
		local		SurfacePlantedString = "", SurfacePlantedHeightString = "", WrapString = "", MergeString = "",
						SplitString = "", SurfaceMipMapString = "", SurfaceCurvedString = "", DetailAxisString = "", SurfaceShadowCullingString = "", SurfaceCacheIntersectingString = "",
						SurfaceAlwaysFaceString = "", SurfaceNoCacheString = "", SurfaceNoCacheAloneString = "", SurfacePriorityString = ""
		
		-- Process Object Type
			-- CInstance
			if GeneralObjectType.selection == 1 do
				(
					ObjTypeString = "string Class = \"CInstance\";\r\r\n"
					if RenderingRollout.IgnorePhysics.checked == false do
						(
							-- Determine Physics Properties
								IgnoreDensity = false
								if PhysicsRollout.PhysicsPropsTangible.checked == true do
									(
										PhysicsTangibleString = "bool Tangible = true;\r\r\n"
										if PhysicsRollout.PhysicsPropsMoveable.checked == false do
											(
												IgnoreDensity = true
												PhysicsDensityString = "float Density = 10.0;\r\r\n"
											)
									)
			
								if PhysicsRollout.PhysicsPropsMoveable.checked == true do
									PhysicsMoveableString = "bool Moveable = true;\r\r\n"
			
								if IgnoreDensity == false do
									if PhysicsRollout.PhysicsPropsDensity.value != 0.1 do
										PhysicsDensityString = ("float Density = " + (FloatAsString PhysicsRollout.PhysicsPropsDensity.value) + ";\r\r\n")
			
								if PhysicsRollout.PhysicsPropsFriction.value != 5.0 do
									PhysicsFrictionString = ("float Friction = " + (FloatAsString PhysicsRollout.PhysicsPropsFriction.value) + ";\r\r\n")
		
								if PhysicsRollout.PhysicsPropsElasticity.value != 0.2 do
									PhysicsElasticityString = ("float Elasticity = " + (FloatAsString PhysicsRollout.PhysicsPropsElasticity.value) + ";\r\r\n")
		
								if PhysicsRollout.SoundPropertiesList.selection != 1 do (PhysicsSoundMaterialString = ("string SoundMaterial = \"" + MaterialTypesArray[CIndex] + "\";\r\r\n"))
		
								if (PhysicsBoxes = PhysicsRollout.PhysicsBoundingBoxList.items).count != 0 do
									(
										PhysicsBoundingBoxString = "string Type = \"Compound\";\r\r\n"
										for i = 1 to PhysicsBoxes.count do
											(
												if i < 11 then
													iString = ("0" + (i - 1) as string)
												else
													iString = (i - 1) as string
													
												PhysicsBoundingBoxString = (PhysicsBoundingBoxString + "string Model" + iString + " = \"" + PhysicsBoxes[i] + "\";\r\r\n")
											)
									)
							)

						if PhysicsRollout.IgnoreRendering.checked == false do
							(
								-- Determine Rendering Properties
								if RenderingRollout.SurfaceBumpiness.value != 0.025 do
									SurfaceBumpinessString = ("float Bumpiness = " + (FloatAsString RenderingRollout.SurfaceBumpiness.value) + ";\r\r\n")
		
								if RenderingRollout.SurfaceCurved.checked do
									SurfaceCurvedString = "bool curved = true;\r\r\n"
									
								if RenderingRollout.SurfaceBumpMaps.checked != true do
									SurfaceBumpMapsString = "bool Bumpmaps = false;\r\r\n"
		
								if RenderingRollout.SurfaceMipMap.checked == false do
									SurfaceMipMapString = "bool MipMap = false;\r\r\n"

								if RenderingRollout.SurfaceCacheIntersecting.checked do
									SurfaceCacheIntersectingString = "bool CacheIntersecting = true;\r\r\n"

								if RenderingRollout.SurfaceVisible.checked == false do
									SurfaceVisibleString = ("bool Visible = false;\r\r\n")
		
								if RenderingRollout.SurfaceCastsShadows.checked == false do
									SurfaceCastsShadowsString = ("bool Shadow = false;\r\r\n")
		
								if RenderingRollout.SurfacePlanted.checked do
									(
										SurfacePlantedString = "bool Planted = true;\r\r\n"
										SurfacePlantedHeightString = ("float Height = " + (FloatAsString RenderingRollout.SurfacePlantedHeight.value) + ";\r\r\n")
									)

								if RenderingRollout.CullDistance.value != 0.0 do
									SurfaceCullingString = ("float Culling = " + (FloatAsString RenderingRollout.CullDistance.value) + ";\r\r\n")

								if RenderingRollout.ShadowCullDistance.value != 0.0 do
									SurfaceShadowCullingString = ("float ShadowCulling = " + (FloatAsString RenderingRollout.ShadowCullDistance.value) + ";\r\r\n")

								if RenderingRollout.SurfacePriority.value != 0 do
									SurfacePriorityString = ("int Priority = " + ((RenderingRollout.SurfacePriority.value as integer) as string) + ";\r\r\n")

								if RenderingRollout.SurfaceAlwaysFace.checked do
									SurfaceAlwaysFaceString = "bool AlwaysFace = true;\r\r\n"

								if RenderingRollout.SurfaceNoCache.checked do
									SurfaceNoCacheString = "bool NoCache = true;\r\r\n"

								if RenderingRollout.SurfaceNoCacheAlone.checked do
									SurfaceNoCacheAloneString = "bool NoCacheAlone = true;\r\r\n"

								if RenderingRollout.Wrap.checked == false do
									WrapString = "bool Wrap = false;\r\r\n"

								if RenderingRollout.Merge.checked == false do
									MergeString = "bool Merge = false;\r\r\n"

								if RenderingRollout.Split.checked do
									SplitString = "bool Split = true;\r\r\n"

								if RenderingRollout.UseAnimatedTextures.checked and (IFL.count > 0) do
									(
										AnimTexString = ""
										for i = 1 to IFL.count do
											(
												iString = (i - 1) as string
												if i <= 10 do
													iString = ("0" + iString)
												AnimTexString = (AnimTexString + ("string Anim" + istring + " = \"" + IFL[i] + "\";\r\r\n"))
											)
										AnimTexRateString = ("float Interval = " + (FloatAsString (1 / RenderingRollout.fps.value)) + ";\r\r\n")
									)
								
									if RenderingRollout.DeltaX.value != 0.0 do
										DeltaXString = ("float DeltaX = " + (FloatAsString RenderingRollout.DeltaX.value) + ";\r\r\n")
		
									if RenderingRollout.DeltaY.value != 0.0 do
										DeltaYString = ("float DeltaY = " + (FloatAsString RenderingRollout.DeltaX.value) + ";\r\r\n")
		
									-- Advanced Rendering Parameters
									if RenderingRollout.MaterialDiffuse.value != 1.0 do
										SurfaceDiffuseString = ("float Diffuse = " + (FloatAsString RenderingRollout.MaterialDiffuse.value) + ";\r\r\n")
		
									if RenderingRollout.MaterialSpecular.value != 0 do
										SurfaceSpecularString = ("float Specular = " + (FloatAsString RenderingRollout.MaterialSpecular.value) + ";\r\r\n")
									
									if RenderingRollout.MaterialSpecularAngle.value != 1.0 do
										SurfaceSpecularAngleString = ("float SpecularAngle = " + (FloatAsString RenderingRollout.MaterialSpecularAngle.value) + ";\r\r\n")
									
									if RenderingRollout.MaterialRefractIndex.value != 1.0 do
										SurfaceRefractIndexString = ("float RefractIndex = " + (FloatAsString RenderingRollout.MaterialRefractIndex.value) + ";\r\r\n")
									
									if RenderingRollout.MaterialEmissive.value != 0 do
										SurfaceEmissiveString = ("float Emissive = " + (FloatAsString RenderingRollout.MaterialEmissive.value) + ";\r\r\n")
		
									if RenderingRollout.MaterialReflect.checked == true do
										SurfaceReflectString = "bool Reflect = true;\r\r\n"
			
									if RenderingRollout.MaterialRefract.checked == true do
										SurfaceRefractString = "bool refract = true;\r\r\n"
		
								if RenderingRollout.SurfaceCurvatureFlags.selection != 4 do
									(
										if RenderingRollout.SurfaceCurvatureFlags.selection == 1 do
											SurfaceCurvatureString = ("int Normals = 0;\r\r\n")
				
										if RenderingRollout.SurfaceCurvatureFlags.selection == 2 do
											SurfaceCurvatureString = ("int Normals = 1;\r\r\n")
				
										if RenderingRollout.SurfaceCurvatureFlags.selection == 3 do
											SurfaceCurvatureString = ("int Normals = 2;\r\r\n")
									)
		
								if (CurrentDDLObjectArray = RenderingRollout.DetailReductionObjectList.items).count != 0 do
									(
											-- determine the local axis of detail reduction
											if RenderingRollout.DetailAxis.state != 3 do
												(
													if RenderingRollout.DetailAxis.state == 1 then
														DetailAxisString = "bool X = true;\r\r\n"
													else
														DetailAxisString = "bool Y = true;\r\r\n"
												)
											for i = 1 to CurrentDDLObjectArray.count do
												(
													DROString = (DROString + ("string Detail" + i as string + " = \"" + CurrentDDLObjectArray[i] + "\";\r\r\n"))
													DROString = (DROString + ("float A0" + i as string + " = " + FloatAsString (DRData[i] as float) + ";\r\r\n"))
												)
									)
							)
						ObjParamsString = (PhysicsTangibleString + PhysicsMoveableString + PhysicsDensityString + PhysicsFrictionString + PhysicsElasticityString + PhysicsSoundMaterialString + PhysicsBoundingBoxString)
						local tempString1 = (SurfaceBumpinessString + SurfaceCurvedString + SurfaceBumpMapsString + SurfaceMipMapString + SurfaceCacheIntersectingString + SurfaceVisibleString + SurfaceCastsShadowsString + SurfacePlantedString  + SurfaceCullingString + SurfaceShadowCullingString + SurfaceAlwaysFaceString)
						local tempString2 = (SurfacePriorityString + SurfaceNoCacheString + SurfaceNoCacheAloneString + WrapString + MergeString + SplitString + SurfacePlantedHeightString + AnimTexString + AnimTexRateString + DeltaXString + DeltaYString + SurfaceCurvatureString + SurfaceDiffuseString + SurfaceSpecularString + SurfaceSpecularAngleString + SurfaceRefractIndexString + SurfaceEmissiveString + SurfaceReflectString + SurfaceRefractString + DROString + DetailAxisString)
						ObjParamsString2 = (tempString1 + tempString2)
				)
			-- Player
			if GeneralObjectType.selection == 2 do
				ObjTypeString = "string Class = \"Player\";\r\r\n"
			
			-- Terrain Texture Objects
			if GeneralObjectType.selection == 3 do
				(
					TIndex = TerraintextureRollout.SoundPropertiesList.selection
					TerrainSoundMaterialString = ""
					ObjTypeString = "string Class = \"CTerrainObj\";\r\r\n"

					if TerraintextureRollout.SoundPropertiesList.selection != 1 do 
						TerrainSoundMaterialString = ("string SoundMaterial = \"" + TerrainMaterialTypesArray[TIndex] + "\";\r\r\n")

					ObjParamsString = ("int Height = " + TerraintextureRollout.TextureLevel.value as string + ";\r\r\n" + TerrainSoundMaterialString)
				)
			
			-- Terrain Extent Object
			if GeneralObjectType.selection == 4 do
				ObjTypeString = "string Class = \"TerrainExtent\";\r\r\n"
		
			-- Terrain Placement Object
			if GeneralObjectType.selection == 5 do
				ObjTypeString = "string Class = \"TerrainPlacement\";\r\r\n"
			
			-- Animal
			if GeneralObjectType.selection == 6 do
				ObjTypeString = "string Class = \"CAnimal\";\r\r\n"
			
			-- Magnet
			if GeneralObjectType.selection == 7 do
				(
					MagnetTypeString = ""
					MagnetBreakableString = ""
					MagnetBreakStrengthString = ""
					MagnetDOFxString = ""
					MagnetDOFyString = ""
					MagnetDOFzString = ""
					MasterObjectString = ""
					SlaveObjectString = ""
					MagnetInstanceString = ""

					ObjTypeString = "string Class = \"CMagnet\";\r\r\n"
					-- Magnet Properties
					if MagnetsRollout.MagnetType.state != 1 do
						if MagnetsRollout.MagnetType.state == 2 then
							MagnetTypeString = "bool HandPickup = true;\r\r\n"
						else
							MagnetTypeString = "bool ShoulderHold = true;\r\r\n"

					if MagnetsRollout.MagnetBreakable.checked == true do
						(
							MagnetBreakableString = "bool Breakable = true;\r\r\n"
							if MagnetsRollout.MagnetBreakStrength.value != 0 do
								MagnetBreakStrengthString = ("float BreakStrength = " + (FloatAsString MagnetsRollout.MagnetBreakStrength.value) + ";\r\r\n")
						)

					if MagnetsRollout.MagnetInstance.checked do
						MagnetInstanceString = "bool Instance = true;\r\r\n"

					if MagnetsRollout.MagnetDOF_X.checked == true do
						MagnetDOFxString = "bool XFree = true;\r\r\n"

					if MagnetsRollout.MagnetDOF_Y.checked == true do
						MagnetDOFyString = "bool YFree = true;\r\r\n"
						
					if MagnetsRollout.MagnetDOF_Z.checked == true do
						MagnetDOFzString = "bool ZFree = true;\r\r\n"
					
					if MagnetsRollout.GetMasterObject.text != "" do
						MasterObjectString = ("string MasterObject = \"" + MagnetsRollout.GetMasterObject.text + "\";\r\r\n")

					if MagnetsRollout.GetSlaveObject.text != "" do
						SlaveObjectString = ("string SlaveObject = \"" + MagnetsRollout.GetSlaveObject.text + "\";\r\r\n")
						
					ObjParamsString = (MagnetTypeString + MagnetBreakableString + MagnetBreakStrengthString + MagnetInstanceString + MagnetDOFxString + MagnetDOFyString + MagnetDOFzString + MasterObjectString + SlaveObjectString)
				)
			
			-- Location Trigger
			if GeneralObjectType.selection == 8 do
				(
					exitcode = false
					if TriggersRollout.ObjectInTrigger.state or TriggersRollout.ObjectEnterTrigger.state or TriggersRollout.ObjectLeaveTrigger.state do
						if ActiveObject == "" do
							(
								MessageBox "No Active Object defined!!!\nYou must define an Active Object when using Object Trigger types!"
								exitcode = true
							)
					if exitcode == false do
						(
							-- default trigger settings 
							local		TriggerDefaultsString = "bool Wrap = false;\r\r\nbool Merge = false;\r\r\nbool Split = false;\r\r\n"

							local 	SoundPropsString = "", FireDelayString = "", FireCountString = "", PlayerInTriggerString = "", PlayerEnterTriggerString = "",
											PlayerLeaveTriggerString = "", CreatureInTriggerString = "", CreatureEnterTriggerString = "", CreatureLeaveTriggerString = "",
											ObjectInTriggerString = "", ObjectEnterTriggerString = "", ObjectLeaveTriggerString = "", CreatureEnterCountString = "",
											CreatureLeaveCountString = "", TriggerActionTypeString = "", TriggerProbabilityString = "", TriggerIdentifierString = "",
											ActiveObjectString = "", TriggerExpressionString = "", SpatialTypeString = "", VolumeString = "", AttenuationString = "",
											FrustumAngleString = "", OutsideVolumeString = "", SoundPropsString = "", LoopedString = "", MaxDistanceString = "",
											TriggerBoundingVolumeString = "", TriggerAlphaChannelString = "", MaxVolDistanceString = "", LoopCountString = ""
							
							-- AI Action strings
							local 	AIActionsString = "", AIFearString = "", AILoveString = "", AIAngerString = "", AIHungerString = "",
											AIThirstString = "", AIFatigueString = "", AIPainString = "", AISolidityString = ""
							
							-- Fade Music Action
							local 	VolFaderString = "", FadeMusicString = ""
							
							-- Fog Actions
							local 	FogColorString = "", FogTypeString = "", FogPowerString = "", FogHalfString = "", FogActionsString = ""
							
							-- Renderer Actions
							local 	AmbientLightString = "", FarClipPlaneString = "", NearClipPlaneString = "", RendererActionsString = "",CullMaxDistString = "",
											CullMaxRadiusString = "", CullMaxDistShadowString = "", CullMaxRadiusShadowString = ""
							
							-- Terrain Actions
							local 	PixelTolString = "", PixelTolRelaxString = "", EvalDelayFactorString = "", TexelsperMeterString = "",
											TexelsperPixelString = "", DistanceMultiplyString = "", MinNodeSizeString = "", MaxSizeTextureString = "",
											MaxSizeShadowString = "", MaxSizeMvShadowString = "", TextureLifetimeString = "", DynamicString = "",
											TextureStaticString = "", PerTriangleString = "", AdjustForOrientationString = "", AdjustForDistanceString = "",
											PurgeDistanceString = "", TerrainActionsString = ""
							
							-- Physics Actions
							local 	PhysicsTargetString = "", XVelString = "", YVelString = "", ZVelString = "", PhysicsActionsString = ""
							

							-- Depth Sort Actions
							local 	NearTolString = "", FarTolString = "", NearZString = "", FarZString = "", FarZNoString = "", SortPixTolString = "",
											MaxNumToSortString = "", Sort2PartString = "", Sort4PartString = "", TerrainNearTolString = "", TerrainFarTolString = "",
											TerrainNearZString = "", TerrainFarZString = "", SeperateTolString = "", DepthSortActionsString = ""

							-- Sky Actions
							local 	SkyActionHeightString = "", SkyActionScaleString = "", SkyActionNearFogBandString = "", SkyActionNearFogBandString = "",
											SkyActionFarFogBandString = "", SkyActionNearFogBandString = "", SkyActionWindSpeedXString = "", SkyActionWindSpeedYString = "",
											SkyActionSubdivisionString = "", SkyActionTextureString = "", SkyActionFillScreenString = "", SkyActionsString = ""


							ObjTypeString = "string Class = \"CLocationTrigger\";\r\r\n"
							-- Trigger Parameters
							
							if TriggersRollout.TriggerBoundingVolume.selection != 1 do
								(
									sel = TriggersRollout.TriggerBoundingVolume.selection
									TriggerBoundingVolumeString = ("int BoundVol = " + ((sel - 1) as integer) as string + ";\r\r\n")
								)
							
							if TriggersRollout.TriggerFireDelay.value != 0 do
								FireDelayString = ("float FireDelay = " + (FloatAsString TriggersRollout.TriggerFireDelay.value) + ";\r\r\n")
		
							if TriggersRollout.TriggerFireInfinite.checked then
								FireCountString = "int FireCount = -1;\r\r\n"
							else
								FireCountString = ("int FireCount = " + TriggersRollout.TriggerFireCount.value as string + ";\r\r\n")

							if TriggersRollout.TriggerProbability.value != 100 do
								TriggerProbabilityString = ("int Probability = " + TriggersRollout.TriggerProbability.value as string + ";\r\r\n")
							
							-- Trigger Expressions
							if (TriggerExpression = TriggersRollout.Expression.text).count != 0 do
								TriggerExpressionString = "string FireExpression = \"" + TriggerExpression + "\";\r\r\n"

							TriggerAlphaChannelString = ("int AlphaChannel = " + (TriggersRollout.TriggerAlphaChannel.value as integer) as string + ";\r\r\n")

							-- Trigger Actions
								-- Voice Over
								if TriggersRollout.TriggerActionTypeList.selection == 1 do
									(
										TriggerActionTypeString = ("int ActionType = 0;\r\r\n")
										if TriggersRollout.Identifier.selection != 1 do
											TriggerIdentifierString = ("string Sample = \"" + TriggersRollout.Identifier.items[TriggersRollout.Identifier.selection] + "\";\r\r\n")

										if SoundRollout.SpatialType.selection != 1 do
											SpatialTypeString = ("int SpatialType = " + ((SoundRollout.SpatialType.selection - 1) as integer) as string + ";\r\r\n")

										if SoundRollout.Volume.value != 0.0 do
											VolumeString = ("float Volume = " + (FloatAsString SoundRollout.Volume.value) + ";\r\r\n")

										if SoundRollout.Attenuation.value != 1.0 do
											AttenuationString = ("float Attenuation = " + (FloatAsString SoundRollout.Attenuation.value) + ";\r\r\n")

										if SoundRollout.FrustumAngle.value != 360.0 do
											FrustumAngleString = ("float FrustumAngle = " + (FloatAsString SoundRollout.FrustumAngle.value) + ";\r\r\n")

										if SoundRollout.OutsideVolume.value != -15.0 do
											OutsideVolumeString = ("float OutsideVolume = " + (FloatAsString SoundRollout.OutsideVolume.value) + ";\r\r\n")
										
										SoundPropsString = (SpatialTypeString + VolumeString + AttenuationString + FrustumAngleString + OutsideVolumeString + LoopedString + MaxDistanceString)
									)

								-- Ambient
								if TriggersRollout.TriggerActionTypeList.selection == 2 do
									(
										TriggerActionTypeString = ("int ActionType = 1;\r\r\n")

										if TriggersRollout.Identifier.selection != 1 do
											TriggerIdentifierString = ("string Sample = \"" + TriggersRollout.Identifier.items[TriggersRollout.Identifier.selection] + "\";\r\r\n")

										if SoundRollout.SpatialType.selection != 1 do
											SpatialTypeString = ("int SpatialType = " + ((SoundRollout.SpatialType.selection - 1) as integer) as string + ";\r\r\n")

										if SoundRollout.Volume.value != 0.0 do
											VolumeString = ("float Volume = " + (FloatAsString SoundRollout.Volume.value) + ";\r\r\n")
										
										if SoundRollout.Attenuation.value != 1.0 do
											AttenuationString = ("float Attenuation = " + (FloatAsString SoundRollout.Attenuation.value) + ";\r\r\n")
										
										if SoundRollout.FrustumAngle.value != 360.0 do
											FrustumAngleString = ("float FrustumAngle = " + (FloatAsString SoundRollout.FrustumAngle.value) + ";\r\r\n")

										if SoundRollout.OutsideVolume.value != -15.0 do
											OutsideVolumeString = ("float OutsideVolume = " + (FloatAsString SoundRollout.OutsideVolume.value) + ";\r\r\n")
										
										if SoundRollout.MaxVolDistance.value != 0 do
											MaxVolDistanceString = ("float MaxVolDistance = " + (floatAsString SoundRollout.MaxVolDistance.value) + ";\r\r\n")

										if SoundRollout.looped.checked == true do
											(
												LoopedString = "bool Looped = true;\r\r\n"
												if SoundRollout.LoopCount.value != 0 do
													LoopCountString = ("int LoopCount = " + ((SoundRollout.LoopCount.value as integer) as string) + ";\r\r\n")
											)
										
										SoundPropsString = (SpatialTypeString + VolumeString + AttenuationString + FrustumAngleString + OutsideVolumeString + LoopedString + LoopCountString + MaxDistanceString + MaxVolDistanceString)
									)

								-- Music
								if TriggersRollout.TriggerActionTypeList.selection == 3 do
									(
										TriggerActionTypeString = ("int ActionType = 2;\r\r\n")

										if TriggersRollout.Identifier.selection != 1 do
											TriggerIdentifierString = ("string Sample = \"" + TriggersRollout.Identifier.items[TriggersRollout.Identifier.selection] + "\";\r\r\n")

										SpatialTypeString = ("int SpatialType = 0;\r\r\n")

										if SoundRollout.Volume.value != 0.0 do
											VolumeString = ("float Volume = " + (FloatAsString SoundRollout.Volume.value) + ";\r\r\n")
										
										if SoundRollout.Attenuation.value != 1.0 do
											AttenuationString = ("float Attenuation = " + (FloatAsString SoundRollout.Attenuation.value) + ";\r\r\n")
										
										if SoundRollout.looped.checked == true do
											(
												LoopedString = "bool Looped = true;\r\r\n"
												if SoundRollout.LoopCount.value != 0 do
													LoopCountString = ("int LoopCount = " + ((SoundRollout.LoopCount.value as integer) as string) + ";\r\r\n")
											)
										
										if SoundRollout.UseSizeOfEmitter.checked == false do
											MaxDistanceString = ("float MaxDistance = " + (FloatAsString SoundRollout.MaxDistance.value) + ";\r\r\n")
										
										SoundPropsString = (SpatialTypeString + VolumeString + AttenuationString + FrustumAngleString + OutsideVolumeString + LoopedString + LoopCountString + MaxDistanceString)
									)

								-- Fade Music Action
								if TriggersRollout.TriggerActionTypeList.selection == 4 do
									(
										TriggerActionTypeString = ("int ActionType = 3;\r\r\n")
										VolFaderString = ("float VolumeFader = " + (FloatAsString FadeMusicRollout.VolFader.value) + ";\r\r\n")
										FadeMusicString = VolFaderString
									)

								-- Overlay Actions (tbd)

								-- Fog Actions
								if TriggersRollout.TriggerActionTypeList.selection == 6 do
									(
										TriggerActionTypeString = ("int ActionType = 5;\r\r\n")
										
										if FogRollout.UseFogColor.checked do
											(
												fc = FogRollout.FogColor.color
												r = fc.r
												g = fc.g
												b = fc.b
												FogColorString = ("int R = " + (r as integer) as string + ";\r\r\nint G = " + (g as integer) as string + ";\r\r\nint B = " + (b as integer) as string + ";\r\r\n")
											)

										if FogRollout.UseFogType.checked do
											if FogRollout.FogType.state == 1 then
												FogTypeString = ("int FogType = 0;\r\r\n")
											else
												FogTypeString = ("int FogType = 1;\r\r\n")
										
										if FogRollout.UseFogPower.checked do
											FogPowerString = ("float FogPower = " + (FloatAsString FogRollout.FogPower.value) + ";\r\r\n")

										if FogRollout.UseFogHalf.checked do
											FogHalfString = ("float FogHalf = " + (FloatAsString FogRollout.FogHalf.value) + ";\r\r\n")
										
										FogActionsString = (FogColorString + FogTypeString + FogPowerString + FogHalfString)
									)

								-- Renderer Actions
								if TriggersRollout.TriggerActionTypeList.selection == 7 do
									(
										TriggerActionTypeString = ("int ActionType = 6;\r\r\n")
										
										if RendererRollout.UseAmbientLight.checked do
											AmbientLightString = ("float AmbientLight = " + (FloatAsString RendererRollout.AmbientLight.value) + ";\r\r\n")
										if RendererRollout.UseFarClip.checked do
											FarClipPlaneString = ("float FarClipPlane = " + (FloatAsString RendererRollout.FarClip.value) + ";\r\r\n")
										if RendererRollout.UseNearClip.checked do
											NearClipPlaneString = ("float NearClipPlane = " + (FloatAsString RendererRollout.NearClip.value) + ";\r\r\n")
										if RendererRollout.UseCullMaxDist.checked do
											CullMaxDistString = ("float CullMaxDist = " + (FloatAsString RendererRollout.CullMaxDist.value) + ";\r\r\n")
										if RendererRollout.UseCullMaxRadius.checked do
											CullMaxRadiusString = ("float CullMaxRadius = " + (FloatAsString RendererRollout.CullMaxRadius.value) + ";\r\r\n")
										if RendererRollout.UseCullMaxDistShadow.checked do
											CullMaxDistShadowString = ("float CullMaxDistShadow = " + (FloatAsString RendererRollout.CullMaxDistShadow.value) + ";\r\r\n")
										if RendererRollout.UseCullMaxRadiusShadow.checked do
											CullMaxRadiusShadowString = ("float CullMaxRadiusShadow = " + (FloatAsString RendererRollout.CullMaxRadiusShadow.value) + ";\r\r\n")
										
										RendererActionsString = (AmbientLightString + FarClipPlaneString + NearClipPlaneString + CullMaxDistString + CullMaxRadiusString + CullMaxDistShadowString + CullMaxRadiusShadowString)
									)

								-- Terrain Actions
								if TriggersRollout.TriggerActionTypeList.selection == 8 do
									(
										TriggerActionTypeString = ("int ActionType = 7;\r\r\n")

										if TerrainActionsRollout.UsePixelTol.checked do
											PixelTolString = ("float TrrPixelTol = " + (FloatAsString TerrainActionsRollout.PixelTol.value) + ";\r\r\n")

										if TerrainActionsRollout.UsePixelTolRelax.checked do
											PixelTolRelaxString = ("float TrrPixelTolFar = " + (FloatAsString TerrainActionsRollout.PixelTolRelax.value) + ";\r\r\n")

										if TerrainActionsRollout.UseEvalDelayFactor.checked do
											EvalDelayFactorString = ("float TrrEvalDelayFactor = " + (FloatAsString TerrainActionsRollout.EvalDelayFactor.value) + ";\r\r\n")

										if TerrainActionsRollout.UseTexelsPerMeter.checked do
											TexelsperMeterString = ("float TexelsPerMeter = " + (FloatAsString TerrainActionsRollout.TexelsPerMeter.value) + ";\r\r\n")

										if TerrainActionsRollout.UseTexelsPerPixel.checked do
											TexelsperPixelString = ("float TexelsPerPixel = " + (FloatAsString TerrainActionsRollout.TexelsPerPixel.value) + ";\r\r\n")

										if TerrainActionsRollout.UseDistanceMultiply.checked do
											DistanceMultiplyString = ("float DistanceMultiply = " + (FloatAsString TerrainActionsRollout.DistanceMultiply.value) + ";\r\r\n")

										if TerrainActionsRollout.UseMinNodeSize.checked do
											MinNodeSizeString = ("float MinNodeSize = " + (FloatAsString TerrainActionsRollout.MinNodeSize.value) + ";\r\r\n")

										if TerrainActionsRollout.UseMaxSizeTexture.checked do
											MaxSizeTextureString = ("float MaxSizeTexture = " + (FloatAsString TerrainActionsRollout.MaxSizeTexture.value) + ";\r\r\n")

										if TerrainActionsRollout.UseMaxSizeShadow.checked do
											MaxSizeShadowString = ("float MaxSizeShadow = " + (FloatAsString TerrainActionsRollout.MaxSizeShadow.value) + ";\r\r\n")

										if TerrainActionsRollout.UseMaxSizeMvShadow.checked do
											MaxSizeMvShadowString = ("float MaxSizeMvShadow = " + (FloatAsString TerrainActionsRollout.MaxSizeMvShadow.value) + ";\r\r\n")

										if TerrainActionsRollout.UseTextureLifetime.checked do
											TextureLifetimeString = ("int TextureLifetime = " + ((TerrainActionsRollout.TextureLifetime.value as integer) as string) + ";\r\r\n")

										if TerrainActionsRollout.UseDynamic.checked do
											DynamicString = ("bool Dynamic = " + (TerrainActionsRollout.Dynamic.checked as string) + ";\r\r\n")

										if TerrainActionsRollout.UseTextureStatic.checked do
											TextureStaticString = ("bool TextureStatic = " + (TerrainActionsRollout.TextureStatic.checked as string) + ";\r\r\n")

										if TerrainActionsRollout.UsePerTriangle.checked do
											PerTriangleString = ("bool PerTriangle = " + (TerrainActionsRollout.PerTriangle.checked as string) + ";\r\r\n")

										if TerrainActionsRollout.UseAdjustForOrientation.checked do
											AdjustForOrientationString = ("bool AdjustForOrient = " + (TerrainActionsRollout.AdjustForOrientation.checked as string) + ";\r\r\n")

										if TerrainActionsRollout.UseAdjustForDistance.checked do
											AdjustForDistanceString = ("bool AdjustForDist = " + (TerrainActionsRollout.AdjustForDistance.checked as string) + ";\r\r\n")

										if TerrainActionsRollout.UsePurgeDistance.checked do
											PurgeDistanceString = ("bool PurgeDistance = " + (TerrainActionsRollout.PurgeDistance.checked as string) + ";\r\r\n")

										TerrainActionsString = (PixelTolString + PixelTolRelaxString + EvalDelayFactorString + TexelsperMeterString + TexelsperPixelString + DistanceMultiplyString + MinNodeSizeString + MaxSizeTextureString + MaxSizeShadowString + MaxSizeMvShadowString + TextureLifetimeString + DynamicString + TextureStaticString + PerTriangleString + AdjustForOrientationString + AdjustForDistanceString + PurgeDistanceString)

									)

								-- AI Actions
								if TriggersRollout.TriggerActionTypeList.selection == 10 do
									(
										if AIRollout.AITarget.text != "" then
											(
												TriggerActionTypeString = ("int ActionType = 9;\r\r\n")
												AITargetString = ("string Target = \"" + AIRollout.AITarget.text + "\";\r\r\n")

												if AIRollout.UseAIFear.checked do
													AIFearString = ("float Fear = " + (FloatAsString AIRollout.AIFear.value) + ";\r\r\n")

												if AIRollout.UseAILove.checked do
													AILoveString = ("float Love = " + (FloatAsString AIRollout.AILove.value) + ";\r\r\n")

												if AIRollout.UseAIAnger.checked do
													AIAngerString = ("float Anger = " + (FloatAsString AIRollout.AIAnger.value) + ";\r\r\n")

												if AIRollout.UseAIHunger.checked do
													AIHungerString = ("float Hunger = " + (FloatAsString AIRollout.AIHunger.value) + ";\r\r\n")

												if AIRollout.UseAIThirst.checked do
													AIThirstString = ("float Thirst = " + (FloatAsString AIRollout.AIThirst.value) + ";\r\r\n")

												if AIRollout.UseAIPain.checked do
													AIPainString = ("float Pain = " + (FloatAsString AIRollout.AIPain.value) + ";\r\r\n")

												if AIRollout.UseAISolidity.checked do
													AISolidityString = ("float Solidity = " + (FloatAsString AIRollout.AISolidity.value) + ";\r\r\n")
												
												AIActionsString = (AITargetString + AIFearString + AILoveString + AIAngerString + AIThirstString + AIPainString + AISolidityString)
											) else (
												Messagebox "You must define a Target Object\nfor the AI action!"
											)
									)

								-- Physics Actions
								if TriggersRollout.TriggerActionTypeList.selection == 11 do
									(
										if PhysicsActionsRollout.PhysicsTarget.text != "" then
											(
												TriggerActionTypeString = ("int ActionType = 10;\r\r\n")
		
												PhysicsTargetString = ("string Target = \"" + PhysicsActionsRollout.PhysicsTarget.text + "\";\r\r\n")

												if PhysicsActionsRollout.UseXVel.checked do
													XVelString = ("float X = " + (FloatAsString PhysicsActionsRollout.XVel.value) + ";\r\r\n")
		
												if PhysicsActionsRollout.UseYVel.checked do
													YVelString = ("float Y = " + (FloatAsString PhysicsActionsRollout.YVel.value) + ";\r\r\n")
		
												if PhysicsActionsRollout.UseZVel.checked do
													ZVelString = ("float Z = " + (FloatAsString PhysicsActionsRollout.ZVel.value) + ";\r\r\n")
												
												PhysicsActionsString = (PhysicsTargetString + XVelString + YVelString + ZVelString)
											)
									)

								-- Depth Sort Actions
								if TriggersRollout.TriggerActionTypeList.selection == 13 do
									(
										TriggerActionTypeString = ("int ActionType = 12;\r\r\n")
										
										-- Object Tolerances
										if DepthSortRollout.UseNearTol.checked do
											NearTolString = ("float NearTolerance = " + (FloatAsString DepthSortRollout.NearTol.value) + ";\r\r\n")
										if DepthSortRollout.UseFarTol.checked do
											FarTolString = ("float FarTolerance = " + (FloatAsString DepthSortRollout.FarTol.value) + ";\r\r\n")
										if DepthSortRollout.UseNearZ.checked do
											NearZString = ("float NearZ = " + (FloatAsString DepthSortRollout.NearZ.value) + ";\r\r\n")
										if DepthSortRollout.UseFarZ.checked do
											FarZString = ("float FarZ = " + (FloatAsString DepthSortRollout.FarZ.value) + ";\r\r\n")
										if DepthSortRollout.UseFarZNo.checked do
											FarZNoString = ("float FarZNo = " + (FloatAsString DepthSortRollout.FarZNo.value) + ";\r\r\n")
										if DepthSortRollout.UseSortPixTol.checked do
											SortPixTolString = ("float SortPixelTol = " + (FloatAsString DepthSortRollout.SortPixTol.value) + ";\r\r\n")
										if DepthSortRollout.UseMaxNumToSort.checked do
											MaxNumToSortString = ("int MaxNumToSort = " + (DepthSortRollout.MaxNumToSort.value as string) + ";\r\r\n")
										if DepthSortRollout.UseSort2Part.checked do
											Sort2PartString = ("int Sort2PartAt = " + (DepthSortRollout.Sort2Part.value as string) + ";\r\r\n")
										if DepthSortRollout.UseSort4Part.checked do
											Sort4PartString = ("int Sort4PartAt = " + (DepthSortRollout.Sort4Part.value as string) + ";\r\r\n")

										-- Terrain Tolerances
										if DepthSortRollout.UseTerrainNearTol.checked do
											TerrainNearTolString = ("float TerrNearTolerance = " + (FloatAsString DepthSortRollout.TerrainNearTol.value) + ";\r\r\n")
										if DepthSortRollout.UseTerrainFarTol.checked do
											TerrainFarTolString = ("float TerrFarTolerance = " + (FloatAsString DepthSortRollout.TerrainFarTol.value) + ";\r\r\n")
										if DepthSortRollout.UseTerrainNearZ.checked do
											TerrainNearZString = ("float TerrNearZ = " + (FloatAsString DepthSortRollout.TerrainNearZ.value) + ";\r\r\n")
										if DepthSortRollout.UseTerrainFarZ.checked do
											TerrainFarZString = ("float TerrFarZ = " + (FloatAsString DepthSortRollout.TerrainFarZ.value) + ";\r\r\n")
										if DepthSortRollout.UseSeperateTol.checked do
											SeperateTolString = ("bool UseSeperateTol = " + (DepthSortRollout.SeperateTol.checked as string) + ";\r\r\n")
										
										DepthSortActionsString = (NearTolString + FarTolString + NearZString + FarZString + FarZNoString + SortPixTolString + MaxNumToSortString + Sort2PartString + Sort4PartString + TerrainNearTolString + TerrainFarTolString + TerrainNearZString + TerrainFarZString + SeperateTolString)

									)

								-- Sky Actions
								if TriggersRollout.TriggerActionTypeList.selection == 14 do
									(
										TriggerActionTypeString = ("int ActionType = 13;\r\r\n")

										if SkyActionsRollout.UseSkyActionHeight.checked do
											SkyActionHeightString = ("float Height = " + (floatAsString SkyActionsRollout.SkyActionHeight.value) + ";\r\r\n")
										if SkyActionsRollout.UseSkyActionScale.checked do
											SkyActionScaleString = ("float Scale = " + (floatAsString SkyActionsRollout.SkyActionScale.value) + ";\r\r\n")
										if SkyActionsRollout.UseSkyActionNearFogBand.checked do
											SkyActionNearFogBandString = ("float Scale = " + (floatAsString SkyActionsRollout.SkyActionNearFogBand.value) + ";\r\r\n")
										if SkyActionsRollout.UseSkyActionNearFogBand.checked do
											SkyActionNearFogBandString = ("float FogNear = " + (floatAsString SkyActionsRollout.SkyActionNearFogBand.value) + ";\r\r\n")
										if SkyActionsRollout.UseSkyActionFarFogBand.checked do
											SkyActionFarFogBandString = ("float FogFar = " + (floatAsString SkyActionsRollout.SkyActionFarFogBand.value) + ";\r\r\n")
										if SkyActionsRollout.UseSkyActionNearFogBand.checked do
											SkyActionNearFogBandString = ("float FogNear = " + (floatAsString SkyActionsRollout.SkyActionNearFogBand.value) + ";\r\r\n")
										if SkyActionsRollout.UseSkyActionWindSpeedX.checked do
											SkyActionWindSpeedXString = ("float WindSpeedX = " + (floatAsString SkyActionsRollout.SkyActionWindSpeedX.value) + ";\r\r\n")
										if SkyActionsRollout.UseSkyActionWindSpeedY.checked do
											SkyActionWindSpeedYString = ("float WindSpeedY = " + (floatAsString SkyActionsRollout.SkyActionWindSpeedY.value) + ";\r\r\n")
										if SkyActionsRollout.UseSkyActionSubdivision.checked do
											SkyActionSubdivisionString = ("int Subdivision = " + ((SkyActionsRollout.SkyActionSubdivision.value as integer) as string) + ";\r\r\n")
										if SkyActionsRollout.UseSkyActionTexture.checked do
											SkyActionTextureString = ("bool Texture = " + SkyActionsRollout.SkyActionTexture.checked as string + ";\r\r\n")
										if SkyActionsRollout.UseSkyActionFillScreen.checked do
											SkyActionFillScreenString = ("bool FillScreen = " + SkyActionsRollout.SkyActionFillScreen.checked as string + ";\r\r\n")

										SkyActionsString = (SkyActionHeightString + SkyActionScaleString + SkyActionNearFogBandString + SkyActionNearFogBandString + SkyActionFarFogBandString + SkyActionNearFogBandString + SkyActionWindSpeedXString + SkyActionWindSpeedYString + SkyActionSubdivisionString + SkyActionTextureString + SkyActionFillScreenString)
									)

							-- CLocationTrigger Parameters
								-- Player affects trigger
								if TriggersRollout.PlayerInTrigger.state == true do
									PlayerInTriggerString = "bool PlayerInTrigger = true;\r\r\n"
		
								if TriggersRollout.PlayerEnterTrigger.state == true do
									PlayerEnterTriggerString = "bool PlayerEnterTrigger = true;\r\r\n"
		
								if TriggersRollout.PlayerLeaveTrigger.state == true do
									PlayerLeaveTriggerString = "bool PlayerLeaveTrigger = true;\r\r\n"
		
								-- Creature Affects Trigger
								if TriggersRollout.CreatureInTrigger.state == true do
									CreatureInTriggerString = "bool CreatureInTrigger = true;\r\r\n"
								
								if TriggersRollout.CreatureEnterTrigger.state == true do
									CreatureEnterTriggerString = "bool CreatureEnterTrigger = true;\r\r\n"
		
								if TriggersRollout.CreatureLeaveTrigger.state == true do
									CreatureLeaveTriggerString = "bool CreatureLeaveTrigger = true;\r\r\n"
			
								-- Object Affects Trigger
								if TriggersRollout.ObjectInTrigger.state == true do
									ObjectInTriggerString = "bool ObjectInTrigger = true;\r\r\n"
								
								if TriggersRollout.ObjectEnterTrigger.state == true do
									ObjectEnterTriggerString = "bool ObjectEnterTrigger = true;\r\r\n"
								
								if TriggersRollout.ObjectLeaveTrigger.state == true do
									ObjectLeaveTriggerString = "bool ObjectLeaveTrigger = true;\r\r\n"
								
								if ActiveObject != "" do
									ActiveObjectString = "string TriggerActivate = \"" + ActiveObject + "\";\r\r\n"
								
								if TriggersRollout.CreatureEnterCount.value != 0 do
									CreatureEnterCountString = ("int CreatureEnterCount = " + TriggersRollout.CreatureEnterCount.value as string + ";\r\r\n")

								if TriggersRollout.CreatureLeaveCount.value != 0 do
									CreatureLeaveCountString = ("int CreatureLeaveCount = " + TriggersRollout.CreatureLeaveCount.value as string + ";\r\r\n")
		
							ObjParamsString = (TriggerBoundingVolumeString + FireDelayString + FireCountString + TriggerProbabilityString + TriggerAlphaChannelString + TriggerExpressionString + TriggerActionTypeString + TriggerIdentifierString + PlayerInTriggerString + PlayerEnterTriggerString + PlayerLeaveTriggerString + CreatureInTriggerString + CreatureEnterTriggerString + CreatureLeaveTriggerString + ObjectInTriggerString + ObjectEnterTriggerString + ObjectLeaveTriggerString + ActiveObjectString + CreatureEnterCountString + CreatureLeaveCountString)
							ObjParamsString2 = (TriggerDefaultsString + SoundPropsString + AIActionsString + FadeMusicString + FogActionsString + RendererActionsString + TerrainActionsString + PhysicsActionsString + DepthSortActionsString + SkyActionsString)
						)
				)

			-- Settings
			if GeneralObjectType.selection == 9 do
				(
					AlphaChannelString = ""
					AlphaColorString = ""
					AlphaString = ""

					ObjTypeString = "string Class = \"Settings\";\r\r\n"
					AlphaChannel =  (SettingsRollout.AlphaColorList.selection - 1) as string
					AlphaChannelString = ("int AlphaChannel = " + AlphaChannel + ";\r\r\n")
					AlphaColor = SettingsRollout.AlphaColor.color
					r = AlphaColor.r
					g = AlphaColor.g
					b = AlphaColor.b
					AlphaColorString = ("int R = " + r as string + ";\r\r\nint G = " + g as string + ";\r\r\nint B = " + b as string + ";\r\r\n") 
					AlphaString = ("float Alpha = " + (FloatAsString SettingsRollout.AlphaLevel.value) + ";\r\r\n")
					
					ObjParamsString = (AlphaChannelString + AlphaColorString + AlphaString)
				)
			
			-- Water Object
			if GeneralObjectType.selection == 10 do
				ObjTypeString = "string Class = \"CEntityWater\";"
	
			-- Water Disturbance Object
			if GeneralObjectType.selection == 11 do
				(
					ObjTypeString = "string Class = \"CWaterDisturbance\";\r\r\n"

					if PhysicsRollout.UseObjRadius.checked then
						WDRadius = 1.0
					else
						WDRadius = PhysicsRollout.WDRadius.value

					WDStrengthString = ("float Strength = " + (FloatAsString PhysicsRollout.WDStrength.value) + ";\r\r\n")
					WDIntervalString = ("float Interval = " + (FloatAsString PhysicsRollout.WDInterval.value) + ";\r\r\n")
					WDRadiusString = ("// float Radius = " + (FloatAsString WDRadius) + ";\r\r\n")

					ObjParamsString = (WDStrengthString + WDIntervalString + WDRadiusString )
				)

			-- Directional Lights
			if GeneralObjectType.selection == 12 do
				(
					LightIntensityString = ""
					
					ObjTypeString = "string Class = \"CLightDirectional\";\r\r\n"
					
					if LightRollout.Intensity.value != 1.0 do
						LightIntensityString = ("float Intensity = " + FloatAsString LightRollout.Intensity.value + ";\r\r\n")
					
					ObjParamsString = (LightIntensityString + "bool Visible = false;\r\r\n")
				)
			-- Sky object
			if GeneralObjectType.selection == 13 do
				(
					ObjTypeString = "string Class = \"CSky\";\r\r\n"
					SkyFlatColorIndexString = ("int SkyFlatColour = " + ((SkyRollout.SkyFlatColorIndex.value as integer) as string) + ";\r\r\n")
					ObjParamsString = ("#no_quant\r\r\nbool MipMap = false;\r\r\n" + SkyFlatColorIndexString)
				)

			-- Gun Object
			if GeneralObjectType.selection == 14 do
				(
					ROFString = ""
					DamageString = ""
					RangeString = ""
					PushString = ""
					BarrelString = ""
					MFlashOrgString = ""
					RecoilOrgString = ""
					RecoilForceString = ""
					MFlashObjectsString = ""
					FlashDurationString = ""
					RandomRotateString = ""
					FireSampleString = ""
					EmptyClipSampleString = ""
					GunAutoFireString = ""

					ObjTypeString = "string Class = \"CGun\";\r\r\n"

					if GunRollout.GunROF.value != 1.0 do
						ROFString = ("float ROF = " + (FloatAsString GunRollout.GunROF.value) + ";\r\r\n")

					if GunRollout.GunDamage.value != 2.0 do
						DamageString = ("float Damage = " + (FloatAsString GunRollout.GunDamage.value) + ";\r\r\n")

					if GunRollout.GunRange.value != 100.0 do
						RangeString = ("float Range = " + (FloatAsString GunRollout.GunRange.value) + ";\r\r\n")

					if GunRollout.GunPush.value != 100.0 do
						PushString = ("float Push = " + (FloatAsString GunRollout.GunPush.value) + ";\r\r\n")

					if GunRollout.GunAutoFire.checked do
						GunAutoFireString = "bool AutoFire = true;\r\r\n"

					if GunRollout.GunBarrel.text != "" do
						BarrelString = ("string Barrel = \"" + GunRollout.GunBarrel.text + "\";\r\r\n")

					if GunRollout.MFlashOrg.text != "" do
						MFlashOrgString = "string MflashOrg = \"" + GunRollout.MFlashOrg.text + "\";\r\r\n"

					if GunRollout.RecoilOrg.text != "" do
						(
							RecoilOrgString = "string RecoilOrg = \"" + GunRollout.RecoilOrg.text + "\";\r\r\n"
							RecoilForceString = ("float RecoilForce = " + (FloatAsString GunRollout.RecoilForce.value) + ";\r\r\n")
						)

					if GunRollout.MFlashes.items.count > 0 do
						(
							for i = 1 to GunRollout.MFlashes.items.count do
								MFlashObjectsString += ("string MFlashObject" + (i - 1) as string + " = \"" + GunRollout.MFlashes.items[i] + "\";\r\r\n")
							FlashDurationString = ("float MFlashDuration = " + (FloatAsString GunRollout.MFlashDuration.value) + ";\r\r\n")
						)
					
					if GunRollout.RndRot.checked do
						RandomRotateString = "bool RandomRotate = true;\r\r\n"

					if GunRollout.FireSample.text != "" do
						FireSampleString = ("string Sample = \"" + GunRollout.FireSample.text + "\";\r\r\n")

					if GunRollout.EmptyClipSample.text != "" do
						EmptyClipSampleString = ("string EmptyClipSample = \"" + GunRollout.EmptyClipSample.text + "\";\r\r\n")

					ObjParamsString = (ROFString + DamageString + RangeString + PushString + GunAutoFireString + BarrelString + MFlashOrgString + RecoilOrgString + RecoilForceString + MFlashObjectsString + FlashDurationString + RandomRotateString + FireSampleString + EmptyClipSampleString)
				)

			-- Backdrop Object
			if GeneralObjectType.selection == 15 do
				ObjTypeString = "string Class = \"CBackdrop\";"

			-- Marker Object
			if GeneralObjectType.selection == 16 do
				ObjTypeString = "string Class = \"Marker\";"

			-- Teleport Object
			if GeneralObjectType.selection == 17 do
				ObjTypeString = "string Class = \"Teleport\";\r\r\nbool Visible = false;\r\r\nbool Shadow = false;\r\r\nbool Wrap = false;\r\r\nbool Merge = false;\r\r\nbool Split = false;\r\r\n"

		-- write out the final stuff to the Object Properties buffer
		if (ObjTypeString == undefined) or (ObjParamsString == undefined) or (ObjParamsString2 == undefined) then
			(
				MessageBox "Undefined Parts\nSomething's fucked up."
			) else (
				if Preview.checked then
					(
						NewBuffer = (ObjTypeString + ObjParamsString + ObjParamsString2)
						NewBuffer = PruneNewLines NewBuffer
						format "%" NewBuffer
					) else (
						ObjArray = for obj in selection collect obj
						if ObjArray.count != 0 do
							for i = 1 to ObjArray.count do
								(
									NewBuffer = (ObjTypeString + ObjParamsString + ObjParamsString2)
		
									if AppendALLProps.checked do
										(
											OldBuffer = getUserPropBuffer ObjArray[i]
											NewBuffer = (OldBuffer + "\r\r\n" + NewBuffer)
										)
		
									if AppendProps.checked do
										(
											NewBuffer = (ObjParamsString + ObjParamsString2)
											OldBuffer = getUserPropBuffer ObjArray[i]
											NewBuffer = (OldBuffer + "\r\r\n" + NewBuffer)
										)
		
									SetUserPropBuffer ObjArray[i] NewBuffer
									if NewBuffer.count > 500 do
										MessageBox "The Object Properties you have set\nare over the advised limit of 500 characters.\n If you save your file and\nattempt to reload, MAX will likely crash\nand you will lose your work."
								)
					)
			)

		if CleanInterface.checked do
			(
				removeRollout ObjProps
				AddRollout ObjProps
				CleanInterface.checked = true
			)
	)

-- Settings Rollout
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

-- Terrain Texture Rollout
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

-- Rendering Properties Rollout
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

		checkbox IgnorePhysics "Ignore Physics Props"

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
				spinner SurfacePriority "Priority:" range:[0,4,0] type:#integer fieldwidth:35
				checkbox SurfaceAlwaysFace "Always Face"
				checkbox SurfaceNoCache "No Cache"
				checkbox SurfaceNoCacheAlone "No Cache Alone"
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

-- Physics Parameters Rollout
rollout PhysicsRollout "Physics Properties"
(
	local PhysicsDefaults = #(#(1,false,false,1,5,1,.5,.25,1,true))

	checkBox IgnoreRendering "Ignore Render Params"

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
)

-- Magnet Properties Rollout
rollout MagnetsRollout "Magnet Properties"
	(
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

-- Trigger Properties Rollout
rollout TriggersRollout "Trigger Properties"
	(
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

-- VoiceOver Rollout
rollout SoundRollout "Sound Attributes"
	(
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

		on close pressed do
			removeRollout SoundRollout
	)

-- Light Parameters Rollout
rollout LightRollout "Light Parameters"
	(
		spinner Intensity "Intensity: " range:[0,1,1] type:#float
		button close "Close"
		
		on close pressed do
			removeRollout LightRollout
	)

-- Gun Parameters Rollout
rollout GunRollout "Gun Parameters"
	(

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

		on close pressed do
			removeRollout GunRollout
	)

-- ***********************
-- AI Parameters Rollout
-- ***********************
rollout AIRollout "AI Action Parameters"
	(
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

		on AITarget picked obj do AITarget.text = obj.name
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

-- Mesh Substitution Actions
rollout MeshSubstRollout "Mesh Substitution Actions"
	(
		label Tl "Target: " align:#left offset:[-4,0]
		pickbutton Target width:100 offset:[0,-20] align:#right
		spinner Substitute "Substitute:" range:[0,20,0] type:#integer

		button close "Close"
		on close pressed do
			removeRollout MeshSubstRollout
	)

-- Physics Actions
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

-- Terrain Actions
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

-- Image Cache Actions
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

-- Renderer Actions
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

-- Fog Actions Rollout
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

-- Teleporter Rollout
rollout TeleporterRollout "Teleporter"
	(
		label sub "Subtitle:" enabled:false		
		editText Subtitle enabled:false		

		button close "Close"
		on close pressed do
			removeRollout FogRollout
	)

-- Overlay Rollout
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

-- Fade Music Rollout
rollout FadeMusicRollout "Fade Music Actions"
	(
		spinner VolFader "Vol Fader:" range:[-100,100,0]
		button close "Close"

		on close pressed do
			removeRollout FadeMusicRollout
	)

-- Sky Rollout
rollout SkyRollout "Sky"
	(
		spinner SkyFlatColorIndex "Flat Color Pal Index:" range:[0,63,0] type:#integer fieldwidth:35
		button close "Close"

		on close pressed do removeRollout SkyRollout
	)

-- Sky Actions Rollout
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

-- Utility Open
on ObjProps open do
	(
		MaterialTypesArray = #("")
		ActiveObject = ""
		CIndex = 1
		AddRollout PhysicsRollout rolledup:true
		AddRollout RenderingRollout rolledup:true
		removeRollout MagnetsRollout
		removeRollout TriggersRollout
		removerollout TerrainTextureRollout
		removeRollout GunRollout
		RemoveRollout FadeMusicRollout
		RemoveRollout SoundRollout
		RemoveRollout AIRollout
		RemoveRollout FogRollout
		RemoveRollout RendererRollout
		RemoveRollout TerrainActionsRollout
		RemoveRollout PhysicsActionsRollout
		RemoveRollout DepthSortRollout
		RemoveRollout TeleporterRollout
	)

-- Utility Close
on ObjProps close do
	(
		removeRollout RenderingRollout
		removerollout PhysicsRollout
		removeRollout MagnetsRollout
		removeRollout TriggersRollout
		removerollout TerrainTextureRollout
		removeRollout GunRollout
		RemoveRollout FadeMusicRollout
		RemoveRollout SoundRollout
		RemoveRollout AIRollout
		RemoveRollout FogRollout
		RemoveRollout RendererRollout
		RemoveRollout TerrainActionsRollout
		RemoveRollout PhysicsActionsRollout
		RemoveRollout DepthSortRollout
		RemoveRollout TeleporterRollout
	)
) -- end Utility
