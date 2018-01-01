Utility TrnTexUtil "Terrain Texture Utility"
(

local PriorityLtr, i, NewHeight, NewWidth;

group "Terrain Obj Creation Options"
(
Spinner Rez "Rez per Meter:" range:[1,256,16] type:#integer
Spinner MapWidth "Map Width:" range:[2,256,32] type:#integer
Spinner MapHeight "Map Height:" range:[2,256,32] type:#integer
CheckBox ConvertFace "Convert To Mesh?" checked:true
)
button CreateTerrainObject "Create Terrain Object" width:140
label blank0 ""
group "TrnObj Name Options"
(
Label PLabel "Texture Object Priority"
label plabel2 "Low >------------------------------< Hi"
Radiobuttons TexturePriority labels:#("A", "B", "C", "D", "E") default:1
edittext Name_Suffix "Suffix:" Width:20 align:#left
)

button RenameSel "Rename Selected"

on RenameSel pressed do
(
i = 0
undo on 
(
for o in objects do 
(
if o.isselected do
(
i = (i +1)
if i >= 1 and i <= 9 do (object_count = ("0" + (i as string)))
if i >= 10 do (object_count = (i as string))

if TexturePriority.state == 1 do (PriorityLtr = "A")
if TexturePriority.state == 2 do (PriorityLtr = "B")
if TexturePriority.state == 3 do (PriorityLtr = "C")
if TexturePriority.state == 4 do (PriorityLtr = "D")
if TexturePriority.state == 5 do (PriorityLtr = "E")

suffix = Name_Suffix.text

o.name = ("TrnObj" + PriorityLtr + suffix + "-" + object_count)
))))

on CreateTerrainObject pressed do
(undo on (
NewHeight = MapHeight.value as float / Rez.value as float
NewWidth = MapWidth.value as float / Rez.value as float
nf = face height:NewHeight width:NewWidth pos:[0,0,0] texture_coordinates:1 face_type:0
if ConvertFace.checked == true do (convertToMesh nf)
))

)

