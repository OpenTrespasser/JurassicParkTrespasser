fn KeyName CName = 
	(
		local DashPos = -1
		for i = 1 to CName.count do
			if CName[i] == "-" do
				DashPos = i
		if DashPos == -1 then
			return CName
		else
			return (substring CName 1 (DashPos - 1))
	)

Utility Equalizer "Equalizer"
	(
		group ""
			(
				checkbox MatDef "Material Definition" checked:true
				checkbox ObjProps "Object Properties"
				checkbox WireCol "Wire Color" checked:true
				checkbox MatIDs "Material ID's" checked:true
			)
		
		button EqualizeObjects "Equalize Selection" width:140
	
		on EqualizeObjects pressed do
			(
				WireColData = #()
				propData = #()
				MatData = #()
				KeyNameData = #()
				nf = -1
				MatIDData = #()
				ObjArray = selection as array
				oc = ObjArray.count 
				ocInv = (1.0 / oc) * 100
				-- get the data for all -00 objects
				ProgressStart "Collecting Data...."
				for i = 1 to oc do
					(
						ProgressUpdate (i * ocInv)
						o = ObjArray[i]
						if (substring o.name (o.name.count - 2) 3) == "-00" do
							(
								append KeyNameData (KeyName o.name)
								if MatDef.checked and o.mat != undefined then
									append MatData (o.mat)
								else
									append MatData undefined
								if ObjProps.checked do append propData (getUserPropBuffer o)
								if WireCol.checked do append WireColData o.wirecolor
								if MatIDs.checked do
									(
										append MatIDData (for j = 1 to o.numfaces collect (getFaceMatID o j))
										nf = o.numfaces
									)
							)
					)
				ProgressEnd()
				ProgressStart "Equalizing...."
				for i = 1 to oc do
					(
						ProgressUpdate (i * ocInv)
						o = ObjArray[i]
						if (CIndex = FindItem KeyNameData (KeyName o.name)) != 0 do
							(
								if MatDef.checked do
									if MatData[CIndex] != undefined do
										o.mat = MatData[CIndex]
								if ObjProps.checked do (SetUserPropBuffer o PropData[CIndex])
								if WireCol.checked do o.wirecolor = WireColData[CIndex]
								if MatIDs.checked do
									(
										if o.numfaces == nf do
											for j = 1 to nf do
												setFaceMatID o j MatIDData[CIndex][j]
									)
							)
					)
				ProgressEnd()
			)
	)