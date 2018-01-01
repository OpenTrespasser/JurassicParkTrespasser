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

fn InstanceNum CName = 
	(
		local DashPos = -1
		for i = 1 to CName.count do
			if CName[i] == "-" do
				DashPos = i
		if DashPos == -1 then
			return undefined 
		else
			(
				Num = (substring CName (DashPos + 1) (CName.count - DashPos))
				return Num
			)
	)

Utility MatchNames "Match Names"
	(
		local c, p, PKeyName, CKeyName, XYThresh

		local debug = true

		label PLabel "Parent:" align:#left offset:[-4,0]
		pickbutton GetParent width:100 align:#right offset:[0,-20]
		label CLabel "Child:" align:#left offset:[-4,0]
		pickbutton GetChild width:100 align:#right offset:[0,-20]
		
		button match "Match Names"
	
		on GetParent Picked obj do
			(
				p = obj
				PKeyName  = keyname p.name
				if PKeyName != GetChild.text then
					(
						GetParent.text = PKeyName
					) else (
						GetParent.text = ""
						p = PKeyName = undefined
						Messagebox "Parent and Child objects\nmust be different"
					)
			)

		on GetChild Picked obj do
			(
				c = obj
				CKeyName  = keyname c.name
				if CKeyName != GetParent.text then
					(
						GetChild.text = CKeyName
						xyMin = [c.min.x, c.min.y]
						xyMax = [c.max.x, c.max.y]
						XYThresh = (distance xyMin xyMax) * 0.5
					) else (
						GetChild.text = ""
						c = CKeyName = XYThresh = undefined
						MessageBox "Parent and Child objects\nmust be different"
					)
			)
		
		on Match pressed do
			(
				ParentList = #()
				ChildList = #()
				UsedChildren = #()
				-- find the valid parents and children in the scene
				for o in objects do
					(
						if (KeyName o.name) == CKeyName do
							(
								append ChildList o
								append UsedChildren #Free
							)
						if (KeyName o.name) == PKeyName do
							append ParentList o
					)
				cnt = 0
				for i = 1 to ParentList.count do
					(
						CParent = ParentList[i]
						PNum = InstanceNum CParent.name
						CParentPos = CParent.pos
						co = -1
						CIndex = -1
						sd = 9999999.1
						for j = 1 to ChildList.count do
							(
								CChild = ChildList[j]
								CChildPos = CChild.pos
								-- get the smallest distance from the parent to the child
								if ((cd = distance [CParentPos.x, CParentPos.y] [CChildPos.x, CChildPos.y]) <= sd) do
									(
										co = CChild 
										sd = cd
										CIndex = j
									)
							)
						if UsedChildren[CIndex] == #Free do
							(
								co.name = CKeyName + "-" + PNum
								UsedChildren[CIndex] = #Used
								cnt += 1
							)
					)
				MessageBox ((cnt * 2) as string + " object's names were re-matched")
			)
	)