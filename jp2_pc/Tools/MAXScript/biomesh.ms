-- **********************************************************************************************
-- *
-- * Copyright © DreamWorks Interactive, 1997
-- *
-- * Contents:
-- *
-- * Bugs:
-- *
-- * To do:
-- *
-- * Notes:
-- *
-- **********************************************************************************************
-- string Class = "Player";

-- int NumJoints = 19;

-- int PVA = 3;


-- detects stray vertices in the mesh
fn hasStrayVerts CObj =
	(
		ConvertToMesh CObj
		StrayVertArray = (for i = 1 to CObj.numverts collect i)
		UsedVerts = #()
		NewVertSelection = #()

		for i = 1 to CObj.numfaces do
			(
				CFace = (getface CObj i)
				if (findItem UsedVerts CFace.x) == 0 do (append UsedVerts CFace.x)
				if (findItem UsedVerts CFace.y) == 0 do (append UsedVerts CFace.y)
				if (findItem UsedVerts CFace.z) == 0 do (append UsedVerts CFace.z)
			)

		for i = 1 to StrayVertArray.count do
			if (findItem UsedVerts i) == 0 do (append NewVertSelection i)

		
		if NewVertSelection.count > 0 then
			(
				setvertselection CObj NewVertSelection
				updateCObj
				return true
			) else (
				return false
			)
	)

-- makes a copy of a string without it's references
fn CopyString str = 
	(
		NewString = ""
		for i = 1 to str.count do NewString += str[i]
		return NewString
	)

-- returns the start positions from the user Prop buffer of all #include statements
fn GetIncludePositions CBuffer = 
	(
		NBuffer = CopyString Cbuffer
		StartPositions = #()
		foundPos = true
		while foundPos do
			(
				if (StartPos = findString NBuffer "#include <") != undefined then
					(
						append StartPositions StartPos
						NBuffer = replace NBuffer StartPos 1 "@"
					) else (
						foundPos = false
					)
			)
		return StartPositions 
	)

-- returns an array of .oal #include objects
fn GetIncludes obj =
	(
		-- #include "[Object.name]"
		local IncludeObjects = #()
		local StartPos, EndPos
		CBuffer = GetUserPropBuffer obj
		if (StartPositions = GetIncludePositions CBuffer) != #() then
			(
				for i = 1 to StartPositions.count do
					(
						StartPos = StartPositions[i]
						EndPos = 0
						for i = (StartPos + 10) to Cbuffer.count do
							if CBuffer[i] == ">" do
								(
									EndPos = i
									Exit
								)
						-- print EndPos
						if EndPos > 0 do
							(
								local CName = (substring CBuffer (StartPos + 10) (EndPos - (StartPos + 10)))
								-- print CName
								for o in objects do
									if o.name == CName do
										(
											append IncludeObjects  o
											Exit
										)
							)
					)
			)
		return IncludeObjects
	)

-- returns the keyname of an object
fn KeyName CName = 
	(
		if classof CName == String then
			(
				local DashPos = -1
				for i = 1 to CName.count do
					if CName[i] == "-" do
						(
							DashPos = i
							Exit
						)

				if DashPos == -1 then
					(
						-- check and see if the object is an oal object
						if (iOAL = findString CName ".oal") != undefined then
							return (substring CName 1 (iOAL - 3))
						else
							return CName
					) else (
						return (substring CName 1 (DashPos - 1))
					)
			) else (
				return undefined
			)
	)

-- returns a cumlative props buffer
fn ReadProps obj = 
	(
		if (Objarray = (GetIncludes obj)) == #() then
			return (getUserPropBuffer obj)
		else
			(
				NBuffer = getUserPropBuffer obj
				for i = 1 to ObjArray.count do
					NBuffer += getUserPropBuffer ObjArray[i]
				return (PruneIncludes NBuffer)
			)
	)

-- removes the #include statements from a props buffer
fn PruneIncludes CBuffer =
	(
		if (StartPos = findString CBuffer "#include <") != undefined then
			(
				EndPos = 0
				for i = (StartPos + 10) to Cbuffer.count do
					if CBuffer[i] == ">" do
						(
							EndPos = i
							Exit
						)
				if EndPos > 0 do
					(
						Prefix = substring CBuffer 1 (StartPos - 1)
						Suffix = substring Cbuffer (EndPos + 1) (CBuffer.count - EndPos)
						CBuffer = Prefix + Suffix
					)
				-- recurse it
				PruneIncludes CBuffer
			) else (
				return CBuffer
			)
	)

fn MungePropsBuffer o kn = 
	(
		CBuffer = getUserPropBuffer o
		if Cbuffer.count > 500 then
			(
				ObjIndex = 1
				while (getUserPropBuffer o).count > 500 do
					(
						CBuffer = getUserPropBuffer o
						SplitIndex = 0
						for i = 400 to Cbuffer.count do
							if (substring CBuffer i 3) == "\r\r\n" do
								(
									SplitIndex = i + 3
									-- format "Split Index:%\n" SplitIndex
									Exit
								)
						Buffer1 = substring CBuffer 1 (SplitIndex - 1)
						Buffer2 = substring CBuffer SplitIndex (Cbuffer.count - SplitIndex + 1)
						h = hedra()
						h.radius = 0.05
						h.name = (kn + "_" + (ObjIndex as string) + ".oal")
						if h.name[1] == "$" do h.name = (substring h.name 2 (h.name.count - 1))
						h.pos = o.pos
						h.wirecolor = (color 0 255 155)
						setUserPropBuffer o (Buffer1 + "#include <" + h.name + ">\r\r\n")
						setUserPropBuffer h ("//OAL bject used by \"" + o.name + "\"\r\r\n" + Buffer2)
						ObjIndex += 1
						o = h
					)
				return true
			) else (
				return undefined
			)
	)

fn joint_Flt obj = findstring obj.name "$J" == 1

fn getVerts o = 
	(
		a = getVertSelection o
		b = for i = 1 to a.count collect (a[i] - 1)
		return b
	)

fn DoubleDigitNum num =
	(
		NumString = ""
		if num >= 0 and num <= 9 then
			numString = ("0" + (num as integer) as string)
		else
			numString = (num as integer) as string
		return NumString
	)

fn vertInside v mn mx = 
	(
		if v.x > mn.x and v.x < mx.x and v.y > mn.y and v.y < mx.y and v.z > mn.z and v.z < mx.z then
			return true
		else
			return false
	)

Utility BioMeshUtil "BioMesh"
	(
		local CBioMesh, AssignmentData, AssignedVerts, j1, j2, c
		
		
		-- the AssignmentData#() array is in the following structure:
		-- #(<Joint#>, <Joint#>, <Joint#>...)
		-- where <Joint#> = #(<vert#>, <vert#>, <vert#>, <vert#>...)

		group "BioMesh Initialization"
			(
				label BiomeshLabel "BioMesh:" align:#left offset:[-5,0]
				pickbutton ChooseBioMesh width:90 align:#right offset:[0,-20]
				radiobuttons BioMeshType "Class:" labels:#("Player", "CAnimal") align:#left columns:2
				dropdownList BioType "Physics Type:" items:#("Player", "Biped", "Quadped")
				spinner NumJoints "# Joints: " range:[1,40,19] type:#integer fieldwidth:35
				button InitilaizeBioMesh "Initilaize BioMesh" width:140
			)

		group "Bio Boxes"
			(
				pickButton CreateBoxFromVerts "Box from Vert Selection" width:140 filter:joint_Flt
				button AssignFromBioBoxes "Assign Scene BioBoxes" width:140
			)

		group "Assignments/Selections"
			(
				pickButton AssignSelection "Assign Selection" width:140 filter:joint_Flt
				pickbutton GetSelFromJoint "Get Sel From Joint" width:140 filter:joint_Flt
				button SelUnassigned "Select Unassigned Verts" width:140
			)

		group "Status"
			(
				label NumVerts "NumVerts:" align:#left
				Label VertsAssigned "Verts Assigned:" align:#left
				label VertsUnAssigned "Verts UnAssigned: " align:#left
			)

		button WritePropsToJoints "Write to Joints" width:140
		button MungeJoints "Munge Joints" width:140
		button ClearJoints "Clear Joints" width:140
		button writeProperties "Write Properties" width:140
		spinner PVA "PVA #: " type:#integer fieldwidth:45 range:[0,9999,0]
		spinner AssignUnassigned "Unassigned -> joint:" fieldwidth:35 type:#integer
		checkbox modProps "Write Props to BioMesh"
		button LoadBioMesh "Load" width:70 offset:[-37,0]
		button SaveBioMesh "Save" width:70 offset:[37,-26]

		group "Joint Influences"
			(
				label j1Label "Joint 1: " align:#left offset:[-5,0]
				pickbutton GetJoint1 align:#right width:100 offset:[5,-20]
				label j2Label "Joint 2: " align:#left offset:[-5,0]
				pickbutton GetJoint2 align:#right width:100 offset:[5,-20]
				label cjLabel "Center: " align:#left offset:[-5,0]
				pickbutton GetCenterJoint align:#right width:100 offset:[5,-20]
				spinner	Percentage "%: "
			)

		on ClearJoints pressed do
			(
				for o in objects do
					if ((findString o.name "$J") == 1) do
						setUserPropBuffer o ""
				d = #()
				for o in objects do
					if ((findString o.name "J" == 1) and (findString o.name ".oal" != 0)) do
						append d o
				delete d
			)

		on GetJoint1 picked obj do
			(
				j1 = obj
				GetJoint1.text = j1.name
			)

		on GetJoint2 picked obj do
			(
				j2 = obj
				GetJoint2.text = j2.name
			)

		on GetCenterJoint picked obj do
			(
				c = obj
				GetCenterJoint.text = c.name
			)

		on Percentage changed state do
			(
				if j1 != undefined and j2 != undefined and c != undefined do
					(
						v = normalize (j2.pos - j1.pos)
						p = Percentage.value * 0.01
						d = (distance j2.pos j1.pos) * p
						c.pos = (j1.pos + v * d)
					)
			)

		on MungeJoints pressed do
			(
				j = #()
				for o in objects do
					if findString o.name "$J" == 1 do
						append j o
				if j.count > 0 do
					for i = 1 to j.count do
						(
							kn = (KeyName j[i].name)
							MungePropsBuffer j[i] kn
						)
			)
		-- ************************************************************************************************
		-- * generate the vertex assignments from !BioBoxes
		-- ************************************************************************************************
		on AssignFromBioBoxes pressed do
			(
				bioBoxes = #()
				-- identify the bioboxes from the scene
				for o in objects do
					if o.name.count >= 7 do
						if (substring o.name 1 7) == "!BioBox" do
							append bioBoxes o
				if bioBoxes.count > 0 do
					(
						-- format "% bio boxes located...\n" bioBoxes.count
						for i = 1 to bioBoxes.count do
							(
								CJointNum = (substring bioboxes[i].name (bioBoxes[i].name.count - 1) 2) as integer
								s = #()
								minBox = bioBoxes[i].min
								maxBox = bioBoxes[i].max
								for j = 1 to CBioMesh.numverts do
									(
										CVert = getVert CBiomesh j
										if (vertInside CVert minBox maxBox) do
											(
												AssignedVerts[j] = true
												if (FindItem AssignmentData[CJointNum + 1] j) == 0 do
													append AssignmentData[CJointNum + 1] j
												-- remove any vert assignments from other joints that may
												for k = 1 to AssignmentData.count do
													if k != (CJointNum + 1) do
														if (ItemIndex = finditem AssignmentData[k] j) != 0 do
															deleteItem AssignmentData[k] ItemIndex
											)
									)
							)
						NumAssigned = 0
						for i = 1 to AssignmentData.count do
							NumAssigned = NumAssigned + AssignmentData[i].count

						NumVerts.text = ("NumVerts: " + CBioMesh.numverts as string)
						VertsAssigned.text = ("Verts Assigned: " + NumAssigned as string)
						VertsUnAssigned.text = ("Verts UnAssigned: " + (CBioMesh.numverts - NumAssigned) as string)
					)
			)

		-- ************************************************************************************************
		-- * Create a Box from a vertex selection
		-- ************************************************************************************************
		on CreateBoxFromVerts picked obj do
			(
				if CBiomesh != undefined do
					(
						sel = getVertSelection CBioMesh
						if sel.count > 0 do
							(
								CJointNum = (substring obj.name (Obj.name.count - 1) 2)
								format "%\n" CJointNum
								-- find the min and max vertices
								minPoint = getVert CBiomesh sel[1]
								maxPoint = getVert CBiomesh sel[1]
								for i = 1 to Sel.count do
									(
										CVert = getVert CBiomesh sel[i]
										if CVert.x > maxPoint.x do maxPoint.x = CVert.x
										if CVert.x < minPoint.x do minPoint.x = CVert.x
										if CVert.y > maxPoint.y do maxPoint.y = CVert.y
										if CVert.y < minPoint.y do minPoint.y = CVert.y
										if CVert.z > maxPoint.z do maxPoint.z = CVert.z
										if CVert.z < minPoint.z do minPoint.z = CVert.z
									)
								buffer = 0.01
								x = maxPoint.x - minPoint.x
								y = maxPoint.y - minPoint.y
								z = maxPoint.z - minPoint.z
								b = box()
								b.width = x + buffer
								b.length = y + buffer
								b.height = z + buffer
								b.pivot = b.center
								b.pos = (minPoint + maxPoint) / 2
								addModifier b (NormalModifier())
								b.modifiers[1].flip = true
								convertToMesh b
								b.wirecolor = (color 238 13 78)
								b.name = ("!BioBox" + CJointNum)
							)
					)
			)

		-- ************************************************************************************************
		-- * Write Properties to Joints
		-- ************************************************************************************************
		on WritePropsToJoints pressed do
			(
				if (AssignmentData.count > 0) and (AssignmentData.count != undefined) do
					(
						-- Write the user properties to the UserPropBuffer
						if modProps.checked do
							(
								if BioMeshType.state == 1 do
									ObjPropsString = ("string Class = \"Player\";\r\r\nint NumJoints = " + (NumJoints.value as integer) as string + ";\r\r\nint PVA = -2;")
		
								if BioMeshType.state == 2 do
									ObjPropsString = ("string Class = \"CAnimal\";\r\r\nint NumJoints = " + (NumJoints.value as integer) as string + ";\r\r\nint PVA = -2;")
		
								if BioMeshType.state == 3 do
									ObjPropsString = ("string Class = \"CInstance\";\r\r\nint NumJoints = " + (NumJoints.value as integer) as string + ";\r\r\nint PVA = -2;")
		
								setUserPropBuffer CBioMesh ObjPropsString
							)

						for i = 1 to AssignmentData.count do
							(
								CJoint = undefined
								-- Find the Joint
								for o in objects do
									if (findString o.name "$J") == 1 and (findstring o.name (DoubleDigitNum (i - 1))) != undefined do
										(
											CJoint = o
											Exit
										)

								if CJoint != undefined do
									(
										-- Setup the assignment data
										CPropsString = ""
										for j = 1 to AssignmentData[i].count do
											CPropsString = (CPropsString + "int A" + (DoubleDigitNum (j - 1)) + " = " + ((AssignmentData[i][j] - 1) as integer) as string + ";\r\r\n")
										SetUserPropBuffer CJoint CPropsString
									)
							)
					)
			)

		-- ************************************************************************************************
		-- * Set Selection to unassigned verts
		-- ************************************************************************************************
		on SelUnassigned pressed do
			(
				CSel = #()
				for i = 1 to AssignedVerts.count do
					if AssignedVerts[i] == false do
						append CSel i
			
				if CSel.count > 0 then
					(
						setvertselection CBioMesh CSel
						update CBioMesh
					) else (
						MessageBox "All Verts are assigned!"
					)
			)
		-- ************************************************************************************************
		-- * Get Vertex Selection from a picked joint
		-- ************************************************************************************************
		on GetSelFromJoint picked obj do
			(
				if (substring obj.name 1 2) == "$J" then
					(
						CJointNum = (substring obj.name (Obj.name.count - 1) 2) as integer
						SetVertSelection CBioMesh AssignmentData[CJointNum + 1]
						update CBioMesh
					) else (
						MessageBox "Not a valid joint!"
					)
			)

		-- ************************************************************************************************
		-- * Write Properties
		-- ************************************************************************************************
		on writeProperties pressed do
			(
				if (AssignmentData.count > 0) and (AssignmentData.count != undefined) do
					(
						-- Write the user properties to the UserPropBuffer
							-- determine it
							if modProps.checked do
								(
									if BioMeshType.state == 1 do
										ObjPropsString = ("string Class = \"Player\";\r\r\nint NumJoints = " + (NumJoints.value as integer) as string + ";\r\r\nint PVA = " + (PVA.value as integer) as string + ";")
			
									if BioMeshType.state == 2 do
										ObjPropsString = ("string Class = \"CAnimal\";\r\r\nint NumJoints = " + (NumJoints.value as integer) as string + ";\r\r\nint PVA = " + (PVA.value as integer) as string + ";")
			
									if BioMeshType.state == 3 do
										ObjPropsString = ("string Class = \"CInstance\";\r\r\nint NumJoints = " + (NumJoints.value as integer) as string + ";\r\r\nint PVA = " + (PVA.value as integer) as string + ";")
						
									-- Determine the biomesh type
									if BioType.selection == 1 do TypeString = "Player"
									if BioType.selection == 2 do TypeString = "Raptor"
									if BioType.selection == 3 do TypeString = "TRex"
			
									ObjPropsString = ObjPropsString + ("\r\r\nstring Type = \"" + TypeString + "\";")
		
									-- write it
									setUserPropBuffer CBioMesh ObjPropsString
								)

						format "AlwaysAssert(pau_joint_links.uLen == %);\n" CBioMesh.numverts
						for i = 1 to AssignedVerts.count do
							if AssignedVerts[i] == false do
								format "pau_joint_links[%] = %;\n" (i - 1) ((AssignUnassigned.value as integer) as string)

						for i = 1 to AssignmentData.count do
							for j = 1 to AssignmentData[i].count do
								format "pau_joint_links[%] = %;\n" (AssignmentData[i][j] - 1) (i - 1)
					)
			)

		-- ************************************************************************************************
		-- * BioMesh Initialization
		-- ************************************************************************************************

		on InitilaizeBioMesh pressed do
			(
				if CBioMesh != undefined then
					(
						KName = substring CBioMesh.name 1 (CBioMesh.name.count - 3)
						AssignedVerts = #()
						for i = 1 to CBioMesh.numverts do
							AssignedVerts[i] = false

						AssignmentData = #()
						for i = 1 to NumJoints.value do append AssignmentData #()
						-- messageBox ("Initializations:\nBioMesh: " + CBioMesh.name + "\nJoints: " + AssignmentData.count as string)
						NumJoints.enabled = InitilaizeBioMesh.enabled = BiomeshLabel.enabled = ChooseBioMesh.enabled = BioMeshType.enabled = BioType.enabled = false

					)
				else
					MessageBox "BioMesh not defined!"
			)

		-- ************************************************************************************************
		-- * Interface Manipulation
		-- ************************************************************************************************

--		on BioType selected s do
--			(
--				if s == 1 do NumJoints.value = 19
--				if s == 2 do NumJoints.value = 21
--				if s == 3 do NumJoints.value = 29
--			)

		on JointNum changed state do if JointNum.value > NumJoints.value do JointNum.value = NumJoints.value

		-- ************************************************************************************************
		-- * Load BioMesh
		-- ************************************************************************************************
		on LoadBioMesh pressed do
			(
				if (OpenFilename = getOpenFileName caption:"Choose BioMesh File:") != undefined do
					(
						f = OpenFile OpenFilename
						CLine = readline f
						if CLine[CLine.count] == "\n" then CLine = replace CLine CLine.count 1 ""
						loadMAXFile CLine
						CLine = readLine f
						if CLine[CLine.count] == "\n" then CLine = replace CLine CLine.count 1 ""
						nv = 0
						for o in objects do
							if o.name == CLine do
								(
									ChooseBioMesh.text = o.name
									nv = o.numverts
									CBioMesh = o
								)
						AssignmentData = #()
						AssignedVerts = #()
						nj = readvalue f
						for i = 1 to nj do
							AssignmentData[i] = #()
						
						if nv != 0 do
							for i = 1 to nv do
								append AssignedVerts false
								
						while not eof f do
							(
								cj = readvalue f
								cv = readvalue f
								append AssignmentData[cj] cv
								AssignedVerts[cv] = true
							)
						close f
						NumJoints.enabled = InitilaizeBioMesh.enabled = false
					)
			)

		-- ************************************************************************************************
		-- * Save BioMesh
		-- ************************************************************************************************
		on SaveBioMesh pressed do
			(
				if CBioMesh != undefined then
					(
						if (Save_Filename = getsavefilename caption:"Choose BioMesh File [.BM]") != undefined do
							(
								f = createfile Save_Filename
								format "%%\n" MAXFilePath MAXFilename to:f
								format "%\n" CBioMesh.name to:f
								format "%\n" (NumJoints.value as integer) to:f
								for i = 1 to AssignmentData.count do 
									for j = 1 to AssignmentData[i].count do
										if j != 0 do
											format "% %\n" i AssignmentData[i][j] to:f
								close f
							)
					)
				else
					MessageBox "No BioMesh defined!"
			)

		-- ************************************************************************************************
		-- * Assign Selection
		-- ************************************************************************************************
		on AssignSelection picked obj do
			(
				if (substring obj.name 1 2) == "$J" then
					(
						-- get the joint number
						CJointNum = (substring obj.name (Obj.name.count - 1) 2) as integer
						-- get the current sub-object vertex seelction from the biomesh
						CVertSelection = (getvertSelection CBioMesh)
						for i = 1 to CVertSelection.count do
							(
								AssignedVerts[CVertSelection[i]] = true
								if (FindItem AssignmentData[CJointNum + 1] CVertSelection[i]) == 0 do
									append AssignmentData[CJointNum + 1] CVertSelection[i]
							)
						-- remove any vert assignments from other joints that may
						for i = 1 to CVertSelection.count do
							(
								cvi = CVertSelection[i]
								for j = 1 to AssignmentData.count do
									if j != (CJointNum + 1) do
										if (ItemIndex = finditem AssignmentData[j] cvi) != 0 do
											deleteItem AssignmentData[j] ItemIndex
							)

						-- Update the Interface
						NumAssigned = 0
						for i = 1 to AssignmentData.count do
							NumAssigned = NumAssigned + AssignmentData[i].count

						NumVerts.text = ("NumVerts: " + CBioMesh.numverts as string)
						VertsAssigned.text = ("Verts Assigned: " + NumAssigned as string)
						VertsUnAssigned.text = ("Verts UnAssigned: " + (CBioMesh.numverts - NumAssigned) as string)
						
					) else (
						MessageBox "Not a valid joint"
					)
			)

		-- ************************************************************************************************
		-- * Choose BioMesh
		-- ************************************************************************************************
		on ChooseBioMesh picked obj do
			(
				if ((hasStrayVerts obj) == false) then
					(
						if classOf obj == editable_mesh and obj.modifiers.count == 0 then
							(
								CBioMesh = obj
								ChooseBioMesh.text = CBioMesh.name
								NumVerts.text = ("NumVerts: " + CBioMesh.numverts as string)
								VertsAssigned.text = "Verts Assigned: 0"
								VertsUnAssigned.text = ("Verts UnAssigned: " + CBioMesh.numverts as string)
								NumJoints.enabled = InitilaizeBioMesh.enabled = true
								cnt = 0
								for o in objects do
									if (findString o.name ("$J" + CBioMesh.name)) != undefined do
										cnt += 1
								NumJoints.value = cnt
							) else (
								messageBox "Needs a collapsed\neditable mesh object."
							)
					) else (
						messageBox "Stray verts detected!\n They are selected"
					)
			)
	) -- End Utility
