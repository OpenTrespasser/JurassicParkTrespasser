Utility RemBumps "Remove Bump Maps"
(
local NMat

button ProcessSelected "Process Selected Objects" width:145
label results

on ProcessSelected pressed do
	(
		ObjArray = for obj in selection collect obj
		oc = ObjArray.count
		for i = 1 to oc do
			(
				NMat = undefined
				o = ObjArray[i]
				if classof o.mat == MultiMaterial do
					(
						ns = o.mat.numsubs
						NMat = MultiMaterial()
						Nmat.numsubs = o.mat.numsubs
						for j = 1 to ns do
							(
							if classof o.mat[j].diffusemap == bitmaptexture do
								(
									NMat[j].diffusemap = o.mat[j].diffusemap
								)
							if classof o.mat[j].opacitymap == bitmaptexture do
								(
									NMat[j].opacitymap = o.mat[j].opacitymap
								)
							)
						o.mat = NMat
					)

				if classof o.mat == StandardMaterial do
					(
						NMat = standardMaterial()
						if classof o.mat.diffusemap == bitmaptexture do
							(
								NMat.diffusemap = o.mat.diffusemap
							)
						if classof o.mat.opacitymap == bitmaptexture do
							(
								NMat.opacitymap = o.mat.opacitymap
							)
						o.mat = NMat
					)
			)
	)
)