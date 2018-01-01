Utility selFacesByID "Sel out of range IDs"
(
	button FindBadIds "Find bad ID's in Selected"
	
	on FindBadIds pressed do
		(
			ObjArray = for obj in selection collect obj
			for i = 1 to ObjArray.count do
				(
					CObj = ObjArray[i]
					FaceSel = #()
					OutOfRange = false
					if classof CObj.mat == MultiMaterial then
						(
							convertToMesh CObj
							ns = CObj.mat.numsubs
							nf = CObj.numfaces
							OutOfRange = false
							for i = 1 to nf do
								(
									Cid = (getfacematID CObj i)
									if Cid > ns do
										(
											append FaceSel i
											OutOfRange = true
										)
								)
							if OutOfRange == true do
								(
									setFaceSelection CObj FaceSel
									MessageBox (CObj.name + " has out of range Material ID's\nThose faces are now the current Sub-Object selection.")
								)
						)
				)
		)
)