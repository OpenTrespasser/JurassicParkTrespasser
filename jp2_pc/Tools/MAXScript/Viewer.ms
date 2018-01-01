Utility MaterialsViewer "Viewer"
	(
		button FloatNewInterface "Float Interface"
		
		rollout Mat_Viewer "DWI MAX Material Viewer"
		(
			local ObjArray, MatArray = #(), SubMats
		
			listbox ObjectList "Object List" width:200 offset:[0,0]
			listbox SubMaterialList "SubMaterial List" width:200
			listbox MapListList "Bitmap List" width:200
			bitmap CBitmap width:256 height:256 offset:[205,-457]
			radiobuttons BitmapSize labels:#("1x", "2x", "4x") offset:[120,0]
			label CBitmapLocation offset:[120,0]
			label CBitmapSize offset:[120,0]
			label CBitmapType offset:[120,0]
			button ProcessScene "Parse Scene" width:150 height:30 offset:[-135,170]
			button ProcessSelection "Parse Selection" width:150 height:30 offset:[-135,0]
			label ver "v0.1" offset:[-135,0]

			on ProcessSelection pressed do
				(
					ObjArray = #()
					for o in geometry do
						if o.mat != undefined and o.isSelected do
							append ObjArray o
					ObjectList.items = for o in Objarray collect o.name
				)

			on ProcessScene pressed do
				(
					ObjArray = #()
					for o in geometry do
						if o.mat != undefined do
							append ObjArray o
					ObjectList.items = for o in Objarray collect o.name
				)
		
			on ObjectList selected s do
				(
					CObj = ObjArray[s]
					CMatList = #()
					SubMats = #()
					MapsArray = #()
					MapNameArray = #()
					if CObj.mat != undefined do
						(
							if classof CObj.mat == standardMaterial do
								(
									append SubMats CObj.mat
									MapListList.items = #()
									SubMaterialList.items = #("Single: " + CObj.mat.name)
									if classof CObj.mat.diffusemap == bitmaptexture do
										(
											if CObj.mat.diffusemap.filename != "" then
												(
													append MapsArray (openBitmap SubMats[1].diffusemap.filename)
													append MapNameArray ("Diffuse: " + filenameFromPath CObj.mat.diffusemap.filename)
												)
										)
									if classof CObj.mat.Opacitymap == bitmaptexture do
										(
											if CObj.mat.Opacitymap.filename != "" do
												(
													append MapsArray (openBitmap SubMats[1].Opacitymap.filename)
													append MapNameArray ("Opacity: " + filenameFromPath CObj.mat.Opacitymap.filename)
												)
										)
									if classof CObj.mat.Bumpmap == bitmaptexture do
										(
											if CObj.mat.Bumpmap.filename != "" do
												(
													append MapsArray (openBitmap SubMats[1].Bumpmap.filename)
													append MapNameArray ("Bump: " + filenameFromPath CObj.mat.Bumpmap.filename)
												)
										)
								)
							if classof CObj.mat == MultiMaterial do
								(
									ns = CObj.mat.numsubs
									for i = 1 to ns do append SubMats CObj.mat[i]
									MatListItems = #()
									for i = 1 to SubMats.count do
										if SubMats[i] != undefined then
											append MatListItems (i as string + ": " + SubMats[i].name)
										else
											append MatListItems "<< Undefined Material >>"
		
									SubMaterialList.items = for i = 1 to SubMats.count collect (i as string + ": " + SubMats[i].name)
									if SubMats[1] != undefined do
										(							
											if classof SubMats[1].diffusemap == bitmaptexture do
												(
													if SubMats[1].diffusemap.filename != "" do
														(
															append MapsArray (openBitmap SubMats[1].diffusemap.filename)
															append MapNameArray ("Diffuse:  " + (FilenameFromPath SubMats[1].diffusemap.filename))
														)
												)
											if classof SubMats[1].opacitymap == bitmaptexture do
												(
													if SubMats[1].opacitymap.filename != "" do
														(
															append MapsArray (openBitmap SubMats[1].opacitymap.filename)
															append MapNameArray ("Opacity:  " + (FilenameFromPath SubMats[1].opacitymap.filename))
														)
												)
											if classof SubMats[1].bumpmap == bitmaptexture do
												(
													if SubMats[1].bumpmap.filename != "" do
														(
															append MapsArray (openBitmap SubMats[1].bumpmap.filename)
															append MapNameArray ("Bump:  " + (FilenameFromPath SubMats[1].bumpmap.filename))
														)
												)
										)
								)
							MapListList.items = MapNameArray
							if MapsArray[1] != undefined do
								(
									if BitmapSize.state == 1 do DisplayMap = MapsArray[1]
									if BitmapSize.state == 2 do
										(
											NewSize = [(MapsArray[1].width * 2), (MapsArray[1].height * 2)]
											if NewSize.x > 256 do NewSize.x = 256
											if NewSize.y > 256 do NewSize.y = 256
											displayMap = bitmap NewSize.x NewSize.y
											
											copy MapsArray[1] displayMap
										)
									if BitmapSize.state == 3 do
										(
											NewSize = [(MapsArray[1].width * 2), (MapsArray[1].height * 2)]
											if NewSize.x > 256 do NewSize.x = 256
											if NewSize.y > 256 do NewSize.y = 256
											displayMap = bitmap NewSize.x NewSize.y

											copy MapsArray[1] displayMap
										)
									CBitmap.bitmap = DisplayMap
									CBitmapSize.text = ("Width: " + MapsArray[1].width as string + "   Height: " + MapsArray[1].height as string)
									CBitmapLocation.text = (FileNameFromPath MapsArray[1].filename)
									if MapsArray[1].palette != undefined then
										CBitmapType.text = "Paletted"
									else
										CBitmapType.text = "True Color"
								)
						)
				)
		
			on SubMaterialList selected s do
				(
					MapsArray = #()
					MapNameArray = #()
					if SubMats[s] != undefined then
						(
							if classof SubMats[s].diffusemap == bitmaptexture do
								(
									if SubMats[s].diffusemap.filename != "" do
										(
											append MapsArray (openBitmap SubMats[s].diffusemap.filename)
											append MapNameArray ("Diffuse:  " + (FilenameFromPath SubMats[s].diffusemap.filename))
										)
								)
							if classof SubMats[s].opacitymap == bitmaptexture do
								(
									if SubMats[s].opacitymap.filename != "" do
										(
											append MapsArray (openBitmap SubMats[s].opacitymap.filename)
											append MapNameArray ("Opacity:  " + (FilenameFromPath SubMats[s].opacitymap.filename))
										)
								)
							if classof SubMats[s].bumpmap == bitmaptexture do
								(
									if SubMats[s].bumpmap.filename != "" do
										(
											append MapsArray (openBitmap SubMats[s].bumpmap.filename)
											append MapNameArray ("Bump:  " + (FilenameFromPath SubMats[s].bumpmap.filename))
										)
								)
							MapListList.items = MapNameArray
							if MapsArray[1] != undefined do
								(
									if BitmapSize.state == 1 do DisplayMap = MapsArray[1]
									if BitmapSize.state == 2 do
										(
											NewSize = [(MapsArray[1].width * 2), (MapsArray[1].height * 2)]
											if NewSize.x > 256 do NewSize.x = 256
											if NewSize.y > 256 do NewSize.y = 256
											displayMap = bitmap NewSize.x NewSize.y

											DisplayMap = copy MapsArray[1] displayMap
										)
									if BitmapSize.state == 3 do
										(
											NewSize = [(MapsArray[1].width * 2), (MapsArray[1].height * 2)]
											if NewSize.x > 256 do NewSize.x = 256
											if NewSize.y > 256 do NewSize.y = 256
											displayMap = bitmap NewSize.x NewSize.y

											DisplayMap = copy MapsArray[1] displayMap
										)
									CBitmap.bitmap = DisplayMap
									CBitmapSize.text = ("Width: " + MapsArray[1].width as string + "   Height: " + MapsArray[1].height as string)
									CBitmapLocation.text = (FileNameFromPath MapsArray[1].filename)
									if MapsArray[1].palette != undefined then
										CBitmapType.text = "Paletted"
									else
										CBitmapType.text = "True Color"
								)
						) else (
							MapListList.items = #()
						)
				)
		
			on MapListList selected s do
				(
					if MapsArray.count >= s do
						(
							if BitmapSize.state == 1 do DisplayMap = MapsArray[s]
							if BitmapSize.state == 2 do
								(
									NewSize = [(MapsArray[s].width * 2), (MapsArray[s].height * 2)]
									if NewSize.x > 256 do NewSize.x = 256
									if NewSize.y > 256 do NewSize.y = 256
									displayMap = bitmap NewSize.x NewSize.y
									DisplayMap = copy MapsArray[s] displayMap
								)
							if BitmapSize.state == 3 do
								(
									NewSize = [(MapsArray[s].width * 2), (MapsArray[s].height * 2)]
									if NewSize.x > 256 do NewSize.x = 256
									if NewSize.y > 256 do NewSize.y = 256
									displayMap = bitmap NewSize.x NewSize.y
									DisplayMap = copy MapsArray[s] displayMap
								)
							CBitmap.bitmap = DisplayMap
							CBitmapSize.text = ("Width: " + MapsArray[s].width as string + "   Height: " + MapsArray[s].height as string)
							CBitmapLocation.text = (FileNameFromPath MapsArray[s].filename)
							if MapsArray[s].palette != undefined then
								CBitmapType.text = "Paletted"
							else
								CBitmapType.text = "True Color"
						)
				)
		)

	on FloatNewInterface pressed do
		(
			Viewer = newRolloutFloater "DWI MAX Material Viewer" 510 700 50 80
			addrollout Mat_Viewer Viewer
		)
	)
	