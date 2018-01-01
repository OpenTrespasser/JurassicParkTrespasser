Utility trnObjCuber "Cuber"
(
button cubeMe "Cube Terrain Objects"

on CubeMe pressed do
(
	ObjArray = for obj in selection collect obj
	oc = ObjArray.count
	for i = 1 to oc do
	(
		obj = ObjArray[i]
		ConvertToMesh obj
		HeightOffset = (obj.max.x - obj.min.x)
		VertPos = (Getvert obj 1)
		setvert obj 1 [VertPos.x, VertPos.y,VertPos.z + HeightOffset]
		update obj
	)
)
)