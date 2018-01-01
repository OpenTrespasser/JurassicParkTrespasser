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

Utility SceneWrangling "Scene Wrangling"
	(
		ListBox Method "Method:" items:#("Select from Scene", "Select from Selection") height:2
		button MastersWithoutInstances "Maseter w/o Instances" width:140
		label status1

		on MastersWithoutInstances pressed do
			(
				if method.selection == 1 do
					ObjArray = objects as array
				
				if method.selection == 2 do
					ObjArray = selection as array
				
				oc = ObjArray.count
				-- build a list of the master objects
				MasterObjects = #()
				masters = #()
				foundMasters = #()
				status1.text = "Finding Masters"
				for i = 1 to oc do
					(
						o = ObjArray[i]
						if (IsMaster o.name) and (o.name[1] != "!") do
							if findItem Masters (KeyName o.name) == 0 do
								(
									append MasterObjects o
									append Masters (KeyName o.name)
									append foundMasters false
								)
					)
				status1.text = "Finding Masters without instances"
				for i = 1 to oc do
					(
						o = ObjArray[i]
						-- look at only potential instances
						if IsMaster (o.name) == false do 
							(
								CName = KeyName o.name
								if (CIndex = findItem masters CName) != 0 do
									foundMasters[CIndex] = true
							)
					)
				status1.text = "generating selection"
				sel = #()
				for i = 1 to foundMasters.count do
					if foundMasters[i] == false do
						append sel masterObjects[i]
				if sel.count > 0 then
					(
						status1.text = "Selecting"
						select sel
						messageBox ((sel.count as string) + " Objects selected")
					) else (
						messageBox "0 Objects selected"
					)
			)
	)
