fn MinMax foo a = 
	(
		mn = a[1]
		mx = a[1]
		for i = 2 to a.count do
			(
				if a[i] > mx do mx = a[i]
				if a[i] < mn do mn = a[i]
			)
		return #(mn, mx)
	)

fn detectStreakingFaces CObj = 
	(
		local fs = #(), thresh = 0.00390625, du, dv, vx, vy, vz
		for i = 1 to CObj.numfaces do
			(
				CTVFace = getTVFace CObj i
				vx = getTvert CObj CTVFace.x
				vy = getTvert CObj CTVFace.y
				vz = getTvert CObj CTVFace.z
				du = minMax true #(vx.x, vy.x, vz.x)
				dv = minMax true #(vx.y, vy.y, vz.y)
				du = abs(du[1] - du[2])
				dv = abs(dv[1] - dv[2])
				if du < thresh or dv < thresh do
					append fs i
			)
		return fs
	)

Utility StreakingFaces "Streaking Faces"
	(
		button ProcessSelection "Process Selection"

		on ProcessSelection pressed do
			(
				ObjArray = selection as array
				oc = ObjArray.count
				format "****** Start streaking faces log.... ******\n"
				for i = 1 to oc do
					(
						if (o = convertToMesh ObjArray[i]) != undefined do
							if (fs = detectStreakingFaces o) != #() do
								(
									setFaceSelection o fs
									format "%\n" o.name
								)
					)
				format "****** End streaking faces log.... ******\n\n"
				messageBox "Complete."
			)
	)
	
