fn IsMaster2 CName = 
	(
		local DashPos = -1
		for i = 1 to CName.count do
			if CName[i] == "-" do
				DashPos = i
		if DashPos == -1 then
			return true
		else
			(
				if (substring CName (DashPos) 3) == "-00" then
					return true
				else
					return false
			)
	)

fn RemBumps o = 
	(
		local NMat
		if classof o.mat == standardMaterial do
			(
				OMats = #(o.mat.diffuseMap, o.mat.opacityMap)
				NMat = standardMaterial()
				NMat.name = o.mat.name
				NMat.diffusemap = o.mat.diffusemap
				if Omats[2] != undefined do NMat.opacityMap = OMats[2]
			)

		if classOf o.mat == MultiMaterial do
			(
				NMat = MultiMaterial()
				NMat.name = o.mat.name
				ns = o.mat.numsubs
				NMat.numsubs = ns
				for i = 1 to ns do
					(
						NMat[i] = standardMaterial()
						NMat[i].name = o.mat[i].name
						NMat[i].diffuseMap = o.mat[i].diffuseMap
						if o.mat[i].opacityMap != undefined do NMat[i].opacityMap = o.mat[i].opacityMap 
					)
			)
		gc()
		return NMat
	)

fn CleanIDs o = 
	(
		if classof o.mat == Multimaterial do
			(
				local ns = o.mat.numsubs
				for i = 1 to o.numfaces do
					if (local iID = getFaceMatID o i) > ns do
						setFaceMatID o i (mod iID ns)
			)

		if classof o.mat == StandardMaterial do
			for i = 1 to o.numfaces do setFaceMatID o i 1
		update o
	)

Utility BitmapTools "Bitmap Tools"
	(
	
		local SubMaterialView, SubMatNameArray = #(), CNumsubs = 1, MaterialArray, CObj, CFaceArray,
		NFaceArray, Cnf, WackyIDs, NMat, CSub, CMat, NewIDs, CID, OldBitmap, NewBitmap
		
		local debug = false
		
		group "Map Path Info"
			(
				radiobuttons infotype labels:#("All", "Selected")
				button getMapInfo "Get Bitmap Info" width:140
				label MapPathInfoLabel1 "Map Paths are Displayed"
				label MapPathInfoLabel2 "in Listener Window"
			)
		
		group "Map Path Remapper"
			(
				checkbox WDialogs "Show Warning Dialogs" checked:true
				button RemapSelected "Remap Selected" width:140
			)
		
		group "Multi-Sub Manager"
			(
				pickButton pickObject "Pick Object" width:140 enabled:false
				button RebuildSelected "Rebuild Selected" width:140
				button RemBumpsSel "Remove Bump Maps" width:140
			)
		
		
		group "Map Replacement"
			(
				label b1label "Old:" align:#left
				button b1 width:100 align:#right offset:[5,-20]
				label b2label "New:" align:#left
				button b2 width:100 align:#right offset:[5,-20]
				button remapSelection "Remap Selection"
			)
		
		on RemBumpsSel pressed do
			(
				ObjArray = selection as array
				oc = ObjArray.count
				ocInv = (1.0 / oc) * 100
				ProgressStart "Removing Bumps...."
				for i = 1 to oc do
					(
						ProgressUpdate (ocInv * i)
						CObj = ObjArray[i]
						CObj.mat = RemBumps CObj
						if debug do print heapfree
					)
				ProgressEnd()
			)

		on b1 pressed do
			(
				if (OldBitmap = selectBitmap()) != undefined then	
					(
						b1.text = (FileNameFromPath OldBitmap.filename)
					) else (
						OldBitmap = undefined
					)
			)
	
		on b2 pressed do
			(
				if (NewBitmap = selectBitmap()) != undefined then	
					(
						b2.text = (FileNameFromPath NewBitmap.filename)
					) else (
						NewBitmap = undefined
					)
			)
		
		on remapSelection pressed do
			(
				if OldBitmap != undefined and NewBitmap != undefined then
					(
						ObjArray = for obj in selection collect obj
						oc = ObjArray.count 
						SearchName = (FilenameFromPath OldBitmap.filename)
						for i = 1 to oc do
							(
								o = ObjArray[i]
								if ClassOf o.mat == standardmaterial do
									(
										if classof o.mat.diffusemap == bitmaptexture do
											(
												Cname = (FileNamefromPath o.mat.diffusemap.filename)
												if CName == SearchName do
													o.mat.diffusemap.filename = NewBitMap.filename
											)
		
										if classof o.mat.opacitymap == bitmaptexture do
											(
												Cname = (FileNamefromPath o.mat.opacitymap.filename)
												if CName == SearchName do
													o.mat.opacitymap.filename = NewBitMap.filename
											)
		
										if classof o.mat.bumpmap == bitmaptexture do
											(
												Cname = (FileNamefromPath o.mat.bumpmap.filename)
												if CName == SearchName do
													o.mat.bumpmap.filename = NewBitMap.filename
											)
									)
		
								if ClassOf o.mat == Multimaterial do
									(
										ns = o.mat.numsubs
										for j = 1 to ns do
											(
												if classof o.mat[j].diffusemap == bitmaptexture do
													(
														Cname = (FileNamefromPath o.mat[j].diffusemap.filename)
														if CName == SearchName do
															o.mat[j].diffusemap.filename = NewBitMap.filename
													)
												if classof o.mat[j].opacitymap == bitmaptexture do
													(
														Cname = (FileNamefromPath o.mat[j].opacitymap.filename)
														if CName == SearchName do
															o.mat[j].opacitymap.filename = NewBitMap.filename
													)
												if classof o.mat[j].bumpmap == bitmaptexture do
													(
														Cname = (FileNamefromPath o.mat[j].bumpmap.filename)
														if CName == SearchName do
															o.mat[j].bumpmap.filename = NewBitMap.filename
													)
											)
									)
							)
						messageBox "Done"
					) else (
						MessageBox "Bitmaps are not defined"
					)
			
			)

		on BitmapTools open do SubMatNameArray = #()

		on BitmapTools close do removeRollout SubMaterialView

		on PickObject Picked obj do
			(
				removeRollout SubMaterialView
				SubMatNameArray = #()
				CFaceArray = #()
				NFaceArray = #()
				if classof obj != editable_mesh then
					(
						if (querybox "Object is not an EditableMesh, do you want to Collapse?\n\n If you click No, a copy of the object will be made") == true then
							(
								ConvertToMesh Obj
								CObj = obj
							) else (
								CObj = copy Obj	
								ConvertToMesh CObj
								CObj.name = (Obj.name + "-Copy")
							)
					) else (
						CObj = obj
						ConvertToMesh CObj
					)
				Cnf = CObj.numfaces
				NMat = undefined
		-- build FaceID Array
				for i = 1 to Cnf do (CFaceArray[i] = (getfaceMatID CObj i))
				if classof CObj.mat == MultiMaterial then
					(
						CNumSubs = CObj.mat.numsubs
						for i = 1 to CNumSubs do (append SubMatNameArray (i as string + ": " +(CObj.mat[i]).name))
						addrollout SubMaterialView rolledup:false
					) else (
						messagebox "Picked Object does not have\n a MultiMaterial assigned to it!"
					)
			)
		
		rollout SubMaterialView "Sub Materials"
		(
			listbox SubMatList "Sub Materials:" height:10 items:SubMatNameArray
			button RemoveSubMaterial "Remove" width:145
			button MoveUp "Bump Up" width:70 offset:[-37,0] enabled:false
			button MoveDown "Bump Down" width:70 offset:[37,-26] enabled:false
			button RebuildMat "Rebuild Material" width:145
			button SimplifyMaterial "Simplify Material" width:145
			label status1
		
		
		-- *********************************************************************************************
		-- Simplify Material
		-- *********************************************************************************************
		on SimplifyMaterial pressed do
			(
				MaterialArray = #()
				CMat = CObj.mat
		
				-- build the array of material definitions
				for i = 1 to CNumSubs do
					(
						MaterialArray[i] = #("","","")
						if classof CMat[i].diffusemap == bitmaptexture do
							MaterialArray[i][1] = (filenameFromPath CObj.mat[i].diffusemap.filename)
						if classof CMat[i].OpacityMap == bitmaptexture do
							MaterialArray[i][2] = (filenameFromPath CObj.mat[i].OpacityMap.filename)
						if classof CMat[i].BumpMap == bitmaptexture do
							MaterialArray[i][3] = (filenameFromPath CObj.mat[i].BumpMap.filename)
					)
				NewIDs = #()
				for i = 1 to MaterialArray.count do
					(
						NewIDs[i] = #(-1)
						CMat = MaterialArray[i]
						if (CMat[1] == "") and (CMat[2] == "") and (CMat[3] == "") then
							(
								1 == 1
							) else (
								for j = 1 to MaterialArray.count do	
									if i != j do
										if (CMat[1] == MaterialArray[j][1]) and (CMat[2] == MaterialArray[j][2]) and (CMat[3] == MaterialArray[j][3]) do
											(
												format "Found Match: i = %; j = %\n" i j
												append NewIDs[i] j
												-- clear out that MaterialArray#() index
												MaterialArray[j] = #("", "", "")
											)
							)
					)
				for i = 1 to NewIDs.count do
					(
						if NewIDs[i].count > 1 do
							(
								for j = 2 to NewIDs[i].count do
									for k = 1 to CObj.numfaces do	
										(
											CID = getfaceMatID CObj k
											if CID == NewIDs[i][j] do (setFaceMatID CObj k i)
										)	
							)
					)
				update CObj
			)
			
		on RemoveSubMaterial pressed do
			(
				CSub = SubMatList.selection
				CMat = CObj.mat
		
				-- build the new Multi Material
				NMat = MultiMaterial()
				NMat.name = CMat.name
				NMat.numsubs = (CNumSubs - 1)
				NMat.name = CMat.name
				for i = 1 to NMat.numsubs do
					(
						if i >= CSub do (NMat[i] = CMat[i+1])
						if i < CSub do (NMat[i] = CMat[i])
					)
		
				-- Modify CObj's face Mat ID's
				WackyIDs = false
				for i = 1 to CFaceArray.count do
					(
						Cid = CFaceArray[i]
						if Cid == CSub do 
							(
								CFaceArray[i] = 1000
								WackyIDs = true
							)
						if Cid > CSub do (CFaceArray[i] = (Cid - 1))
					)
				-- apply the FaceMatID Array back to the object
				for i = 1 to CObj.numfaces do
					(
						setfaceMatid CObj i CFaceArray[i]
					)
				-- Apply the new material to CObj
				CObj.mat = NMat
				
				-- Update all the interface stuff
				removeRollout SubMaterialView
				CNumSubs = CObj.mat.numsubs
				SubMatNameArray = #()
				for i = 1 to CNumSubs do 
					(
						append SubMatNameArray (i as string + ": " +(CObj.mat[i]).name)
					)
				AddRollout SubMaterialView
				if WackyIDs == true do	(MessageBox "Faces found in object that have\nthe Material ID of the deleted Sub Material.\nSuch faces now have the\nMaterial ID of 1000")
			)
			
		on RebuildMat pressed do
			(
			FaceIDArray = #()
			UniqueFaceIDArray = #()
			ns = CObj.mat.numsubs
			nf = CObj.numfaces
			NMat = MultiMaterial()
			FaceIDArray = for i = 1 to nf collect getFaceMatID CObj i
			for i = 1 to nf do
				(
					if finditem UniqueFaceIDArray FaceIDArray[i] == 0 do (append UniqueFaceIDArray FaceIDArray[i])
				)
			NMat = MultiMaterial()
			NMat.name = CObj.mat.name
			NMat.numsubs = UniqueFaceIDArray.count
			NewSubMatCount = 0
			for i = 1 to ns do
				(
					status1.text = i as string
					if findItem FaceIDArray i != 0 then
						(
							NewSubMatCount = NewSubMatCount + 1
							for j = 1 to nf do
								(
									if FaceIDArray[j] == i do (SetFaceMatID CObj j NewSubMatCount) -- reset the face material ID
									NMat[NewSubMatCount] = CObj.mat[i] -- toss the new Sub Material into the new MM
								)
						) else (
						1 == 1
						)
				)
			standard = false
			if NMat.numsubs == 1 do
				(
					NMat2 = StandardMaterial()
					NMat2.name = NMat.name
					NMat2.diffusemap = bitmaptexture()
					NMat2.diffusemap = NMat[1].diffusemap
					if NMat[1].OpacityMap == bitmaptexture do
						(
							NMat2.Opacitymap = bitmaptexture()
							NMat2.opacitymap = NMat[1].OpacityMap
						)
					if NMat[1].BumpMap == bitmaptexture do
						(
							NMat2.Bumpmap = bitmaptexture()
							NMat2.BumpMap = NMat[1].Bumpmap
						)
					NMat = NMat2
					NMat.name = NMat2.name
					standard = true
				)
			CObj.mat = NMat -- asign the new material to CObj
			if CObj.mat.name == "" do 
				CObj.mat.name = CObj.name
			-- rebuild the interface
			removeRollout SubMaterialView
			if standard == false do
				(
					CNumSubs = CObj.mat.numsubs
					SubMatNameArray = #()
					for i = 1 to CNumSubs do 
						append SubMatNameArray (i as string + ": " +(CObj.mat[i]).name)
					AddRollout SubMaterialView
				)
			)
		) -- end rollout
		
		
		-- *******************************************************************
		-- * Rebuild Selection
		-- *******************************************************************
		on RebuildSelected pressed do
			(
				ObjArray = #()
				-- make sure we're only getting geometry into the tool
				g = geometry as array
				for obj in selection do
					if (findItem g obj) != 0 do
						append ObjArray obj

				oc = ObjArray.count
				ocInv = (1.0 / oc) * 100 -- ProgressUpdate() scalar
				HeapNow = heapFree
				ProgressStart ("Rebuilding..." + oc as string)
				for i = 1 to oc do
					(
						CObj = ObjArray[i]
						HeapBefore = HeapNow
						HeapNow = heapFree
						-- if (mod i 10) == 0 do gc()
						CleanIDs CObj -- fn call to CleanIDs()
						MaterialArray = #()
						CMat = CObj.mat
						if CMat != undefined do
							(
								ProgressUpdate (i * ocInv)
								if classof CMat == MultiMaterial do
									(
										CNumSubs = CMat.numsubs
										for i = 1 to CNumSubs do
											(
												local submat = CMat[i]
												if submat != undefined do
													(
														MaterialArray[i] = #("","","")
														if classof submat.diffusemap == bitmaptexture do
															MaterialArray[i][1] = (filenameFromPath submat.diffusemap.filename)
														if classof submat.OpacityMap == bitmaptexture do
															MaterialArray[i][2] = (filenameFromPath submat.OpacityMap.filename)
														if classof submat.BumpMap == bitmaptexture do
															MaterialArray[i][3] = (filenameFromPath submat.BumpMap.filename)
													)
											)
										NewIDs = #()
										for i = 1 to MaterialArray.count do
											(
												NewIDs[i] = #(-1)
												local Mat = MaterialArray[i]
												if (Mat[1] != "") or (Mat[2] != "") or (Mat[3] != "") do
													for j = 1 to MaterialArray.count do	
														if i != j do
															if (Mat[1] == MaterialArray[j][1]) and (Mat[2] == MaterialArray[j][2]) and (Mat[3] == MaterialArray[j][3]) do
																(
																	append NewIDs[i] j
																	-- clear out that MaterialArray#() index
																	MaterialArray[j] = #("", "", "")
																)
											)
										for i = 1 to NewIDs.count do
											if NewIDs[i].count > 1 do
												for j = 2 to NewIDs[i].count do
													for k = 1 to CObj.numfaces do	
														(
															CID = getfaceMatID CObj k
															if CID == NewIDs[i][j] do (setFaceMatID CObj k i)
														)	
										update CObj
										FaceIDArray = #()
										UniqueFaceIDArray = #()
										CMat = CObj.mat
										ns = CMat.numsubs
										nf = CObj.numfaces
										FaceIDArray = for i = 1 to nf collect getFaceMatID CObj i
										for i = 1 to nf do
												if finditem UniqueFaceIDArray FaceIDArray[i] == 0 do (append UniqueFaceIDArray FaceIDArray[i])
										NMat = MultiMaterial()
										NMat.name = CMat.name
										NMat.numsubs = UniqueFaceIDArray.count
										NewSubMatCount = 0
										local smats = for m in CMat collect m
										for i = 1 to ns do
											if findItem FaceIDArray i != 0 do
												(
													NewSubMatCount = NewSubMatCount + 1
													for j = 1 to nf do
														(
															if FaceIDArray[j] == i do (SetFaceMatID CObj j NewSubMatCount) -- reset the face material ID
															NMat[NewSubMatCount] = smats[i] -- toss the new Sub Material into the new MultiMaterial
														)
												)
										NewSubMatCount = FaceIDArray = UniqueFaceIDArray = ns = nf = i = undefined
										if NMat.numsubs == 1 do
											(
												NMat2 = StandardMaterial()
												NMat2.name = NMat.name
												NMat2.diffusemap = bitmaptexture()
												NMat2.diffusemap = NMat[1].diffusemap
												if NMat[1].OpacityMap == bitmaptexture do
													(
														NMat2.Opacitymap = bitmaptexture()
														NMat2.opacitymap = NMat[1].OpacityMap
													)
												if NMat[1].BumpMap == bitmaptexture do
													(
														NMat2.Bumpmap = bitmaptexture()
														NMat2.BumpMap = NMat[1].Bumpmap
													)
												NMat = NMat2
												NMat.name = NMat2.name
											)
										CObj.mat = NMat -- asign the new material to CObj
										NMat = NMat2 = undefined
										if CObj.mat.name == "" do CObj.mat.name = CObj.name
								)
							)
						MaterialArray = CMat = NMat = NewSubMatCount = CNumSubs = undefined
					)
				-- gc()
				ProgressEnd()
			)
		
		on getMapInfo pressed do
			(
				ObjArray = #()
				if infotype.state == 1 do (ObjArray = for o in objects collect o)
				if infotype.state == 2 do (ObjArray = for obj in selection collect obj)
				numObj = ObjArray.count
				for o in objArray do
					(
						if (isMaster2 o.name) do
							(
								format "Object: %\n" o.name
								if classof o.mat == standardmaterial do
									(
										if classof o.mat.diffusemap == bitmaptexture do (format "%\n" o.mat.diffusemap.filename)
										if classof o.mat.opacitymap == bitmaptexture do (format "%\n" o.mat.opacitymap.filename)
										if classof o.mat.bumpmap == bitmaptexture do (format "%\n" o.mat.bumpmap.filename)
									)
							
								if classof o.mat == multimaterial do
									(
										ns = o.mat.numsubs
										for i = 1 to ns do
											(
												if classof o.mat[i].diffusemap == bitmaptexture do (format "%\n" o.mat[i].diffusemap.filename)
												if classof o.mat[i].opacitymap == bitmaptexture do (format "%\n" o.mat[i].opacitymap.filename)
												if classof o.mat[i].bumpmap == bitmaptexture do (format "%\n" o.mat[i].bumpmap.filename)
											)
									)
							)
					)
			)
		
		on RemapSelected pressed do
		(
				Objarray = for obj in selection collect obj
				oc = ObjArray.count
				if oc >= 1 then
					(
						if (SaveLoc = getsavePath caption:"Specify map folder") != undefined do
							(
								for i = 1 to oc do
									(
										o = ObjArray[i]
										MapNotFound = false
				
										-- if we have a standard material
										if classof o.mat == StandardMaterial do
											(
						
												if classof o.mat.diffusemap == bitmaptexture do
													(
														MapName = FilenamefromPath o.mat.diffusemap.filename
														NewFileName = (SaveLoc + "\\" + MapName)
														if (getfiles NewFileName).count != 0 then
															(
																o.mat.diffusemap.filename = NewFileName
															) else (
																MapNotFound = true
																if WDialogs.checked do
																	Messagebox ("Bitmap " + MapName + " was not found\nin the specified location.")
															)
													)
						
												if classof o.mat.opacitymap == bitmaptexture do
													(
														MapName = FilenamefromPath o.mat.OpacityMap.filename
														NewFileName = (SaveLoc + "\\" + MapName)
														if (getfiles NewFileName).count != 0 then
															(
																o.mat.OpacityMap.filename = NewFilename
															) else (
																MapNotFound = true
																if WDialogs.checked do
																	Messagebox ("Bitmap " + MapName + " was not found\nin the specified location.")
															)
													)
						
												if classof o.mat.BumpMap == bitmaptexture do
													(
														MapName = FilenamefromPath o.mat.BumpMap.filename
														NewFileName = (SaveLoc + "\\" + MapName)
														if (getfiles NewFileName).count != 0 then
															(
																o.mat.BumpMap.filename = NewFilename
															) else (
																MapNotFound = true
																if WDialogs.checked do
																	Messagebox ("Bitmap " + MapName + " was not found\nin the specified location.")
															)
													)
											)
										-- if we have a multi material
										if classof o.mat == MultiMaterial do
											(
												MaterialArray = #()
												ns = o.mat.numsubs
												for i = 1 to ns do
													(
														if classof o.mat[i].diffusemap == bitmaptexture do
															(
																MapName = FilenamefromPath o.mat[i].diffusemap.filename
																NewFileName = (SaveLoc + "\\" + MapName)
																if (getfiles NewFileName).count != 0 then
																	(
																		o.mat[i].diffusemap.filename = NewFilename
																	) else (
																		MapNotFound = true
																		if WDialogs.checked do
																			Messagebox ("Bitmap " + MapName + " was not found\nin the specified location.")
																	)
															)
						
														if classof o.mat[i].opacitymap == bitmaptexture do
															(
																MapName = FilenamefromPath o.mat[i].OpacityMap.filename
																NewFileName = (SaveLoc + "\\" + MapName)
																if (getfiles NewFileName).count != 0 then
																	(
																		o.mat[i].OpacityMap.filename = NewFilename
																	) else (
																		MapNotFound = true
																		if WDialogs.checked do
																			Messagebox ("Bitmap " + MapName + " was not found\nin the specified location.")
																	)
															)
						
														if classof o.mat[i].BumpMap == bitmaptexture do
															(
																MapName = FilenamefromPath o.mat[i].BumpMap.filename
																NewFileName = (SaveLoc + "\\" + MapName)
																if (getfiles NewFileName).count != 0 then
																	(
																		o.mat[i].BumpMap.filename = NewFilename
																	) else (
																		MapNotFound = true
																		if WDialogs.checked do
																			Messagebox ("Bitmap " + MapName + " was not found\nin the specified location.")
																	)
															)
													)
											)
										if MapNotfound == true do format "Some maps in object % were not re-mapped\n" o.name
									)
						)
					MessageBox "Done!"
				) else (
					MessageBox "Nothing Selected!\nPlease note the name of this tool...\n\n\"Remap SELECTED\""
				)
			)
	)