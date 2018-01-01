Utility MunchScene "Munch Scene"
	(
		local terrain
	
		spinner DelEvery "Delete Every" range:[1,10,1] type:#integer
		button DelObjs "Delete" width:70 offset:[-37,0]
		button SelObjs "Select" width:70 offset:[37,-26]
		label blank
		group "Munch by Terrain Slope"
			(
				label TerrainLabel "Terrain:" align:#left offset:[-5,0]
				pickbutton getTerrain width:100 offset:[20,-20]
				spinner SlopeThresh "Slope Thresh:" range:[0,90,35]
				button SlopeSelect "Select" width:140
			)
		
		on getTerrain picked obj do
			(
				terrain = obj
				getTerrain.text = terrain.name
			)
		
		on SlopeSelect pressed do
			(
				if Terrain != undefined then
					(
						ObjArray = selection as array
						oc = ObjArray.count
						ocInv = (1.0 / oc) * 100
						cr = ray [0,0,0] [0,0,-1]
						Sel = #()
						ProgressStart "Selecting..."
						for i = 1 to oc do
							(
								ProgressUpdate (ocInv * i)
								CObj = ObjArray[i]
								cr.pos = [CObj.pos.x, CObj.pos.y, (Terrain.max.z + 1.0)]
								if (ir = intersectRay terrain cr) != undefined do
									(
										N = ir.dir
										NewAngle = 90 - (atan (N.z / sqrt(N.x^2 + N.y^2)))
										if NewAngle >= SlopeThresh.value do
											append Sel CObj
									)
							)
						ProgressEnd()
						select Sel
					) else (
						MessageBox "No terrain is defined."
					)
			)
		
		on DelObjs pressed do
			(
				ObjArray = #()
		
				for obj in selection do
					if Obj.parent == undefined do append ObjArray Obj
		
				oc = ObjArray.count
				DeleteArray = #()
				NumDeletes = (oc / DelEvery.value as float) as integer
				for i = 1 to NumDeletes do
					(
						d = false
						while d == false do
							(
								Ci = random 1 oc
								if ObjArray[Ci] != #deleted do
									(
										append DeleteArray ObjArray[Ci]
										ObjArray[Ci] = #deleted
										d = true
									)
							)
					)
				select DeleteArray
				MAX delete
				MessageBox ("Done Munching...\nI sure was hungry!\nI ate " + NumDeletes as string + " objects\nPlease feed me again soon....")
			)
		
		on SelObjs pressed do	
			(
				ObjArray = #()
		
				for obj in selection do
					if Obj.parent == undefined do append ObjArray Obj
		
				oc = ObjArray.count
				DeleteArray = #()
				NumDeletes = (oc / DelEvery.value as float) as integer
				for i = 1 to NumDeletes do
					(
						d = false
						while d == false do
							(
								Ci = random 1 oc
								if ObjArray[Ci] != #deleted do
									(
										append DeleteArray ObjArray[Ci]
										ObjArray[Ci] = #deleted
										d = true
									)
							)
					)
				select DeleteArray
			)
	) -- end Utility