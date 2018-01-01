Utility FixIt "Fix It!"
	(
		button MakeMasters "Make Masters"
		
		on MakeMasters pressed do
			(
				ObjArray = selection as array
				oc = ObjArray.count
				for i = 1 to oc do
					(
						CObj = ObjArray[i]
						if (CIndex = FindString CObj.name "-") != undefined then
							CName = substring CObj.name 1 (CIndex - 1)
						else
							CName = CObj.name
						CObj.name = (CName + "-00")
					)
			)
	)