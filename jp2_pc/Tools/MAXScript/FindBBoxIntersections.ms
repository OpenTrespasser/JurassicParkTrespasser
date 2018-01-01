fn overlap a b =
(
	b.min.x < a.max.x and
	b.min.y < a.max.y and
	b.min.z < a.max.z and
	a.min.x < b.max.x and
	a.min.y < b.max.y and
	a.min.z < b.max.z
)

Utility FindBBoxIntersections "Find Intersections"
(

button ProcessSelected "Process Selected" width:140
label status1
on ProcessSelected pressed do
	(
	ObjArray = for obj in selection collect Obj
	oc = ObjArray.count
	MAX Select None
	for i = 1 to oc do
		(
		status1.text = (i as string + " of " + oc as string)
		CObj = ObjArray[i]
		for j = 1 to oc do
			(
				if i != j do
				(
					if (overlap CObj ObjArray[j]) do selectmore ObjArray[j]
				)
			)	
		)
	)
) -- end Utility