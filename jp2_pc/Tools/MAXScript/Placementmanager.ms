-- *******************************************************************************************
-- *
-- * Copyright © DreamWorks Interactive, 1997
-- *
-- * Contents:
-- *           Implementation of PlacementManager.ms
-- * Bugs:
-- *
-- * To do:
-- *
-- * Notes:
-- *
-- *******************************************************************************************
-- [1]: Key Object Name <string>
-- [2]: Type; #Child or #Parent <name>, #unused
-- [3]: ChildIndices #(int, int, int, ...)
-- [4]: Parent Index <int>
-- [5]: Scale Offset <point3>
-- [6]: Initial Vector <point3>; (c.pos - p.pos) * (p.rotation) as matrix3
-- [7]: Offset Quaternion rotation
-- [8]: Children Scale <bool>
-- [9]: Initial Rotation <quat>
-- *******************************************************************************************

fn AvgScale scl = ((scl.x + scl.y + scl.z) * 0.33333333)

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

fn Prune00s CName = 
	(
		TempName = CName
		if (substring TempName (TempName.count - 2) 3) == "-00" do
			TempName = substring TempName 1 (TempName.count - 3)
		return TempName
	)

fn FindByName CName = 
	(
		for o in objects do
			if (findString o.name CName) != undefined do
				return o
		return undefined
	)

fn Find00th CName = 
	(
		SearchName = (CName + "-00")
		for o in objects do
			if o.name == SearchName do
				return o
		return undefined
	)


fn RotChild p c InitialVec InitialRot = 
	(
		ParentRot = p.rotation
		cm = matrix3 1
		NewRot = (rotate cm (inverse ParentRot))
		c.rotation = ParentRot + InitialRot
		NewVec = InitialVec * cm
		c.pos = p.pos + NewVec
	)

-- Places an object [o], relative to another objects pos [p] on normal [N] ad distance [m].
fn PlcObj p o N m = o.pos = p - (N) * m

-- returns the magnitude of a vector
fn MagV vec = (sqrt((vec.x)^2 + (vec.y)^2 + (vec.z)^2))

-- takes two non-normalized vectors and returns a quaternion rotation between them
fn QuatFromVec v1 v2 = 
	(
		MagV1 = MagV v1
		MagV2 = MagV v2
		x = (cross v1 v2)
		xN = normalize x
		MagX = MagV x
		theta = asin(MagX/(MagV1 * MagV2))
		return (quat theta xN)
	)

-- returns a normalized vector between two objects
fn GetNormalDirVec o1 o2 = normalize (o2.pos - o1.pos)

-- returns a non-normalized vector between two objects
fn GetDirVec o1 o2 = (o2.pos - o1.pos)

-- returns a ray from one object to another
fn CreateRay o1 o2 = 
	return (ray o1.pos (GetDirVec o1 o2))

-- Builds the name list and object definitions/offsets when adding/removing things from a hierarchy
fn BuildNameList i dta NameData tab ObjArray = 
	(
		local tab2 = tab
		local CData = dta[i]
		tabstr = ""
		for k = 1 to tab2 do tabstr = tabstr + "   "
		append NameData (tabstr + CData[1])
		append ObjArray CData[9]
		if CData[3].count != 0 do
			for j = 1 to CData[3].count do
				(
					local i2 = CData[3][j]
					BuildNameList i2 dta NameData (tab2 + 1) ObjArray
				)
	)

-- updates the offset pos/rot/scale from a parent object
fn UpdatePositions i dta = 
	(
		local CData = dta[i]
		-- format "CData[3].count = %\n" CData[3].count
		if CData[3].count > 0 do
			for j = 1 to CData[3].count do
				(
					-- format "j = %\n" j
					local CurrentChild = CData[3][j]
					local Parent = dta[CurrentChild][4]
					if dta[CurrentChild][9] != #NoObject do
						(
							-- format "dta[CurrentChild][9] is valid...\n"
							NewVec = (dta[CurrentChild][6] * (avgScale dta[CurrentChild][5]))
--						RotChild dta[Parent][9] dta[CurrentChild][9] (dta[CurrentChild][6]) dta[CurrentChild][7]
							RotChild dta[Parent][9] dta[CurrentChild][9] NewVec dta[CurrentChild][7]
							dta[CurrentChild][9].scale = dta[CurrentChild][5]
							UpdatePositions CurrentChild dta
						)
				)
	)

-- ******************************************************************************	
-- * Update Selection
-- ******************************************************************************	
-- i is index of Parent object data into RData#()
-- dta is RData#()
-- NParent is the current parent Object
fn UpdateSelection i dta NParent PScale SclChildren = 
	(
		local debug = false
		local ScaleChildren = SclChildren
		ParentNum = InstanceNum NParent.name
		local CData = dta[i]
		-- format "CData[3].count = %\n" CData[3].count
		if CData[3].count > 0 do
			for j = 1 to CData[3].count do
				(
					local CurrentChild = CData[3][j]
					NewChildName = (dta[CurrentChild][1] + "-" + ParentNum)
					if debug do format "NewChildName: %\n" NewChildName
					local ChildMesh = #Null
					for o in objects do
						if o.name == NewChildName do
							ChildMesh = o
					if ChildMesh != #Null then
						(
							if debug do format "Found Child Object Match...%: %\n" NewChildName dta[CurrentChild][9]
							if (dta[CurrentChild][9] != #NoObject) do
								(
									if debug do format "% is valid...\n" dta[CurrentChild][9].name
									NewVec = (dta[CurrentChild][6] * (dta[CurrentChild][5] * (avgScale NParent.scale)))
									if debug do format "OldVec: %\nNewVec: %\n" dta[CurrentChild][6] NewVec
									RotChild NParent ChildMesh NewVec dta[CurrentChild][7]
									if SclChildren do
										(
											if debug do format "Scaling Children is active...\n"
											NewScale = (dta[CurrentChild][5] * (avgScale NParent.scale))
											if debug do format "New Scale: %\n" Newscale
											ChildMesh.scale = [Newscale, Newscale, Newscale]
										)
									UpdateSelection CurrentChild dta NParent PScale ScaleChildren
								)
						) else (
							format "No Match found for Child Object %\n" NewChildName
						)
				)
	)

-- *****************************************************************************************************************************	
-- * 																									Start Utility
-- *****************************************************************************************************************************	
Utility PlacementManager "Placement Manager"
	(
		local 	CParent, CChild, FinalParentPosition,
						RData = #(),
						ChildrenArray = #(),
						ObjArray = #()

		local debug = false

		group "Current Hierarchy"
			(
				listbox ObjectNames
				checkbox AutoSelect "Auto Select" offset:[0,0]
			)

		group "Selection Tools"
			(
				button PropagateInstances "Propagate to Instances" width:140
				button SelectGroupMembers "Sel Active Group Members"
				checkbox KeepSel "Keep Current Selection"
			)

		group "Hierarchy Modifications"
			(
				label ParentLabel "Parent:" align:#left
				pickbutton ChooseParent width:100 offset:[20,-20]
				ListBox Children "Current Children Set:" height:5
				checkbox ScaleChildren "Children scale with Parent" checked:true
				button AddChild "Add" width:38 offset:[-53,0]
				button RemoveChild "Remove" width:55 offset:[-2,-26]
				button ClearList "Clear" width:40 offset:[49,-26]
				checkbox AutoClear "AutoClear Parent/Children" checked:true
				button CreateRelationship "Create Relationship" width:140
			)

		button RefreshSelection "Refresh Selection" width:140
		label filler
		group "Files"
			(
				button SaveFile "Save" width:70 offset:[-37,0]
				button LoadFile "Load" width:70 offset:[37,-26]
				label FileInfo
				label SaveNow
			)


		on PropagateInstances pressed do
			(
				KeyNameArray = #()
				for obj in selection do
					if findItem KeyNameArray (KeyName obj.name) == 0 do
						append KeyNameArray (KeyName obj.name)
				oc = 1 / objects.count as float
				cnt = 0
				ProgressStart "Generating Selection...."
				sel = #()
				for o in objects where o.isselected == false do
					(
						cnt += 1
						progressUpdate ((cnt * oc) * 100)
						if findItem KeyNameArray (KeyName o.name) != 0 do
							(
								if o.isHidden do unhide o
								append Sel o
							)
					)
				selectmore sel
				ProgressEnd()
			)


		-- ******************************************************************************	
		-- * Select Group Members
		-- ******************************************************************************	
		on SelectGroupMembers pressed do
			(
				format "%\n" ObjectNames.selection
				s = ObjectNames.selection
				SelArray = #()
				if ObjArray[s] != undefined do
					(
						if KeepSel.checked == false do
							MAX Select None
						ParentName = (KeyName ObjArray[s].name)
						append SelArray ParentName
						-- Find the Parent in RData
						PIndex = #Null
						for i = 1 to RData.count do
							if RData[i][1] == ParentName do
								(
									PIndex = i
									Exit
								)
						-- Step through the children
						if PIndex != #Null do
							for i = 1 to RData[PIndex][3].count do
								(
									CIndex = RData[PIndex][3][i]
									append SelArray RData[CIndex][1]
								)
						if SelArray.count >= 1 do
							(
								prg = false
								oc = Objects.count
								ocInv = 1 / oc as float
								if oc > 50 do
									(
										ProgressStart "Generating Selection...."
										prg = true
									)
								cnt = 0
								for o in objects do	
									(
										cnt += 1
										if prg do progressUpdate ((cnt * ocInv) * 100)
										if (findItem SelArray (KeyName o.name)) != 0 do
											selectmore o
									)
								if prg do ProgressEnd()
							)
						if debug do format "SelArray: %\n" SelArray
					)
			)

		-- ******************************************************************************	
		-- * Initialize New File
		-- ******************************************************************************	
		on SaveFile pressed do
			(
				-- if we have something in RData#()
				if RData.count >= 2 then
					if (CFileName = getSaveFilename caption:"spec file") != undefined do
						(
							f = createFile CFileName
							for i = 1 to RData.count do
								(
									-- format the index we're in
									format "\n:%\n" i to:f
									format "%\n" RData[i][1] to:f -- Object Name
									if RData[i][2] == #Parent do
										format "#Parent\n" to:f
									if RData[i][2] == #Child do
										format "#Child\n" to:f
									format "%\n" RData[i][3].count to:f
									if RData[i][3].count == 0 then
										(
											format "0\n" to:f
										) else (
											for j = 1 to RData[i][3].count do
												format "% " RData[i][3][j] to:f
											format "\n" to:f
										)
									format "%\n" RData[i][4] to:f

									if RData[i][5] != #Null then
										format "%\n" RData[i][5] to:f
									else
										format "#Null\n" to:f

									if RData[i][6] != #Null then
										format "% % %\n" RData[i][6].x RData[i][6].y RData[i][6].z to:f
									else
										format "#Null\n" to:f

									if RData[i][7] != #Null then
										format "% % % %\n" RData[i][7].x RData[i][7].y RData[i][7].z RData[i][7].w to:f
									else
										format "#Null\n" to:f
									-- Scale children???
									format "%\n" RData[i][8] to:f
								)
							close f
							FileInfoText = (filenameFromPath CFileName)
							if FileInfoText.count > 25 do
								FileInfoText = ((substring FileInfoText 1 23) + "...")
							FileInfo.text = FileInfoText
							SaveNow.text = "File has been saved..."
						)
				else
					MessageBox "No Data!"				
			)		

		-- ******************************************************************************	
		-- * Load File
		-- ******************************************************************************	
		on LoadFile pressed do
			(
				if (OpenFileName = getOpenFilename caption:"Spec File pleeeeze...") != undefined do
					(
						f = openFile OpenFileName
						RData = #()
						if debug do format "Reading from File....\n"
						while not eof f do
							(
								CLine = readline f
								if CLine[1] == ":" do
									(
										NData = #()
										if CLine[CLine.count] == "\n" do Cline = (substring CLine 1 (CLine.count - 1))
										Index = (substring CLine 2 (CLine.count - 1)) as integer
										if debug do format "Index: %\n" Index
										CLine = Readline f
										-- prune off the newline character, if it exists
										if CLine[CLine.count] == "\n" do Cline = (substring CLine 1 (CLine.count - 1))
										-- this is the object name
										NData[1] = CLine
										CLine = Readline f
										if CLine[CLine.count] == "\n" do Cline = (substring CLine 1 (CLine.count - 1))
										if CLine == "#Parent" or CLine == "#parent" do NData[2] = #Parent
										if CLine == "#Child" or CLine == "#child" do NData[2] = #Child
										-- get the number of children
										CLine = (readValue f) as integer
										ca = #()
										if CLine != 0 then
											for i = 1 to CLine do
												append ca (readvalue f)
										else
											CLine = readline f
										NData[3] = ca

										-- This is the Parent - will be 0 if none
										NData[4] = (readValue f) as integer

										-- Scale float value
										if (tmp = readvalue f) != #Null then
											NData[5] = tmp
										else
											NData[5] = #Null

										-- Vector offset
										if (tmp = (readvalue f)) != #Null then
											NData[6] = [tmp, (ReadValue f), (ReadValue f)]
										else
											NData[6] = #Null

										-- Quaternion rotation
										if (tmp = (readvalue f)) != #Null then
											NData[7] = quat tmp (ReadValue f) (ReadValue f) (ReadValue f)
										else
											NData[7] = #Null

										NData[8] = readValue f

										RData[Index] = NData
									)
							)
						-- Update the interface
						if RData.count >= 1 do
							(
								NameArray = #()
								ObjArray = #()
								for i = 1 to RData.count do
									if (CObj = Find00th RData[i][1]) != undefined then
										RData[i][9] = CObj
									else
										RData[i][9] = #NoObject

								for i = 1 to RData.count do
									(
										if debug do format "%:%\n" i RData[i]
										if RData[i][2] == #Parent then
											BuildNameList i RData NameArray 0 ObjArray
									)
								ObjectNames.items = NameArray
								FileInfoText = (filenameFromPath OpenFileName)
								if FileInfoText.count > 25 do
									FileInfoText = ((substring FileInfoText 1 23) + "...")
								FileInfo.text = FileInfoText
							)
					)
			)

		-- ******************************************************************************	
		-- * Select Objects from interface
		-- ******************************************************************************	
		on ObjectNames selected s do
			if AutoSelect.checked do
				if ObjArray.count != 0 do
					select ObjArray[s]

		-- ******************************************************************************	
		-- * select Children from interface
		-- ******************************************************************************	
		on Children selected s do
			if AutoSelect.checked do
				if ChildrenArray.count != 0 do
					select ChildrenArray[s]

		-- ******************************************************************************	
		-- * Choose Parent Object
		-- ******************************************************************************	
		on ChooseParent picked obj do
			(
				if FindItem ChildrenArray obj == 0 then
					(
						if (avgScale obj.scale) == 1.0 then
							(
								CParent = obj
								ChooseParent.text = CParent.name
							) else (
								MessageBox "This object is scaled. Please make sure it has scale of 100% and try again."
							)
					) else (
						MessageBox "You cannot define a Parent Object\nthat is currently in the Child Object list"
						CParent = undefined
					)
			)

		-- ******************************************************************************	
		-- * Clear List
		-- ******************************************************************************	
		on ClearList pressed do
			(
				ChildrenArray = #()
				CList = #()
				Children.items = CList
			)

		-- ******************************************************************************	
		-- * Remove Child
		-- ******************************************************************************	
		on RemoveChild pressed do
			(
				if ChildrenArray.count != 0 do
					(
						s = Children.selection
						CList = Children.items
						deleteItem ChildrenArray s
						deleteItem CList s
						Children.items = CList
					)
			)

		-- ******************************************************************************	
		-- * Add Child
		-- ******************************************************************************	
		on AddChild pressed do
			(
				CObjArray = for obj in selection collect obj
				ierror = false
				serror = false
				-- Check for instancing in potential child objects
				for i = 1 to CObjarray.count do
					(
						CName = (KeyName CObjArray[i].name)
						for j = 1 to CObjArray.count do
							if i != j do
								if (KeyName CObjArray[j].name) == CName do
									ierror = true
					)
				if iError == false then
					(
						for i = 1 to CObjArray.count do
							if (avgScale CObjArray[i].scale) != 1.0 do
								(
									-- MessageBox ("Object \"" + CObjArray[i].name + " is scaled.  Please make sure it's scale is 100% and try again.")
									-- sError = true
									Exit
								)
						if sError == false then
							(
								CList = Children.items
								for i = 1 to CObjArray.count do
									(
										obj = CObjArray[i]
										if (findItem ChildrenArray obj == 0) do
											(
												append ChildrenArray obj
												append CList obj.name
											)
									)
								Children.items = CList
							)
					) else (
						MessageBox "Instancing was found in your object selection.\n You cannot instance child objects.\nPlease rename your objects without instancing."
					)
			)

		-- ******************************************************************************	
		-- ******************************************************************************	
		-- * 
		-- * 														Create Relationship
		-- * 
		-- ******************************************************************************	
		-- ******************************************************************************	
		on CreateRelationship pressed do
			(
				if (CParent != undefined) and (ChildrenArray.count != 0) then
					(
						ParentIndex = 0
						ChildIndices = #()
						-- Test for the existence of the child and parent in the data structure
						for i = 1 to RData.count do
							(
								-- Test for the existence of the PARENT object in Rdata#()
								if RData[i][1] == CParent.name do
									(
										if debug do format "ParentIndex found at: %\n" i
										ParentIndex = i
									)
							)
						for i = 1 to ChildrenArray.count do
							(
								CChild = ChildrenArray[i]
								ChildIndices[i] = 0
								for j = 1 to RData.count do
									(
										-- Test for the existence of the CHILD object in Rdata#()
										if RData[j][1] == CChild.name then
											(
												if debug do format "ChildIndex found at: %\n" j
												ChildIndices[i] = j
												Exit
											)
									)
							)
						if debug do format "ChildIndices:%\n" ChildIndices
						-- Process the Parent Object
						if ParentIndex != 0 then
							(
								FinalParentPosition = ParentIndex
								PData = RData[ParentIndex]
								-- if it's in the list, make sure it's a parent, it could be #unused
								RData[ParentIndex][2] = #Parent
								-- set the object type as a child if it has a parent object defined
								if PData[4] != 0 do PData[2] = #Child
								if debug do format "CI:%\n" ChildIndices
								for i = 1 to ChildIndices.count do
									(
										if (ChildIndices[i] == 0) do
											append PData[3] (Rdata.count + i)
										if (ChildIndices[i] != 0) and (finditem PData[3] ChildIndices[i]) == 0 do
											append PData[3] ChildIndices[i]
									)
							) else (
								-- is the parent a child of another parent?
								PData = #()
								PData[1] = (KeyName CParent.name)	-- Name
								PData[2] = #Parent 									-- Type
								PData[3] = #()											-- children it has
								AlreadyThereCount = 0
								for i = 1 to ChildIndices.count do
									if ChildIndices[i] == 0 then
										append PData[3] (RData.count + (i + 1) - AlreadyThereCount)
									else
										(
											append PData[3] ChildIndices[i]
											AlreadyThereCount += 1
										)
								PData[4] = 0 												-- it's new position in RData
								PData[5] = (avgScale CParent.scale)	-- Scale
								PData[6] = #Null										-- Initial Vector
								PData[7] = #Null										-- Initial Offset Rotation
								PData[8] = ScaleChildren.checked		-- Object definition
								PData[9] = CParent									-- Object definition
								-- add PData onto RData
								append Rdata PData
								FinalParentPosition = RData.count
							)

						for i = 1 to ChildrenArray.count do
							(
								CChild = ChildrenArray[i]
								-- Process the Child Object
								if ChildIndices[i] != 0 and ChildIndices[i] != undefined then
									(
										if debug do format "Already Have Child...\n"
										CData = RData[ChildIndices[i]]
										if (CIndex = FindItem RData[CData[4]][3] ChildIndices[i]) != 0 do
											DeleteItem RData[CData[4]][3] CIndex
										CData[4] = FinalParentPosition
										CData[5] = (avgScale CChild.scale) / (avgScale CParent.scale)
										CData[6] = (CChild.pos - CParent.pos) * (CParent.rotation) as matrix3
										CData[7] = CChild.rotation - CParent.rotation
										CData[8] = ScaleChildren.checked
										CData[9] = CChild
										RData[ChildIndices[i]] = CData
									) else (
										CData = #()
										CData[1] = KeyName CChild.name
										CData[2] = #Child
										CData[3] = #()
										CData[4] = FinalParentPosition
										CData[5] = (avgScale CChild.scale) / (avgScale CParent.scale)
										CData[6] = (CChild.pos - CParent.pos) * (CParent.rotation) as matrix3
										CData[7] = CChild.rotation - CParent.rotation
										CData[8] = ScaleChildren.checked
										CData[9] = CChild
										-- add PData onto RData
										append Rdata CData
									)
							)

						-- check for objects that are flagged as #parent and if they have no children, flag them as unused
						for i = 1 to RData.count do
							if (RData[i][2] == #Parent) and (RData[i][3].count == 0) do
								RData[i][2] = #Unused

						NameArray = #()
						ObjArray = #()
						if debug do
							for i = 1 to Rdata.count do
								format "%:%\n" i Rdata[i]
						for i = 1 to RData.count do
							if RData[i][2] == #Parent then
								BuildNameList i RData NameArray 0 ObjArray
						ObjectNames.items = NameArray

						if AutoClear.checked do
							(
								ChildrenArray = #()
								CList = #()
								Children.items = CList
								CParent = undefined
								ChooseParent.text = ""
							)
						SaveNow.text = "SAVE YOUR FILE!!!!"
					) else (
						messageBox "Objects are undefined."
					)
			)

		-- *************************************************
		-- * Refresh Selection
		-- *************************************************
		on RefreshSelection pressed do
			(
				for i = 1 to RData.count do
					(
						if (CObj = FindByName RData[i][1]) != undefined then
							RData[i][9] = CObj
						else (
							RData[i][9] = #NoObject
							if debug do format "Parent found in RData, but no object found in the scene for %\n" RData[i][1]
						)
					)
				ParentArray = #()
				ParentNames = #()
				for i = 1 to RData.count do
					if RData[i][2] == #Parent and RData[i][9] != #NoObject do
						(
							if debug do format "Found Parent at %\n" i
							append ParentArray #(RData[i][9], i)
							append ParentNames (KeyName RData[i][1])
						)
				ObjArray = selection as array
				oc = ObjArray.count
				ProgressStart "Refreshing Objects..."
				for i = 1 to oc do
					(
						if ProgressUpdate ((i / oc as float) * 100) == false then Exit
						CObj = ObjArray[i]
						if debug do format "CObj: %\n" CObj.name
						-- Is it a parent Object?
						if (PIndex = findItem ParentNames (KeyName CObj.name)) != 0 do
							(
								if debug do format "calling recursive function at RData Parent Index %....\n" ParentArray[PIndex][2]

								RDataParentIndex = ParentArray[PIndex][2]
								if debug do format "RDataParent Index: %\n" RDataParentIndex

								-- Do we need to scale the children
								if RData[RDataParentIndex][8] == true then
									SChildren = true
								else
									SChildren = false

								UpdateSelection RDataParentIndex RData CObj RData[RDataParentIndex][5] SChildren
							)
					)
				ProgressEnd()
			)
		-- *************************************************
		-- * Refresh objects
		-- *************************************************
		on refreshObjects pressed do
			(
				for i = 1 to RData.count do
					(
						if (CObj = FindByName RData[i][1]) != undefined then
							RData[i][9] = CObj
						else
							RData[i][9] = #NoObject
					)
				for i = 1 to RData.count do
					if RData[i][2] == #Parent then
						UpdatePositions i RData
			)
	) -- End Utility