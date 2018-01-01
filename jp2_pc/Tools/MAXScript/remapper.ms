Utility ReMapper "Map Path Re-mapper"
(
local SaveLoc

checkbox WDialogs "Show Warning Dialogs" checked:true
Label ObjectLabel "Object:" align:#left
pickbutton PickObject align:#right width:90 offset:[0,-20]
button RemapSelected "Remap Selected" width:140

on PickObject picked obj do
	(
		o = obj
		if (SaveLoc = getsavePath caption:"Specify map folder") != undefined do
			(
				MapNotFound = false
				PickObject.text = o.name
				-- if we have a standard material
				if classof o.mat == StandardMaterial do
					(

						if classof o.mat.diffusemap == bitmaptexture do
							(
								MapName = FilenamefromPath o.mat.diffusemap.filename
								NewFileName = (SaveLoc + "\\" + MapName)
								if (getfiles NewFileName).count != 0 then
									(
										o.mat.diffusemap.filename = NewFileName
									) else (
										MapNotFound = true
										if WDialogs.checked do
											Messagebox ("Bitmap " + MapName + " was not found\nin the specified location.")
									)
							)

						if classof o.mat.opacitymap == bitmaptexture do
							(
								MapName = FilenamefromPath o.mat.OpacityMap.filename
								NewFileName = (SaveLoc + "\\" + MapName)
								if (getfiles NewFileName).count != 0 then
									(
										o.mat.OpacityMap.filename = NewFilename
									) else (
										MapNotFound = true
										if WDialogs.checked do
											Messagebox ("Bitmap " + MapName + " was not found\nin the specified location.")
									)
							)

						if classof o.mat.BumpMap == bitmaptexture do
							(
								MapName = FilenamefromPath o.mat.BumpMap.filename
								NewFileName = (SaveLoc + "\\" + MapName)
								if (getfiles NewFileName).count != 0 then
									(
										o.mat.BumpMap.filename = NewFilename
									) else (
										MapNotFound = true
										if WDialogs.checked do
											Messagebox ("Bitmap " + MapName + " was not found\nin the specified location.")
									)
							)
					)
				-- if we have a multi material
				if classof o.mat == MultiMaterial do
					(
						ns = o.mat.numsubs
						for i = 1 to ns do
							(
								if classof o.mat[i].diffusemap == bitmaptexture do
									(
										MapName = FilenamefromPath o.mat[i].diffusemap.filename
										NewFileName = (SaveLoc + "\\" + MapName)
										if (getfiles NewFileName).count != 0 then
											(
												o.mat[i].diffusemap.filename = NewFilename
											) else (
												MapNotFound = true
												if WDialogs.checked do
													Messagebox ("Bitmap " + MapName + " was not found\nin the specified location.")
											)
									)

								if classof o.mat[i].opacitymap == bitmaptexture do
									(
										MapName = FilenamefromPath o.mat[i].OpacityMap.filename
										NewFileName = (SaveLoc + "\\" + MapName)
										if (getfiles NewFileName).count != 0 then
											(
												o.mat[i].OpacityMap.filename = NewFilename
											) else (
												MapNotFound = true
												if WDialogs.checked do
													Messagebox ("Bitmap " + MapName + " was not found\nin the specified location.")
											)
									)

								if classof o.mat[i].BumpMap == bitmaptexture do
									(
										MapName = FilenamefromPath o.mat[i].BumpMap.filename
										NewFileName = (SaveLoc + "\\" + MapName)
										if (getfiles NewFileName).count != 0 then
											(
												o.mat[i].BumpMap.filename = NewFilename
											) else (
												MapNotFound = true
												if WDialogs.checked do
													Messagebox ("Bitmap " + MapName + " was not found\nin the specified location.")
											)
									)
							)
					)
					if MapNotfound == true do MessageBox "Some maps not re-mapped"
				)
	)

on RemapSelected pressed do
(
		Objarray = for obj in selection collect obj
		oc = ObjArray.count
		if (SaveLoc = getsavePath caption:"Specify map folder") != undefined do
			(
				for i = 1 to oc do
					(
						o = ObjArray[i]
						MapNotFound = false

						-- if we have a standard material
						if classof o.mat == StandardMaterial do
							(
		
								if classof o.mat.diffusemap == bitmaptexture do
									(
										MapName = FilenamefromPath o.mat.diffusemap.filename
										NewFileName = (SaveLoc + "\\" + MapName)
										if (getfiles NewFileName).count != 0 then
											(
												o.mat.diffusemap.filename = NewFileName
											) else (
												MapNotFound = true
												if WDialogs.checked do
													Messagebox ("Bitmap " + MapName + " was not found\nin the specified location.")
											)
									)
		
								if classof o.mat.opacitymap == bitmaptexture do
									(
										MapName = FilenamefromPath o.mat.OpacityMap.filename
										NewFileName = (SaveLoc + "\\" + MapName)
										if (getfiles NewFileName).count != 0 then
											(
												o.mat.OpacityMap.filename = NewFilename
											) else (
												MapNotFound = true
												if WDialogs.checked do
													Messagebox ("Bitmap " + MapName + " was not found\nin the specified location.")
											)
									)
		
								if classof o.mat.BumpMap == bitmaptexture do
									(
										MapName = FilenamefromPath o.mat.BumpMap.filename
										NewFileName = (SaveLoc + "\\" + MapName)
										if (getfiles NewFileName).count != 0 then
											(
												o.mat.BumpMap.filename = NewFilename
											) else (
												MapNotFound = true
												if WDialogs.checked do
													Messagebox ("Bitmap " + MapName + " was not found\nin the specified location.")
											)
									)
							)
						-- if we have a multi material
						if classof o.mat == MultiMaterial do
							(
								ns = o.mat.numsubs
								for i = 1 to ns do
									(
										if classof o.mat[i].diffusemap == bitmaptexture do
											(
												MapName = FilenamefromPath o.mat[i].diffusemap.filename
												NewFileName = (SaveLoc + "\\" + MapName)
												if (getfiles NewFileName).count != 0 then
													(
														o.mat[i].diffusemap.filename = NewFilename
													) else (
														MapNotFound = true
														if WDialogs.checked do
															Messagebox ("Bitmap " + MapName + " was not found\nin the specified location.")
													)
											)
		
										if classof o.mat[i].opacitymap == bitmaptexture do
											(
												MapName = FilenamefromPath o.mat[i].OpacityMap.filename
												NewFileName = (SaveLoc + "\\" + MapName)
												if (getfiles NewFileName).count != 0 then
													(
														o.mat[i].OpacityMap.filename = NewFilename
													) else (
														MapNotFound = true
														if WDialogs.checked do
															Messagebox ("Bitmap " + MapName + " was not found\nin the specified location.")
													)
											)
		
										if classof o.mat[i].BumpMap == bitmaptexture do
											(
												MapName = FilenamefromPath o.mat[i].BumpMap.filename
												NewFileName = (SaveLoc + "\\" + MapName)
												if (getfiles NewFileName).count != 0 then
													(
														o.mat[i].BumpMap.filename = NewFilename
													) else (
														MapNotFound = true
														if WDialogs.checked do
															Messagebox ("Bitmap " + MapName + " was not found\nin the specified location.")
													)
											)
									)
							)
						if MapNotfound == true do MessageBox ("Some maps in object " + o.name + " were not re-mapped")
					)
			)
		MessageBox "Done!"
	)


) -- End Utility