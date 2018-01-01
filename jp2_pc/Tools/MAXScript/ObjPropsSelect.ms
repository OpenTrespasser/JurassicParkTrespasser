Utility ObjPropsSelect "Obj Props Select"
	(
		local 	SearchArray = #(),
		-- ***********************************************************************************
		-- ***********************************************************************************
		SelectionDefaults = #(\
												#("CInstance", "Tangible = true", "Moveable = true"),\
												#("CTerrainObj", "Height = 1"),\
												#("CTerrainObj", "Height = 2"),\
												#("CTerrainObj", "Height = 3"),\
												#("CTerrainObj", "Height = 4")\
												)
		-- ***********************************************************************************
		-- ***********************************************************************************
		DropDownList Defaults "Default Sets:" items:#(\
																								"Tangible Moveable",\
																								"Terrain Tex Level 1",\
																								"Terrain Tex Level 2",\
																								"Terrain Tex Level 3",\
																								"Terrain Tex Level 4"\
																								)

		DropDownList ObjectType "Object Classes: " items:#("CInstance Object", "Player", "Terrain Texture Object", "Terrain Extents Object", "Terrain Placement Object", "Animal","Magnet","Trigger", "Settings", "Water Object", "Water Disturbance", "Light", "Sky", "Gun") selection:3

		ListBox SearchList "Search List:"
		EditText SearchString
		button AddToList "Add" width:70 offset:[-37,0]
		button RemoveFromList "Remove" width:70 offset:[37,-26]
		button ClearList "Clear" width:70 offset:[-37,0]
		button Foo "Read Mind" width:70 offset:[37,-26]

		checkbox AutoUnhide "Auto Unhide" checked:true
		checkbox KeepSelection "Keep Current Selection"
		button GenerateSelection "Generate Selection" width:140

		label Status1

		on Foo pressed do	
			(
				MessageBox "Unknown System Exception" title:"MAX Script Fatal Error"
				MessageBox "Please place your head\ncloser to the monitor\nand try again"
				MessageBox "ha"
			)

		on SearchList selected s do
			(
				SearchString.text = SearchList.items[s]
			)

		on Defaults selected s do
			(
				SearchArray = SelectionDefaults[s]
				SearchList.items = SearchArray
				SearchString.text = SearchList.items[SearchList.selection]
			)

		on ClearList pressed do
			(
				SearchArray = #()
				SearchList.items = SearchArray
				SearchString.text = ""
			)

		on AddToList pressed do
			(
				CString = SearchString.text
				if CString != "" do
					if FindItem SearchArray CString == 0 do
						(
							append SearchArray CString
							SearchList.items = SearchArray
						)
			)

		on ObjectType selected s do
			(
				ObjTypeString = ""
				if s == 1 do ObjTypeString = "CInstance"
				if s == 2 do ObjTypeString = "Player"
				if s == 3 do ObjTypeString = "CTerrainObj"
				if s == 4 do ObjTypeString = "TerrainExtent"
				if s == 5 do ObjTypeString = "TerrainPlacement"
				if s == 6 do ObjTypeString = "CAnimal"
				if s == 7 do ObjTypeString = "CMagnet"
				if s == 8 do ObjTypeString = "CLocationTrigger"
				if s == 9 do ObjTypeString = "Settings"
				if s == 10 do ObjTypeString = "CEntityWater"
				if s == 11 do ObjTypeString = "CWaterDisturbance"
				if s == 12 do ObjTypeString = "CLightDirectional"
				if s == 13 do ObjTypeString = "CSky"
				if s == 14 do ObjTypeString = "CGun"

				if FindItem SearchArray ObjTypeString == 0 do
					(
						append SearchArray ObjTypeString
						SearchList.items = SearchArray
					)
			)

		on RemoveFromList pressed do
			(
				s = SearchList.selection
				NList = #()
				for i = 1 to SearchArray.count do
					if i != s do
						append NList SearchArray[i]
				SearchList.items = NList
				SearchArray = SearchList.items
				NewText = SearchList.items[SearchList.selection]
				if NewText == undefined do NewText = ""
				SearchString.text = NewText
			)

		on GenerateSelection pressed do
			(
				if KeepSelection.checked == false do
					MAX Select None
				oc = objects.count
				cnt = 0
				ProgressStart "Generating Selection..."
				for o in objects do
					(
						cnt += 1
						if (progressUpdate ((cnt / oc as float) * 100)) == false do Exit
						CBuffer = GetUserPropBuffer o
						SelectObj = true
						for i = 1 to SearchArray.count do
							if (FindString CBuffer SearchArray[i]) == undefined do SelectObj = false
						if SelectObj == true do
							(
								if AutoUnhide.checked do if o.isHidden do unhide o
								selectmore o
							)
					)
				ProgressEnd()
				Status1.text = (selection.count as string + " objects selected")
			)
	)
