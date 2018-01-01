fn IsMaster CName = 
	(
		local DashPos = -1
		for i = 1 to CName.count do
			if CName[i] == "-" do
				DashPos = i
		if DashPos == -1 then
			return false
		else
			(
				if (substring CName (DashPos) 3) == "-00" then
					return true
				else
					return false
			)
	)

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

fn FindObj CName = 
	(
		for o in objects do
			if o.name == CName do
				return o
		return undefined
	)

fn FindByName CName = 
	(
		for o in objects do
			if (findString o.name CName) != undefined do
				return o
		return undefined
	)

fn BoxObj w l h = (box height:h width:w length:l)

fn FindObjRadius o = 
	(
		ObjCenter = o.center
		nv = o.numverts
		Verts = for i = 1 to nv collect (getvert o i)
		FarVert = 0.0
		VertexDistances = #()
		for i = 1 to nv do
			(
				append VertexDistances (distance [ObjCenter.x, ObjCenter.y, Verts[i].z] Verts[i])
				if VertexDistances[i] > FarVert do FarVert = VertexDistances[i]
			)
		return FarVert
	)

fn CylObj r h s = (cylinder height:h radius:r sides:s)

Utility HiRezLoRez "HiRez LoRez"
	(
		radiobuttons Method "Method" labels:#("Box", "Cylinder")
		button goLo "Go LoRez" width:120
		button goHi "Go HiRez" width:120
		label status1

		-- =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
		-- going Low Resolution
		-- =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
		on goLo pressed do
			(
				MasterData = #()
				MasterKeynames = #()
				ChildData = #()
				LoRezMasters = #()

				ObjArray = selection as array
				oc = ObjArray.count
				ocInv = (1.0 / oc) * 100
				status1.text = "Collecting Keynames"
				-- Collect list of keynames
				for i = 1 to oc do
					(
						CKeyName = KeyName ObjArray[i].name
						if findItem MasterKeyNames CKeyName == 0 do
							append MasterKeyNames CKeyName
					)

				status1.text = "Collecting MasterObject Data"
				-- Collect source (Master Object) data
				for i = 1 to MasterKeyNames.count do
					if (CObj = FindObj (MasterKeyNames[i] + "-00")) != undefined then
						(
							-- #(radius, scale, pivot offset, wire color)
							CRadius = FindObjRadius CObj
							CScale = CObj.max - CObj.min
							COffset = CObj.center - CObj.pivot
							CWireColor = CObj.wirecolor
							CData = #(CRadius, CScale, COffset, CWireColor)
							append MasterData CData
							if Method.state == 1 do
								(
									b = (BoxObj CScale.x CScale.y CScale.z)
									b.wirecolor = CWireColor
									b.pivot = (b.center - COffset)
									append LoRezMasters b
								)
							if Method.state == 2 do
								(
									c = (CylObj CRadius CScale.z 8)
									c.pivot = (c.center - COffset)
									c.wirecolor = CWireColor
									append LoRezMasters c
								)
						) else (
							format ("WARNING: Did not find Master Instance for \"" + MasterKeyNames[i] + "\"\n")
							append MasterData #()
						)

				status1.text = "Collecting Instance Data"
				-- Collect Object Data for all the objects we're going to replace and delete them
				delMe = #()
				for i = 1 to oc do
					(
						CObj = ObjArray[i]
						if IsMaster CObj.name == false do
							(
								-- (name, position, rotation, scale)
								CData = #(CObj.name, CObj.pos, CObj.rotation, CObj.scale)
								append ChildData CData
								append DelMe CObj
							)
					)
				status1.text = "Deleting Instances"
				select delMe
				MAX delete

				status1.text = "Converting to LoRez"
				ProgressStart "Converting to lo rez...."
				cc = ChildData.count
				ocInv = (1.0 / cc) * 100
				for i = 1 to cc do
					(
						ProgressUpdate (ocInv * i)
						-- get the keyname of the child object
						CKeyName = keyName ChildData[i][1]
						if ((CIndex = (findItem MasterKeynames CKeyname)) != 0) then
							(
								o = instance LoRezMasters[CIndex]
								o.wirecolor = MasterData[CIndex][4]
								o.name = ChildData[i][1]
								o.rotation = ChildData[i][3]
								o.scale = ChildData[i][4]
								o.pos = ChildData[i][2]
							) else (
								format ("WARNING: KeyName \"" + CKeyName + "\" was not found in MasterKeynames\n")
							)
					)
				status1.text = "Collecting Garbage"
				-- gc()
				select LoRezMasters
				MAX delete
				ProgressEnd()
				status1.text = ""
			)

		-- =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
		-- going High Resolution
		-- =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
		on goHi pressed do
			(
				MasterData = #()
				ChildData = #()
				MasterObjects = #()
				MasterKeynames = #()

				ObjArray = selection as array
				oc = ObjArray.count
				-- collect a list of the keynames in use by the selection
				status1.text = "Collecting Keynames"
				for i = 1 to oc do
					(
						CKeyName = KeyName ObjArray[i].name
						if findItem MasterKeyNames CKeyName == 0 do
							append MasterKeyNames CKeyName
					)

				-- for each keyname, trackdown the master object
				status1.text = "Collecting Master Objects"
				for i = 1 to MasterKeyNames.count do
					if (CObj = FindObj (MasterKeyNames[i] + "-00")) != undefined then
						(
							append MasterObjects CObj
						) else (
							format ("WARNING: Did not find Master Instance for \"" + MasterKeyNames[i] + "\"\n")
							append MasterObjects undefined
						)

				-- Collect the data for all the instances (children)
				delMe = #()
				status1.text = "Collecting Instance Data"
				for i = 1 to oc do
					(
						CObj = ObjArray[i]
						if IsMaster CObj.name == false do
							(
								-- (name, position, rotation, scale)
								CData = #(CObj.name, CObj.pos, CObj.rotation, CObj.scale)
								append ChildData CData
								append DelMe CObj
							)
					)
				status1.text = ("Deleting " + delMe as string + "Instances")
				select delMe
				MAX delete

				-- Step through the Child array, and copy a new master to the child position, rotation, scale
				cc = ChildData.count
				ocInv = (1.0 / cc) * 100
				status1.text = "Converting to Hi Rez"
				ProgressStart "Converting to hi rez...."
				for i = 1 to cc do
					(
						ProgressUpdate (ocInv * i)
						CKeyName = KeyName ChildData[i][1]
						if (CIndex = findItem MasterKeyNames CKeyName) != 0 then
							(
								o = instance MasterObjects[CIndex]
								o.wirecolor = MasterObjects[CIndex].wirecolor
								o.name = ChildData[i][1]
								o.rotation = ChildData[i][3]
								o.scale = ChildData[i][4]
								o.pos = ChildData[i][2]
							) else (
								format "WARNING:\n"
							)
					)
				status1.text = "Collecting Garbage"
				-- gc()
				ProgressEnd()
				status1.text = ""
			)
	)