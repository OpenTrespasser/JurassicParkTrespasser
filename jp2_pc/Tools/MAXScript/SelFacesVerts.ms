Utility SelFacesVerts "Sel Faces / Verts"
	(
		spinner Index "Index:" type:#integer range:[1,4096,1]
		radioButtons SubObjectType "Type:" labels:#("Faces", "Verts")
		button Select "Select"

		on Select Pressed do
			(
				sel = selection as array
				oc = sel.count 
				for i = 1 to oc do
					(
						if SubObjectType.state == 1 do
							setFaceSelection sel[i] #(index.value)

						if SubObjectType.state == 2 do
							setVertSelection sel[i] #(index.value)
					)
			)		
	)