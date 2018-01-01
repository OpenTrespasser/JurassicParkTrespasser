Utility ObjHeightFlag "Sel by Obj Height"
(

group "Options"
(
	spinner ObjHeight "Height:" range:[0,100,15] type:#float
)

button SelObjects "Select Objects"

on SelObjects pressed do
(
	MAX select None
	for o in objects do
	(
		if (o.max.z - o.min.z) > ObjHeight.value do selectmore o
	)
)


)