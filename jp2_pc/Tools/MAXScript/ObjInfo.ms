Utility ObjInfo "Object Info"
	(
		group "Things to check for"
			(
				spinner MAXFaces "Faces above:" range:[0,4096,1024] type:#integer fieldWidth:45
				spinner MAXVerts "Verts above:" range:[0,4096,4096] type:#integer fieldWidth:45
			)
		button ProcessSelection "Process Seleciton" width:140
		checkbox FormatLimit "Show Only Bad Objects"

		on ProcessSelection	pressed do
			(
				if (selection as array).count >= 1 then
					(
						ObjArray = selection as array
						oc = ObjArray.count
						for i = 1 to oc do
							(
								if formatLimit.checked then
									(
										CObj = ObjArray[i]
										if (CObj.numfaces > MAXFaces.value) or (CObj.numVerts > MAXVerts.value) do
											(
												format "\nObject: %\n" CObj.name
												if CObj.numfaces > MAXFaces.value do
													format "Num Faces: %\n" CObj.numfaces
												if CObj.numverts > MAXVerts.value do
													format "Num Verts: %\n" CObj.numverts
											)
									) else (
										CObj = ObjArray[i]
										format "\nObject: %\n" CObj.name
										format "Num Faces: %\n" CObj.numfaces
										format "Num Verts: %\n" CObj.numverts
									)
							)
					) else (
						MessageBox "Select something first..."
					)
			)
	)