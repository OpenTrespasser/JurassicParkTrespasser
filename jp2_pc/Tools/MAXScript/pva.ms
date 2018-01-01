-- **********************************************************************************************
-- *
-- * Copyright © DreamWorks Interactive, 1997
-- *
-- * Contents:
-- *		Implementation of PVA.ms 
-- *
-- * Bugs:
-- *
-- * To do:
-- *
-- * Notes:
-- *			PVA is a tool for assigning selections of vertices to a joint inside 3DSMAX.  
-- *			After the Vertex/Joint assignments are made, the user may output them as a block 
-- *			of C++ code for implementation.
-- *
-- **********************************************************************************************

global BioMesh, BioMeshVertexArray, JointData, NumJoints

Utility PVA "PVA"
(

local CurrentJoint, CurrentJointButton

on PVA open do
(
	CurrentJointButton = 0
)

bitmap pvatitle filename:"\\\JPII_PC\\Trespass\Art\\Tools\Scripts\pva.bmp" offset:[-12,0]

group "Model Presets"
(
button AnnePreset "Anne" offset:[-50,0] enabled:false
button RaptorPreset "Raptor" offset:[0,-26] enabled:false
button TrexPreset "T-Rex" offset:[50,-26] enabled:false
)

group "BioMesh Ops"
(
button loadPVAFile "Load PVA File" width:140
button SavePVAFile "Save PVA File" width:140 enabled:false
pickbutton CreateDataObject "Choose BioMesh" width:140
button PurgeBioMeshData "Purge BioMesh Data" width:140
)

group "Vertex/Joint Assignments"
(
label BioMeshLabel2 ""
label JointLabel ">----------------- Joints -----------------<"
checkbutton joint0 "0" offset:[-60,0] width:29
checkbutton joint1 "1" offset:[-30,-26] width:29
checkbutton joint2 "2" offset:[0,-26] width:29
checkbutton joint3"3" offset:[30,-26] width:29
checkbutton joint4 "4" offset:[60,-26] width:29
checkbutton joint5 "5" offset:[-60,0] width:29
checkbutton joint6 "6" offset:[-30,-26] width:29
checkbutton joint7 "7" offset:[0,-26] width:29
checkbutton joint8 "8" offset:[30,-26] width:29
checkbutton joint9 "9" offset:[60,-26] width:29
checkbutton joint10 "10" offset:[-60,0] width:29
checkbutton joint11 "11" offset:[-30,-26] width:29
checkbutton joint12 "12" offset:[0,-26] width:29
checkbutton joint13 "13" offset:[30,-26] width:29
checkbutton joint14 "14" offset:[60,-26] width:29
checkbutton joint15 "15" offset:[-60,0] width:29
checkbutton joint16 "16" offset:[-30,-26] width:29
checkbutton joint17 "17" offset:[0,-26] width:29
checkbutton joint18 "18" offset:[30,-26] width:29
checkbutton joint19 "19" offset:[60,-26] width:29
label updatemethodlabel "Update Method:" align:#left
radiobuttons UpdateMethod labels:#("Add", "Subtract", "Replace") offset:[-40,0] default:1
button UpdateVertAssignment "Update" offset:[35,-35] enabled:false
label filler ""
)

group "Selection Tools"
(
button SelectUnassigned "Verts Not Assigned" width:140 enabled:false
button SelectAssigned "Assigned to Current Joint" width:140 enabled:false
)

group "Export BioMesh Data"
(
button CodeToListener "Generate Code to Listener" width:140 enabled:false
button CodeToFile "Generate Code To File" width:140 enabled:false
)

-- ***********************************************************************************************************************
-- Purge BioMeshData
-- ***********************************************************************************************************************
on PurgeBiomeshData pressed do
(
BioMesh = undefined
BioMeshVertexArray = undefined
JointData = undefined
NumJoints = undefined
format "Global Data Purged\n"
)

-- ***********************************************************************************************************************
-- Select Unassigned Verts
-- ***********************************************************************************************************************
on selectUnassigned pressed do
(
	setvertselection BioMesh #()
	for i = 1 to BioMeshVertexArray.count do
	(
		if BioMeshVertexArray[i] == false do
		(
			setvertselection BioMesh #(i) keep:true
		)		
	)
	
	update BioMesh
)

-- ***********************************************************************************************************************
-- Select Assigned Verts
-- ***********************************************************************************************************************
on selectAssigned pressed do
(
--	setvertselection BioMesh #()
	setvertselection BioMesh JointData[CurrentJointButton +1]
	update BioMesh
)

-- ***********************************************************************************************************************
-- Update Vertex Assignments
-- ***********************************************************************************************************************

on UpdateVertAssignment pressed do
(

-- adding verts to Joints
if UpdateMethod.state == 1 do
(
	NewVertSelection = GetVertSelection BioMesh

-- First, we have to remove selected verts from the assignment array
	delItemCount = 0
	for i = 1 to NewVertSelection.count do
	(
		cvi = NewVertSelection[i]
		for j = 1 to JointData.count do
		(
			ItemIndex = finditem JointData[j] cvi
			if ItemIndex != 0 do
				(
					deleteItem JointData[j] ItemIndex
					delItemCount = delItemCount + 1
					BioMeshVertexArray[cvi] = true
				)
		)
	)
	format "  % Vertices removed from assignment\n" delItemCount

-- Now we add the selected vertex indices to the assignment array
	for i = 1 to NewVertSelection.count do
	(
		cvi = NewVertSelection[i]
		append JointData[CurrentJointButton +1] NewVertSelection[i]
		BioMeshVertexArray[cvi] = true
	)
		format "% Vertices Added to Joint %\n" NewVertSelection.count CurrentJointButton
)	

-- subtracting verts from Joints
if UpdateMethod.state == 2 do
(
	NewVertSelection = GetVertSelection BioMesh
	subcount = 0
	for i = 1 to NewVertSelection.count do
	(
		cvi = NewVertSelection[i]
		ItemIndex = finditem JointData[CurrentJointButton+1] cvi
		if ItemIndex != 0 do
		(
			deleteitem JointData[CurrentJointButton + 1] ItemIndex
			BioMeshVertexArray[cvi] = false
			subcount = subcount + 1
		)
	)
	format "% Vertices Subtracted from Joint %\n" subcount CurrentJointButton
)

-- replacing
if UpdateMethod.state == 3 do
(

-- First, we gots to wipe out any assignment data for the currently selected joint

	while JointData[CurrentJointButton + 1].count > 0 do
	(
		cvi = (JointData[CurrentJointButton + 1])[1]
		BioMeshVertexArray[cvi] = false
		deleteitem JointData[CurrentJointButton + 1] 1
	)
	JointData[CurrentJointButton + 1] = #()

-- Then we do that thingy about assigning verts to the currently selected joint

	NewVertSelection = GetVertSelection BioMesh
	for i = 1 to NewVertSelection.count do
	(
		append JointData[CurrentJointButton + 1] NewVertSelection[i]
		cvi = NewVertSelection[i]
		BioMeshVertexArray[cvi] = true
	)
	format "% Vertices Replaced for Joint %\n" NewVertSelection.count (CurrentJointButton + 1)
)
)

-- ***********************************************************************************************************************
-- BioMesh Presets
-- ***********************************************************************************************************************

on AnnePreset pressed do (NumJoints_spn.value = 19)
on RaptorPreset pressed do (NumJoints_spn.value = 20)
on TrexPreset pressed do (NumJoints_spn.value = 20)

-- ***********************************************************************************************************************
-- Load PVA File
-- ***********************************************************************************************************************

on loadPVAFile pressed do
(
if (Open_Filename = getOpenfilename caption:"Choose Physics Vertex Assignment File [.PVA]" filename:"*.pva") != undefined do
	(
f = openfile Open_Filename
BioMeshFilename = readline f
if BioMeshFilename[BioMeshFilename.count] == "\n" then BioMeshFilename = replace BioMeshFilename BioMeshFilename.count 1 ""
loadMAXFile BioMeshFilename
BioMeshName = readline f
if BioMeshName[BioMeshName.count] == "\n" then BioMeshName = replace BioMeshName BioMeshName.count 1 ""
format "\n"
format "BioMeshName from file: %\n" BioMeshName
BioMesh = undefined

for o in objects do
(
	if o.name == BioMeshName do 
	(
		BioMesh = o
	)
)
	
if BioMesh != undefined then 
(
	format "BioMesh % Initialized\n" BioMesh.name
	BioMeshLabel2.text = BioMesh.name
) else (
	format "BioMesh Not loaded properly\n"	
)

-- get BioMesh Vertex count from file
BioMeshVertCount = readvalue f
format "BioMesh Vertex Count: %\n" BioMeshVertCount 

-- get Number of joints from file
BioMeshNumJoints = readvalue f
format "BioMesh Joint Count: %\n" BioMeshNumJoints 

BioMeshVertexArray = (for i = 1 to BioMeshVertCount collect false)

-- initialize Joint Data Array
JointData = #()
for i = 1 to BioMeshNumJoints do
(
	JointData[i] = #()
)
	
-- start reading assignments
while not eof f do
(
	VertexIndex = readvalue f as integer
	JointIndex = readvalue f as integer
	if JointIndex >= 0 do
	(
		append JointData[JointIndex + 1] VertexIndex
		BioMeshVertexArray[VertexIndex] = true
--		format "Vert Index Read: %\n" VertexIndex
--		format "Joint Index Read: %\n" JointIndex
	)
)
close f
SelectUnassigned.enabled = true
UpdateVertAssignment.enabled = true
SelectAssigned.enabled = true
CodeToListener.enabled = true
CodeToFile.enabled = true
SavePVAFile.enabled = true
)
)

-- ***********************************************************************************************************************
-- Initialize from BioMesh
-- ***********************************************************************************************************************
on CreateDataObject picked obj do
(
DataObjs = (for obj in $'$D'* collect obj)
if DataObjs.count > 0 then
	(
		DataObject = DataObjs[1]
		PVAFileName = GetUserPropBuffer DataObject
		messagebox ("Please Initialize Mesh by loading thePVA File:\n" + PVAFileName)
		
	) else (

		Save_Filename = getsavefilename caption:"Choose Physics Vertex Assignment File [.PVA]"
		BioMesh = obj
		JointCount = 0
		ModelName = BioMesh.name
		for obj in $'$J'* do 
			(
				if substring obj.name 3 ModelName.Count == ModelName do selectmore obj
				JointCount = JointCount + 1 
			)
		h = hedra radius:0.100 mapcoords:true pos:BioMesh.pos name:("$D" + Modelname)
		ConvertToMesh h
		f = createfile Save_Filename
		format "%%\n" MAXFilePath MAXFilename to:f
		format "%\n" ModelName to:f
		format "%\n" BioMesh.numverts to:f
		format "%\n" JointCount to:f
		for i = 1 to BioMesh.numverts do
			(
				format "% -1\n" i to:f
			)
		close f
		setUserPropBuffer h Save_Filename
		BioMeshLabel2.text = BioMesh.name
		SelectUnassigned.enabled = true
		UpdateVertAssignment.enabled = true
		SelectAssigned.enabled = true
		CodeToListener.enabled = true
		CodeToFile.enabled = true
		SavePVAFile.enabled = true
	)
)

-- ***********************************************************************************************************************
-- Save PVA File
-- ***********************************************************************************************************************

on savePVAFile pressed do
(
	if BioMesh == undefined then
		(
			MessageBox "No BioMesh defined.  Exiting..."

		) else (

			Save_Filename = getsavefilename caption:"Choose Physics Vertex Assignment File [.PVA]"
			JointCount = 0
			ModelName = BioMesh.name
			DataObjs = #()
			for obj in $'$J'* do 
				(
					if substring obj.name 3 ModelName.Count == ModelName do selectmore obj
					JointCount = JointCount + 1 
				)
			for obj in $'$D'* do 
				(
					DataObjs = (for obj in $'$D'* collect obj)
				)
			if DataObjs.count == 0 then
				(
					h = hedra radius:0.100 mapcoords:true pos:BioMesh.pos name:("$D" + Modelname)
					ConvertToMesh h
					f = createfile Save_Filename
					format "%%\n" MAXFilePath MAXFilename to:f
					format "%\n" ModelName to:f
					format "%\n" BioMesh.numverts to:f
					format "%\n" JointCount to:f
					for i = 1 to BioMeshVertexArray.count do
						(
						for j = 1 to JointData.count do
							(
							if finditem JointData[j] i != 0 do
								(
									format "% %\n" i (j - 1) to:f
								)
							)
						)
					close f
					setUserPropBuffer h Save_Filename
				) else (
					DataObject = DataObjs[1]				
					f = createfile Save_Filename
					format "%%\n" MAXFilePath MAXFilename to:f
					format "%\n" ModelName to:f
					format "%\n" BioMesh.numverts to:f
					format "%\n" JointCount to:f
					for i = 1 to BioMeshVertexArray.count do
						(
						for j = 1 to JointData.count do
							(
							if finditem JointData[j] i != 0 do
								(
									format "% %\n" i (j - 1) to:f
								)
							)
						)
					close f
					setUserPropBuffer DataObject Save_Filename
				)
		)
)

-- ***********************************************************************************************************************
-- Code To Listener Window
-- ***********************************************************************************************************************
on CodeToListener pressed do
(
	if JointData != undefined then
		(
			UnassignedVertexArray = #()
			for i = 1 to BioMeshVertexArray.count do
				(
					if BioMeshVertexArray[i] == false do
						(
							append UnassignedVertexArray i
						)
				)
			if UnassignedVertexArray.count > 0 then
				(
				format "Unassigned Vertex Count: %\n" UnassignedVertexArray.count
				if (QueryBox "Unassigned Vertices Exist!\n\nSet unassigned vertices as new vertex selection for biomesh?") == true do
					(
						SetVertSelection BioMesh #()
						SetVertSelection BioMesh UnassignedVertexArray
					)
			) else (
				NumJoints = JointData.count
				JointsString = ("int NumJoints = " + NumJoints as string + ";\r\r\n")
				for i = 1 to NumJoints do
					for j = 1 to JointData[i].count do
						(
							VertIndex = (JointData[i])[j] as string
							if VertIndex.count == 1 do (VertIndex = "00" + VertIndex)
							if VertIndex.count == 2 do (VertIndex = "0" + VertIndex)
							JointsString = (JointsString + ("int Vertex" + VertIndex + " = " + (i - 1) as string + ";\r\r\n"))
						)
				setUserPropBuffer BioMesh JointsString
			)
		) else (
			format "JointData Array is %\n" JointData
		)
)
-- ***********************************************************************************************************************
-- Code To File
-- ***********************************************************************************************************************

on CodeToFile pressed do
(
	if JointData != undefined then
		(
			UnassignedVertexArray = #()
			for i = 1 to BioMeshVertexArray.count do
				(
					if BioMeshVertexArray[i] == false do
						(
							append UnassignedVertexArray i
						)
				)
			if UnassignedVertexArray.count > 0 then
				(
				format "Unassigned Vertex Count: %\n" UnassignedVertexArray.count
				if (QueryBox "Unassigned Vertices Exist!\n\nSet unassigned vertices as new vertex selection for biomesh?") == true do
					(
						SetVertSelection BioMesh #()
						SetVertSelection BioMesh UnassignedVertexArray
					)
			) else (
			Save_Filename = getsavefilename caption:"Specify text file"
			f = createfile save_filename
			format "\n" to:f
			format "{\n" to:f
			NumJoints = JointData.count
			for i = 1 to NumJoints do
				(
					for j = 1 to JointData[i].count do
						(
							VertIndex = (JointData[i])[j]
							format "\tpau_joint_links[%] = %;\n" VertIndex (i - 1) to:f
						)
				)
			format "}\n" to:f
			format "\n" to:f
			close f
			)
		) else (
			format "JointData Array is %\n" JointData
		)
)


-- ********************************************************* Begin redundant Interface Code *********************************************************
on joint0 changed state do
(
if state == on do 
	(
	CurrentJointButton = 0
	joint1.state = off; joint2.state = off; joint3.state = off; joint4.state = off; joint5.state = off; joint6.state = off; joint7.state = off; joint8.state = off; joint9.state = off;
	joint10.state = off; joint11.state = off; joint12.state = off; joint13.state = off; joint14.state = off; joint15.state = off; joint16.state = off; joint17.state = off; joint18.state = off;
	joint19.state = off)
)

on joint1 changed state do
(
if state == on do 
	(
	CurrentJointButton = 1
	joint0.state = off; joint2.state = off; joint3.state = off; joint4.state = off; joint5.state = off; joint6.state = off; joint7.state = off; joint8.state = off; joint9.state = off;
	joint10.state = off; joint11.state = off; joint12.state = off; joint13.state = off; joint14.state = off; joint15.state = off; joint16.state = off; joint17.state = off; joint18.state = off;
	joint19.state = off)
)

on joint2 changed state do
(
if state == on do 
	(
	CurrentJointButton = 2
	joint0.state = off;joint1.state = off; joint3.state = off; joint4.state = off; joint5.state = off; joint6.state = off; joint7.state = off; joint8.state = off; joint9.state = off;
	joint10.state = off; joint11.state = off; joint12.state = off; joint13.state = off; joint14.state = off; joint15.state = off; joint16.state = off; joint17.state = off; joint18.state = off;
	joint19.state = off)
)

on joint3 changed state do
(
if state == on do 
	(
	CurrentJointButton = 3
	joint0.state = off;joint1.state = off; joint2.state = off; joint4.state = off; joint5.state = off; joint6.state = off; joint7.state = off; joint8.state = off; joint9.state = off;
	joint10.state = off; joint11.state = off; joint12.state = off; joint13.state = off; joint14.state = off; joint15.state = off; joint16.state = off; joint17.state = off; joint18.state = off;
	joint19.state = off)
)

on joint4 changed state do
(
if state == on do 
	(
	CurrentJointButton = 4
	joint0.state = off;joint1.state = off; joint2.state = off; joint3.state = off;joint5.state = off; joint6.state = off; joint7.state = off; joint8.state = off; joint9.state = off;
	joint10.state = off; joint11.state = off; joint12.state = off; joint13.state = off; joint14.state = off; joint15.state = off; joint16.state = off; joint17.state = off; joint18.state = off;
	joint19.state = off)
)

on joint5 changed state do
(
if state == on do 
	(
	CurrentJointButton = 5
	joint0.state = off;joint1.state = off; joint2.state = off; joint3.state = off; joint4.state = off; joint6.state = off; joint7.state = off; joint8.state = off; joint9.state = off;
	joint10.state = off; joint11.state = off; joint12.state = off; joint13.state = off; joint14.state = off; joint15.state = off; joint16.state = off; joint17.state = off; joint18.state = off;
	joint19.state = off)
)

on joint6 changed state do
(
if state == on do 
	(
	CurrentJointButton = 6
	joint0.state = off;joint1.state = off; joint2.state = off; joint3.state = off; joint4.state = off; joint5.state = off; joint7.state = off; joint8.state = off; joint9.state = off;
	joint10.state = off; joint11.state = off; joint12.state = off; joint13.state = off; joint14.state = off; joint15.state = off; joint16.state = off; joint17.state = off; joint18.state = off;
	joint19.state = off)
)

on joint7 changed state do
(
if state == on do 
	(
	CurrentJointButton = 7
	joint0.state = off;joint1.state = off; joint2.state = off; joint3.state = off; joint4.state = off; joint5.state = off; joint6.state = off; joint8.state = off; joint9.state = off;
	joint10.state = off; joint11.state = off; joint12.state = off; joint13.state = off; joint14.state = off; joint15.state = off; joint16.state = off; joint17.state = off; joint18.state = off;
	joint19.state = off)
)

on joint8 changed state do
(
if state == on do 
	(
	CurrentJointButton = 8
	joint0.state = off;joint1.state = off; joint2.state = off; joint3.state = off; joint4.state = off; joint5.state = off; joint6.state = off; joint7.state = off; joint9.state = off;
	joint10.state = off; joint11.state = off; joint12.state = off; joint13.state = off; joint14.state = off; joint15.state = off; joint16.state = off; joint17.state = off; joint18.state = off;
	joint19.state = off)
)

on joint9 changed state do
(
if state == on do 
	(
	CurrentJointButton = 9
	joint0.state = off;joint1.state = off; joint2.state = off; joint3.state = off; joint4.state = off; joint5.state = off; joint6.state = off; joint7.state = off; joint8.state = off;
	joint10.state = off; joint11.state = off; joint12.state = off; joint13.state = off; joint14.state = off; joint15.state = off; joint16.state = off; joint17.state = off; joint18.state = off;
	joint19.state = off)
)

on joint10 changed state do
(
if state == on do 
	(
	CurrentJointButton = 10
	joint0.state = off;joint1.state = off; joint2.state = off; joint3.state = off; joint4.state = off; joint5.state = off; joint6.state = off; joint7.state = off; joint8.state = off; joint9.state = off;
	joint11.state = off; joint12.state = off; joint13.state = off; joint14.state = off; joint15.state = off; joint16.state = off; joint17.state = off; joint18.state = off;
	joint19.state = off)
)

on joint11 changed state do
(
if state == on do 
	(
	CurrentJointButton = 11
	joint0.state = off;joint1.state = off; joint2.state = off; joint3.state = off; joint4.state = off; joint5.state = off; joint6.state = off; joint7.state = off; joint8.state = off; joint9.state = off;
	joint10.state = off;joint12.state = off; joint13.state = off; joint14.state = off; joint15.state = off; joint16.state = off; joint17.state = off; joint18.state = off;
	joint19.state = off)
)

on joint12 changed state do
(
if state == on do 
	(
	CurrentJointButton = 12
	joint0.state = off;joint1.state = off; joint2.state = off; joint3.state = off; joint4.state = off; joint5.state = off; joint6.state = off; joint7.state = off; joint8.state = off; joint9.state = off;
	joint10.state = off; joint11.state = off;joint13.state = off; joint14.state = off; joint15.state = off; joint16.state = off; joint17.state = off; joint18.state = off;
	joint19.state = off)
)

on joint13 changed state do
(
if state == on do 
	(
	CurrentJointButton = 13
	joint0.state = off;joint1.state = off; joint2.state = off; joint3.state = off; joint4.state = off; joint5.state = off; joint6.state = off; joint7.state = off; joint8.state = off; joint9.state = off;
	joint10.state = off; joint11.state = off; joint12.state = off; joint14.state = off; joint15.state = off; joint16.state = off; joint17.state = off; joint18.state = off;
	joint19.state = off)
)

on joint14 changed state do
(
if state == on do 
	(
	CurrentJointButton = 14
	joint0.state = off;joint1.state = off; joint2.state = off; joint3.state = off; joint4.state = off; joint5.state = off; joint6.state = off; joint7.state = off; joint8.state = off; joint9.state = off;
	joint10.state = off; joint11.state = off; joint12.state = off; joint13.state = off; joint15.state = off; joint16.state = off; joint17.state = off; joint18.state = off;
	joint19.state = off)
)

on joint15 changed state do
(
if state == on do 
	(
	CurrentJointButton = 15
	joint0.state = off;joint1.state = off; joint2.state = off; joint3.state = off; joint4.state = off; joint5.state = off; joint6.state = off; joint7.state = off; joint8.state = off; joint9.state = off;
	joint10.state = off; joint11.state = off; joint12.state = off; joint13.state = off; joint14.state = off;joint16.state = off; joint17.state = off; joint18.state = off;
	joint19.state = off)
)

on joint16 changed state do
(
if state == on do 
	(
	CurrentJointButton = 16
	joint0.state = off;joint1.state = off; joint2.state = off; joint3.state = off; joint4.state = off; joint5.state = off; joint6.state = off; joint7.state = off; joint8.state = off; joint9.state = off;
	joint10.state = off; joint11.state = off; joint12.state = off; joint13.state = off; joint14.state = off; joint15.state = off; joint17.state = off; joint18.state = off; joint19.state = off)
)

on joint17 changed state do
(
if state == on do 
	(
	CurrentJointButton = 17
	joint0.state = off;joint1.state = off; joint2.state = off; joint3.state = off; joint4.state = off; joint5.state = off; joint6.state = off; joint7.state = off; joint8.state = off; joint9.state = off;
	joint10.state = off; joint11.state = off; joint12.state = off; joint13.state = off; joint14.state = off; joint15.state = off; joint16.state = off; joint18.state = off; joint19.state = off)
)

on joint18 changed state do
(
if state == on do 
	(
	CurrentJointButton = 18
	joint0.state = off;joint1.state = off; joint2.state = off; joint3.state = off; joint4.state = off; joint5.state = off; joint6.state = off; joint7.state = off; joint8.state = off; joint9.state = off;
	joint10.state = off; joint11.state = off; joint12.state = off; joint13.state = off; joint14.state = off; joint15.state = off; joint16.state = off; joint17.state = off; joint19.state = off)
)

on joint19 changed state do
(
if state == on do 
	(
	CurrentJointButton = 19
	joint0.state = off;joint1.state = off; joint2.state = off; joint3.state = off; joint4.state = off; joint5.state = off; joint6.state = off; joint7.state = off; joint8.state = off; joint9.state = off;
	joint10.state = off; joint11.state = off; joint12.state = off; joint13.state = off; joint14.state = off; joint15.state = off; joint16.state = off; joint17.state = off; joint18.state = off)
)

)











