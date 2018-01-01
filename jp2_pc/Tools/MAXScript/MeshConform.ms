Utility MeshConform "Mesh Conform"
	(
		local tObj, CObjArray = #()
		
		group "Object Setup"
			(
				label targetLabel "Target:" offset:[-5,0] align:#left
				pickButton TargetObj width:100 offset:[5,-20] align:#right
				listbox ConformableObjects "Conformable Objects" height:5
				button Add "Add" width:70 offset:[-37,0]
				button Remove "Remove" width:70 offset:[37,-26]
				button Clear "Clear" width:40 offset:[-52,0]
				button MakeHappy "Make me happy" width:100 offset:[22,-26]
			)

		group "Direction Options"
			(
				spinner DirX "X:" range:[-1,1,0]
				spinner DirY "Y:" range:[-1,1,0]
				spinner DirZ "Z:" range:[-1,1,-1]
				label blank0
				button PresetX "X" width:25 offset:[-45,-85]
				button PresetY "Y" width:25 offset:[-45,0]
				button PresetZ "Z" widht:25 offset:[-45,0]
				checkbox negative "(-)" offset:[10,0]
				label blank1
				label ObjDir "Picked Dir:" align:#left offset:[-5,0]
				pickbutton PickObjDir width:80 align:#right offset:[5,-20]
			)
		button Conform "Conform" width:140
		
		on TargetObj picked obj do
			(
				tObj = obj
				TargetObj.text = tObj.name
			)

		on MakeHappy pressed do
			(
				if queryBox "Do you deserve to be happy?" == true then
					messagebox "Let happiness fill your life..." title:"Happiness Dialog"
				else
					(
						messagebox "You will be forever sad..." title:"Happiness Dialog"
						messagebox "sad sad sad...." title:"Happiness Dialog"
						messagebox "so sad....." title:"Happiness Dialog"
						messagebox "Don't you wish you were happy?" title:"Happiness Dialog"
						messagebox "I'll bet you do..." title:"Happiness Dialog"
						messagebox "but remember... you're sad...." title:"Happiness Dialog"
						messagebox "sad sad sad...." title:"Happiness Dialog"
						messagebox "so sad." title:"Happiness Dialog"
					)
			)
			
		on Remove pressed do
			(
				if CObjArray.count != 0 do
					(
						s = ConformableObjects.selection
						deleteItem CObjArray s
						NewNames = #()
						for i = 1 to CObjArray.count do
							append NewNames CObjArray[i].name
						ConformableObjects.items = NewNames
					)
			)

		on Clear pressed do
			(
				CObjArray = #()
				ConformableObjects.items = #()
			)

		on Add pressed do
			(
				ObjArray = selection as array
				oc = ObjArray.count
				noSel = false
				for i = 1 to oc do
					if (findItem CObjArray ObjArray[i]) == 0 do
						if (getVertSelection ObjArray[i]).count != 0 then
							append CObjArray ObjArray[i]
						else
							noSel = true
				NewNames = #()
				for i = 1 to CObjArray.count do
					append NewNames CObjArray[i].name
				ConformableObjects.items = NewNames
				if noSel do
					MessageBox "Some objects were not added to the list\nbecause they didn't have selected vertices."
			)

		on PickObjDir picked obj do
			(
				cDir = obj.dir
				DirX.value = cDir.x
				DirY.value = cDir.y
				DirZ.value = cDir.z
				PickObjDir.text = obj.name
			)

		on PresetX pressed do
			(
				if negative.checked then
					DirX.value = -1
				else
					DirX.value = 1
				DirY.value = DirZ.value = 0
			)

		on PresetY pressed do
			(
				if negative.checked then
					DirY.value = -1
				else
					DirY.value = 1
				DirX.value = DirZ.value = 0
			)

		on PresetZ pressed do
			(
				if negative.checked then
					DirZ.value = -1
				else
					DirZ.value = 1
				DirX.value = DirY.value = 0
			)

		on Conform pressed do
			(
				if CObjArray.count != 0 and tObj != undefined then
					(
						undo on (
								cDir = normalize [DirX.value,DirY.value,DirZ.value]
								InvCDir = cDir * -1
								oc = CObjArray.count
								ocInv = (1.0 / oc) * 100
								for i = 1 to oc do
									(
										CObj = CObjArray[i]
										cSel = getVertSelection CObj
										for j = 1 to CSel.count do
											(
												nr = ray (getVert CObj cSel[j]) cDir
												Invnr = ray (getVert CObj cSel[j]) InvCDir
												if (ir = intersectRay tObj nr) != undefined then
													(
														setVert CObj cSel[j] ir.pos
													
													) else (
														if (ir = intersectRay tObj Invnr) != undefined do
															setVert CObj cSel[j] ir.pos
													)
											)
										update CObj
									)
							)
					) else (
						MessageBox "Some objects are undefined!"
					)
			)
	)