fn round num = (num + 0.5) as integer

fn InstanceNum CName = 
	(
		local DashPos = -1
		for i = 1 to CName.count do
			if CName[i] == "-" do
				DashPos = i
		if DashPos == -1 then
			return undefined 
		else
			(
				Num = (substring CName (DashPos + 1) (CName.count - DashPos))
				return Num
			)
	)

fn KeyName CName = 
	(
		local DashPos = -1
		for i = 1 to CName.count do
			if CName[i] == "-" do
				DashPos = i
		if DashPos == -1 then
			return CName
		else
			return (substring CName 1 (DashPos - 1))
	)

fn ValidTrnObj obj = 
	(
		update obj
		local Valid = true
		for i = 1 to obj.numfaces do
			if (GetFaceNormal obj i) != [0,0,1] do
				(
					Valid = false
					exit
				)
		return Valid
	)

fn Sel0 = 
	(
		for o in objects do
			if (substring o.name (o.name.count - 2) 3) == "-00" do
				selectmore o
	)

fn MakeTrnObjValid obj = 
	(	
		convertToMesh obj
		local Cntr = obj.center.z
		for i = 1 to obj.numverts do
			(
				CVert = getVert obj i
				setVert obj i [CVert.x, Cvert.y, Cntr]
			)
		update obj	
	)

-- *********************************************************************
-- Start Utility
-- *********************************************************************

Utility SceneBuildingUltils "Scene Utils"
	(
		local c, p, PKeyName, CKeyName, XYThresh
		
		local debug = false
	
		group "Activate/Deactivate"
			(
				button DeactivateSelected "Deactivate Selection" width:140
				button ActivateSelected "Activate Selection" width:140
			)

		group "Multiple Object Placement"
			(
				button clearspinners "Clear Spinners"
				spinner XPos "X: " range:[-10000,10000,0] offset:[-20,0]
				checkbutton UseX align:#right offset:[0,-23] checked:true
				spinner YPos "Y: "range:[-10000,10000,0] offset:[-20,0]
				checkbutton UseY align:#right offset:[0,-23] checked:true
				spinner ZPos "Z: " range:[-10000,10000,0] offset:[-20,0]
				checkbutton UseZ align:#right offset:[0,-23] checked:true
	
				button RePosSelection "Reposition Selection" width:140
			)

		group "Terrain Textures"
			(
				button SelectInvalids "Sel Bad Terrain Textures"
				CheckBox AutoFixTrns "Auto Fix Terrain Textures" checked:true
			)
	
		group "Match Maker"
			(
				label PLabel "Parent:" align:#left offset:[-4,0]
				pickbutton GetParent width:100 align:#right offset:[0,-20]
				label CLabel "Child:" align:#left offset:[-4,0]
				pickbutton GetChild width:100 align:#right offset:[0,-20]
				checkbox MatchStragglers "Match Straglers"
				
				button match "Match Names" width:140
			)
		
		group "Wire Color!"
			(
				colorPicker StartColor "Start:" color:(color 240 10 230) offset:[-45,0]
				colorPicker EndColor "End:" color:(color 228 128 10) offset:[30,-25]
				button ReColorSelection "Recolor Selection" width:140
			)

		group " Object Xform Mods"
			(
				button ProcessSelection "Process Selection" width:140
				checkbox rot "Rotation" checked:true
				checkbox scale "Scale"
			)

		on ProcessSelection pressed do
			(
				Objarray = selection as array
				oc = ObjArray.count
				for i = 1 to oc do
					(
						o = ObjArray[i]
						oPos = o.pos
						if rot.checked do
							o.rotation = (quat 0 0 0 1)
						if scale.checked do
							o.scale = [1,1,1]
						o.pos = oPos
					)
			)

		on ReColorSelection pressed do
			(
				sel = selection as array
				sc = sel.count
				RedOffset = ((StartColor.color.r - EndColor.color.r) as float / sc)
				GreenOffset = ((StartColor.color.g - EndColor.color.g) as float / sc)
				BlueOffset = ((StartColor.color.b - EndColor.color.b) as float / sc)
				for i = 1 to sc do
					(
						NewRed = round (StartColor.color.r - RedOffset * i)
						NewGreen = round (StartColor.color.g - GreenOffset * i)
						NewBlue = round (StartColor.color.b - BlueOffset * i)
						nwc = (color NewRed NewGreen NewBlue)
						sel[i].wirecolor = nwc
					)
			)

		on GetParent Picked obj do
			(
				p = obj
				PKeyName  = keyname p.name
				if PKeyName != GetChild.text then
					(
						GetParent.text = PKeyName
					) else (
						GetParent.text = ""
						p = PKeyName = undefined
						Messagebox "Parent and Child objects\nmust be different"
					)
			)

		on GetChild Picked obj do
			(
				c = obj
				CKeyName  = keyname c.name
				if CKeyName != GetParent.text then
					(
						GetChild.text = CKeyName
						xyMin = [c.min.x, c.min.y]
						xyMax = [c.max.x, c.max.y]
						XYThresh = (distance xyMin xyMax) * 0.5
					) else (
						GetChild.text = ""
						c = CKeyName = XYThresh = undefined
						MessageBox "Parent and Child objects\nmust be different"
					)
			)

		on Match pressed do
			(
				ParentList = #()
				ChildList = #()
				UsedChildren = #()
				UsedParents = #()
				-- find the valid parents and children in the scene
				ProgressStart "Matching...."
				for o in objects do
					(
						if (KeyName o.name) == CKeyName do
							(
								append ChildList o
								append UsedChildren #Free
							)
						if (KeyName o.name) == PKeyName do
							(
								append ParentList o
								append UsedParents #Free
							)
					)
				cnt = 0
				ocInv = (1.0 / ParentList.count) * 100
				for i = 1 to ParentList.count do
					(
						ProgressUpdate (i * ocInv)
						CParent = ParentList[i]
						PNum = InstanceNum CParent.name
						CParentPos = CParent.pos
						co = -1
						CIndex = -1
						sd = 9999999.1
						for j = 1 to ChildList.count do
							(
								CChild = ChildList[j]
								CChildPos = CChild.pos
								-- get the smallest distance from the parent to the child
								if ((cd = distance [CParentPos.x, CParentPos.y] [CChildPos.x, CChildPos.y]) <= sd) do
									(
										co = CChild 
										sd = cd
										CIndex = j
									)
							)
						if UsedChildren[CIndex] == #Free do
							(
								co.name = CKeyName + "-" + PNum
								UsedChildren[CIndex] = #Used
								UsedParents[i] = #Used
								cnt += 1
							)
					)
				ProgressEnd()
				if MatchStragglers.checked do
					(
						ProgressStart "Matching Stragglers"
						ocInv = (1.0 / UsedParents.count) * 100
						for i = 1 to UsedParents.count do
							(
								ProgressUpdate (i * ocInv)
								if UsedParents[i] == #Free do
									for j = 1 to UsedChildren.count do
										if UsedChildren[j] == #Free do
											(
												PNum = InstanceNum ParentList[i].name
												ChildList[j].name = ((KeyName ChildList[j].name) + "-" + PNum)
												UsedParents[i] = #Used
												UsedChildren[j] = #Used
												cnt += 1
												Exit
											)
							)
						ProgressEnd()
					)
				MessageBox ((cnt * 2) as string + " object's names were re-matched")
			)


		on SelectInvalids pressed do
			(
				TrnObjs = #()
				-- Identify the terrain texture objects in the scene
				for o in geometry do
					(
						CBuffer = getUserPropBuffer o
						if findstring CBuffer "CTerrainObj" != undefined do
							append TrnObjs o
					)
	
				-- check 'em
				if TrnObjs.count > 0 do
					(
						for i = 1 to TrnObjs.count do
							if (ValidTrnObj TrnObjs[i]) == false do
								(
									selectmore TrnObjs[i]
									if AutoFixTrns.checked do (MakeTrnObjValid TrnObjs[i])
								)
						select TrnObjs
					)
			)

		on ClearSpinners pressed do
			(
				XPos.value = YPos.value = ZPos.value = 0
				UseX.checked = UseY.checked = UseZ.checked = true
			)

		on RePosSelection pressed do
			(
				ObjArray = for obj in selection collect obj
				for i = 1 to ObjArray.count do
					(
						CObj = Objarray[i]
						NewPos = [0,0,0]
						if UseX.checked then
							NewPos.x = XPos.value
						else
							NewPos.x = CObj.pos.x
	
						if UseY.checked then
							NewPos.y = YPos.value
						else
							NewPos.y = CObj.pos.y
	
						if UseZ.checked then
							NewPos.z = ZPos.value
						else
							NewPos.z = CObj.pos.z
						
						CObj.pos = NewPos
					)
			)

		on DeactivateSelected pressed do
			(
				ObjArray = for obj in selection collect obj
				oc = ObjArray.count
				for i = 1 to oc do
					(
						o = ObjArray[i]
						CName = o.name
						if (substring CName 1 1) != "!" do	
							(
								CName = ("!" + CName)
								o.name = CName
							)
					)
			)

		on ActivateSelected pressed do
			(
				ObjArray = for obj in selection collect obj
				oc = ObjArray.count
				for i = 1 to oc do
					(
						o = ObjArray[i]
						CName = o.name
						if (substring CName 1 1) == "!" do	
							(
								CName = (substring CName 2 (CName.count - 1))
								o.name = CName
							)
					)
			)
	)