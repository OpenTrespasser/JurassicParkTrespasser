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

fn Min foo a = 
	(
		local mn = a[1]
		for i = 2 to a.count do
			if a[i] < mn do mn = a[i]
		return mn
	)

fn ClampW CObj = 
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

fn FlipW CObj = 
	(
		for j = 1 to CObj.numfaces do
			(
				CFace = (GetTVFace cObj j)
				c1 = getTvert CObj CFace.x
				c2 = getTvert CObj CFace.y
				c3 = getTvert CObj CFace.z
				if c1.z < 0.0 do SetTVert CObj CFace.x [c1.x, c1.y, (abs c1.z)]
				if c2.z < 0.0 do SetTVert CObj CFace.y [c2.x, c2.y, (abs c2.z)]
				if c3.z < 0.0 do SetTVert CObj CFace.z [c3.x, c3.y, (abs c3.z)]
			)
		update CObj
	)

fn roundTo val n = 
	(
		local mult = 10.0 ^ n
		(floor ((val * mult) + 0.5)) / mult
	)

-- this function returns an array of three arrays - #(#(TVert indices), #(TVert UV's), #(Vertex positions)).
-- it takes an extra argument - #selection, or #all, which determines all the faces, or the current face selection
fn GetTVs CObj =
	(
		local FaceArray = #()
		local nf = CObj.numfaces
		local TVertArray = #()
		local TVertUVArray = #()
		local TVFaceArray = #()
		local VertArray = #()
		-- get the faces into FaceArray#()
		FaceArray = getFaceSelection CObj
		if FaceArray.count == 0 do
			FaceArray = for i = 1 to CObj.numfaces collect i
		-- get all the Tverts
		for i = 1 to FaceArray.count do append TVFaceArray (getTVFace CObj FaceArray[i])
		for i = 1 to TVFaceArray.count do
			(
				CFace = getFace CObj FaceArray[i]
				if FindItem TVertArray TVFaceArray[i].x == 0 do 
					(
						append TVertArray (TVFaceArray[i].x as integer)
						append VertArray (getvert CObj CFace.x)
					)
				if FindItem TVertArray TVFaceArray[i].y == 0 do 
					(
						append TVertArray (TVFaceArray[i].y as integer)
						append VertArray (getvert CObj CFace.y)
					)
				if FindItem TVertArray TVFaceArray[i].z == 0 do 
					(
						append TVertArray (TVFaceArray[i].z as integer)
						append VertArray (getvert CObj CFace.z)
					)
			)
		-- get the TVert point3's
		TVertUVArray = (for i = 1 to TVertArray.count collect (GetTVert CObj TVertArray[i]))
		AllTVerts = #(TVertArray, TVertUVArray, VertArray)
		return AllTVerts
	)

Utility TVInfo "TVert Info"
	(
		local RndPlcs = 2
		button GetSelection "Get info for Selection" width:140
		checkbox OutOfRange "Out of Range"
		checkbox Subsel "Selection"
		button ClampWObj "Clamp W Selection" width:140
		button FlipWObj "Make W Positive" width:140
		button NormalizeTVerts "Normalize TVerts" width:140
		
		on NormalizeTVerts pressed do
			(
				ObjArray = for o in selection collect o
				oc = ObjArray.count
				for i = 1 to oc do
					(
						CObj = ObjArray[i]
						ClampW CObj
						av3TV = #()
						aiTVi = #()
						for j = 1 to CObj.numfaces do
							(
								CFace = (GetTVFace cObj j)
								if FindItem aiTVi CFace.x == 0 do
									(
										append aiTVi CFace.x
										append av3TV (getTVert CObj CFace.x)
									)

								if FindItem aiTVi CFace.y == 0 do
									(
										append aiTVi CFace.y
										append av3TV (getTVert CObj CFace.y)
									)

								if FindItem aiTVi CFace.z == 0 do
									(
										append aiTVi CFace.z
										append av3TV (getTVert CObj CFace.z)
									)
							)
						afTVu = for j = 1 to av3TV.count collect av3TV[j].x
						afTVv = for j = 1 to av3TV.count collect av3TV[j].y
						MinU = (Min true afTVu)
						MinV = (Min true afTVv)

						if MinU < 0.0 then
							MinU = (MinU as integer) - 1
						else
							MinU = (MinU as integer)

						if MinV < 0.0 then
							MinV = (MinV as integer) - 1
						else
							MinV = (MinV as integer)
						
						for j = 1 to av3TV.count do
							(
								av3TV[j].x = av3TV[j].x - MinU
								av3TV[j].y = av3TV[j].y - MinV
							)
						for j = 1 to av3TV.count do
							setTVert CObj aiTVi[j] av3TV[j]
					)
			)

		on ClampWObj pressed do
			(
				ObjArray = for o in selection collect o
				oc = ObjArray.count
				for j = 1 to oc do ClampW Objarray[j]
			)

		on FlipWObj pressed do
			(
				ObjArray = for o in selection collect o
				oc = ObjArray.count
				for j = 1 to oc do FlipW Objarray[j]
			)

		on GetSelection pressed do
			(
				ObjArray = for o in selection collect o
				oc = ObjArray.count
				for j = 1 to oc do
					(
						CObj = ObjArray[j]
						format "\n\n%\n" CObj.name
						format "#Faces: %\n" CObj.numfaces
						TVFaceArray = #()
						FaceArray = for i = 1 to CObj.numfaces collect i
						if SubSel.checked do FaceArray = getFaceSelection CObj
						for i = 1 to FaceArray.count do
							(
								format "\nFace: %\n" i
								CFace = getTVFace CObj FaceArray[i]

								format "TVert Indices: % % %\n" CFace.x CFace.y CFace.z

								CTvert = getTVert cObj CFace.x
								if OutOfRange.checked then
									(
										if CTvert.x > 1.0 do format "u: %\n" CTvert.x
										if CTvert.x < 0.0 do format "u: %\n" CTvert.x
										if CTvert.y > 1.0 do format "v: %\n" CTvert.y
										if CTvert.y < 0.0 do format "v: %\n" CTvert.y
									) else (
										format "%:  %  %  %\n" CFace.x CTvert.x CTvert.y CTvert.z
									)
									
							
								CTvert = getTVert cObj CFace.y
								if OutOfRange.checked then
									(
										if CTvert.x > 1.0 do format "u: %\n" CTvert.x
										if CTvert.x < 0.0 do format "u: %\n" CTvert.x
										if CTvert.y > 1.0 do format "v: %\n" CTvert.y
										if CTvert.y < 0.0 do format "v: %\n" CTvert.y
									) else (
										format "%:  %  %  %\n" CFace.y CTvert.x CTvert.y CTvert.z
									)

								CTvert = getTVert cObj CFace.z
								if OutOfRange.checked then
									(
										if CTvert.x > 1.0 do format "u: %\n" CTvert.x
										if CTvert.x < 0.0 do format "u: %\n" CTvert.x
										if CTvert.y > 1.0 do format "v: %\n" CTvert.y
										if CTvert.y < 0.0 do format "v: %\n" CTvert.y
									) else (
										format "%:  %  %  %\n" CFace.z CTvert.x CTvert.y CTvert.z
									)
							)
					)
			)
	)