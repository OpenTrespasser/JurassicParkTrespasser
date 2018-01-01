fn roundTo val n = 
	(
		local mult = 10.0 ^ n
		(floor ((val * mult) + 0.5)) / mult
	)

Utility Coplanar "Coplanar"
(
	pickbutton ChooseObject "Choose Object" width:140

	on ChooseObject picked obj do
		(
			if classof obj == editable_mesh then
				(
					CObj = obj
					ConvertToMesh CObj
					NotCoplanar = false
					FaceArray = getFaceSelection CObj
					CNormal = GetFaceNormal CObj Facearray[1]
					CNormal = [(RoundTo CNormal.x 4),(RoundTo CNormal.y 4),(RoundTo CNormal.z 4)]
					format "Checking against: %\n" CNormal
					for i = 1 to FaceArray.count do
						(
							NewNormal = GetFaceNormal CObj FaceArray[i]
							NewNormal = [(RoundTo NewNormal.x 4),(RoundTo NewNormal.y 4),(RoundTo NewNormal.z 4)]
							if NewNormal != CNormal do NotCoplanar = true
							format "NewNormal: %\n" NewNormal
						)
					if NotCoplanar == true then
						MessageBox "The selected faces are NOT coplanar."
					else
						MessageBox "The selected faces are coplanar!"
				) else (
					Messagebox "This tool can only use an EditableMesh object.\n Please collapse your mesh."
				)
		)
) -- end utility
