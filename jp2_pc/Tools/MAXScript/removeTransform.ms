Utility RemoveRotation "Remove Rotation"
	(
		button processSelection "Process Selection"
		
		on ProcessSelection pressed do
			(
				Objarray = selection as array
				oc = ObjArray.count
				for i = 1 to oc do
					(
						o = ObjArray[i]
						oPos = o.pos
						o.rotation = (quat 0 0 0 1)
						o.scale = [1,1,1]
						o.pos = oPos
					)
			)
	)