Utility UVClamp "UV Clamp"
(
button ClampSelected "Clamp Selected"

on ClampSelected pressed do
	(
		ObjArray = for obj in selection collect obj
		for o in ObjArray do
			(
				nf = o.numfaces
				TFaces = for i = 1 to nf collect (gettvface o i)
				Tverts = #()
				for i = 1 to nf do
					(
						if finditem Tverts Tfaces[i].x == 0 do (append Tverts Tfaces[i].x)
						if finditem Tverts Tfaces[i].y == 0 do (append Tverts Tfaces[i].y)
						if finditem Tverts Tfaces[i].z == 0 do (append Tverts Tfaces[i].z)
					)
				for i = 1 to Tverts.count do
					(
						CTvert = gettvert o Tverts[i]
						if CTvert.x > 1.0 do setTvert o Tverts[i] [1.0,CTVert.y, CTVert.z]
						if CTvert.x < 0.0 do setTvert o Tverts[i] [0.0,CTVert.y, CTVert.z]
						if CTvert.y > 1.0 do setTvert o Tverts[i] [CTVert.x, 1.0, CTVert.z]
						if CTvert.y < 0.0 do setTvert o Tverts[i] [CTVert.x, 0.0, CTVert.z]
					)
				update o
			)
	)

)