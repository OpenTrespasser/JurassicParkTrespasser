Utility TrnTexNaming "Terrain Texture Naming"
(

local PriorityLtr, i;
group "Texture Object Options"
(
Label PLabel "Texture Object Priority"
Radiobuttons TexturePriority labels:#("A", "B", "C", "D", "E", "F", "G", "H", "I", "J") default:1
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
if TexturePriority.state == 6 do (PriorityLtr = "F")
if TexturePriority.state == 7 do (PriorityLtr = "G")
if TexturePriority.state == 8 do (PriorityLtr = "H")
if TexturePriority.state == 9 do (PriorityLtr = "I")
if TexturePriority.state == 10 do (PriorityLtr = "J")

suffix = Name_Suffix.text

o.name = ("TrnObj" + PriorityLtr + suffix + "-" + object_count)
))))

)