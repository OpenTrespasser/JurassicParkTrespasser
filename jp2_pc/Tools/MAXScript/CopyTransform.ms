Utility CopyTransform "Copy Transform"
	(
		local MasterObject
		
		group "Options"
			(
				label MasterLabel "Source:" offset:[-5,0] align:#left
				pickbutton ChooseMaster width:100 align:#right offset:[5,-20]
				checkbox rotation "Rotation"
				checkbox scale "Scale" align:#right offset:[0,-20]
			)
		button CopyToSelection "Copy to Selection" width:140
		label blank
		group "Clean Transforms"
			(
				button CleanRot "Clean Rotation" width:140
				button CleanScale "Clean Scale" width:140
				button CleanBoth "Clean Rotation and Scale" width:140
			)

		on CleanRot pressed do
			(
				ObjArray = selection as array
				for i = 1 to ObjArray.count do
					(
						o = ObjArray[i]
						p = o.pos
						o.rotation = (quat 0 0 0 1)
						o.pos = p
					)
			)
		
		on CleanScale pressed do
			(
				ObjArray = selection as array
				for i = 1 to ObjArray.count do
					(
						o = ObjArray[i]
						p = o.pos
						o.scale = [1,1,1]
						o.pos = p
					)
			)
		
		on CleanBoth Pressed do
			(
				ObjArray = selection as array
				for i = 1 to ObjArray.count do
					(
						o = ObjArray[i]
						p = o.pos
						o.scale = [1,1,1]
						o.rotation = (quat 0 0 0 1)
						o.pos = p
					)
			)

		on ChooseMaster picked obj do
			(
				MasterObject = obj
				ChooseMaster.text = MasterObject.name
			)

		on CopyToSelection pressed do
			(
				if MasterObject != undefined then
					(
						MasterScale = MasterObject.scale
						MasterRotation = MasterObject.rotation
						ObjArray = selection as array
						oc = ObjArray.count
						for i = 1 to oc do
							(
								CObj = ObjArray[i]
								CPos = CObj.pos
								if rotation.checked do CObj.rotation = MasterRotation
								if scale.checked do CObj.Scale = MasterScale
								CObj.pos = CPos
							)
					) else (
						MessageBox "No Source object defined!"
					)
			)
	)