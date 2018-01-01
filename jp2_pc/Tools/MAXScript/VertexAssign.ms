Utility VertAssingments "Vertex Assign"
(

local PhysicsModel, CurrentJoint, JointData, NumJoints;

group "Physics Model Setup/Options"
(
pickbutton GetPhysicsModel "Pick Model" width:135
Spinner NumJoints_spn "# of Joints: " range: [2,30,2] type:#integer
CheckBox ClearVerts "Clear Vertex Selection?" checked:false
Button Initialize "Initialize" width:135
label intlabel "" align:#left
)

PickButton AssignToJoint "Assign Sel to Joint" width:135
Button writeData "Write Object Data" width:135

on GetPhysicsModel picked obj do
(
PhysicsModel = obj
GetPhysicsModel.text = PhysicsModel.name
if ClearVerts.checked == true do (setvertselection PhysicsModel #())
)

on Initialize pressed do
(
if PhysicsModel == undefined then(intlabel.text = "No Physics Model defined!")
else
(
JointData = #()
NumJoints = NumJoints_spn.value
for i = 1 to numJoints do (JointData[i] = #())
intlabel.text = (NumJoints as string + " Physics Joints Initialized!")
))

On AssignToJoint picked obj do
(
CurrentJoint = obj
JointName = CurrentJoint.name
JointNameLength = (CurrentJoint.name).count
if (substring CurrentJoint.name (JointNameLength - 1) 1 as integer) == 0 then
	(i = substring CurrentJoint.name JointNameLength 1 as integer)
else
	(i = substring CurrentJoint.name (JointNameLength - 1) 2 as integer)
JointData[i] = (GetVertSelection PhysicsModel)
)

on WriteData pressed do
(
format "object BioMesh\n"
format "{\n"
for i = 1 to NumJoints do
(
if i <= 9 do (CurrentJointNumber = ("0" + i as string))
if i >= 10 do (CurrentJointNumber = i as string)
format "\tobject Joint%\n" CurrentJointNumber
format "\t\t{\n"
CurrentVertSet = JointData[i]
	for k = 1 to CurrentVertSet.count do
	(format "\t\tint vert% = %\n" k CurrentVertSet[k])
format "\t\t}\n"
format "\n"
)
format "}\n"
)
)






