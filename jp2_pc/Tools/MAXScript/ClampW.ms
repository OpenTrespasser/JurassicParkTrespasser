fn ClampWfn CObj = 
	(
		for j = 1 to CObj.numfaces do
			(
				CFace = (GetTVFace cObj j)
				c1 = getTvert CObj CFace.x
				c2 = getTvert CObj CFace.y
				c3 = getTvert CObj CFace.z
				if c1.z >= 0.0 and c1.z <= 0.0001 do SetTVert CObj CFace.x [c1.x, c1.y, 1]
				if c2.z >= 0.0 and c2.z <= 0.0001 do SetTVert CObj CFace.y [c2.x, c2.y, 1]
				if c3.z >= 0.0 and c3.z <= 0.0001 do SetTVert CObj CFace.z [c3.x, c3.y, 1]
			)
		update CObj
	)

Utility ClampW "Clamp W"
	(
		button ClampSelection "Clamp Selection"

		on ClampSelection pressed do
			(
				Objarray = for obj in selection collect obj
				for i = 1 to ObjArray.count do ClampWfn Objarray[i]
			)
	)
