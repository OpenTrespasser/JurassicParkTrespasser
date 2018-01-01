fn QuickSort l r a = 
	(
		if r > l do
			(
				i = l - 1
				j = r
				do (
					do (i += 1) while a[i] < a[r]
					while j > 1 do
						(
							j -= 1
							if a[j] <= a[r] do Exit
						)
					t = a[i]
					a[i] = a[j]
					a[j] = t
				) while (
					i < j
				)
				a[j] = a[i]
				a[i] = a[r]
				a[r] = t
				QuickSort l (i - 1) a
				QuickSort (i + 1) r a
			)
		return a
	)

fn QuickSort2 l r a b =
	(
		if r > l do
			(
				i = l - 1
				j = r
				do (
					do (i += 1) while a[i] < a[r]
					while j > 1 do
						(
							j -= 1
							if a[j] <= a[r] do Exit
						)
					t = a[i]
					a[i] = a[j]
					a[j] = t

					t2 = b[i]
					b[i] = b[j]
					b[j] = t2

				) while (
					i < j
				)
				a[j] = a[i]
				a[i] = a[r]
				a[r] = t

				b[j] = b[i]
				b[i] = b[r]
				b[r] = t2

				QuickSort2 l (i - 1) a b
				QuickSort2 (i + 1) r a b
			)
		return #(a,b)
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


Utility SelectionTools "Selection Tools"
	(
		
		local 	SearchArray = #(),
		-- ***********************************************************************************
		SelectionDefaults = #(\
												#("CInstance", "Tangible = true", "Moveable = true"),\
												#("CTerrainObj", "Height = 1"),\
												#("CTerrainObj", "Height = 2"),\
												#("CTerrainObj", "Height = 3"),\
												#("CTerrainObj", "Height = 4")\
												)
		-- ***********************************************************************************

		ListBox Method "Method:" items:#("Select from Scene", "Select from Selection") height:2
		checkbox KeepSel "Keep Current Selection"
		group "Super Select"
			(
				label FilterLabel "Filter:" align:#left
				checkbox SelectZeroth "Master Objects" offset:[10,0]
				checkbox non00Objects "w/o Master Instance" offset:[10,0]
				checkbox WOChildren "Masters w/o Children" offset:[10,0]
				checkbox selectNonObjects "Non-Objects" offset:[10,0]
				checkbox TooManyProps "Too Many Properties" offset:[10,0]
				checkbox SelWOProps "Without Properties" offset:[10,0]
				checkbox ignoreVeg "Ignore veg" offset:[20,0] enabled:false
				checkbox HasBumps "Has Bump Maps" offset:[10,0]
				checkbox SelectByWC "By Wire Color" offset:[10,0]
				colorpicker cWC color:(color 0 0 255) offset:[-50,0] enabled:false
				pickbutton GetWC "Choose" align:#right offset:[-10,-24] enabled:false
				checkBox ByHeight "By Height" offset:[10,0]
				spinner ObjHeight "Height:" range:[0,100,15] type:#float enabled:false fieldWidth:45
				radiobuttons SelBy labels:#("Above", "Below") enabled:false
				checkBox SpecificSearch "Name Search" offset:[10,0]
				radioButtons SearchBy labels:#("Key Name is:", "Name Contains:", "First Letter is:") enabled:false
				editText SearchText enabled:false
				button GenSel "Select" width:140
				label status2
				button SelDups "Select Duplicate Names" width:140
				label status3
				button SelMasters "Select Masters"
			)
		
		group "Select by Objects Props"
			(
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
				button ReadMind "Read Mind" width:70 offset:[37,-26]
				checkbox AutoUnhide "Auto Unhide" checked:true
				checkbox KeepSelection "Keep Current Selection"
				button GenerateSelection "Generate Selection" width:140
				label Status1
			)

		group "Selection Propagation"
			(
				button PropagateInstances "Propagate to Instances" width:140
				button PropagateMasters "Propagate to Masters" width:140
			)

		on SelMasters pressed do
			(
				MAX select none
				print "sel None"
				for o in objects do (if (isMaster o.name) do selectmore o)
			)

		on SelDups pressed do
			(
				ObjArray = #()
				if Method.selection == 1 do
					for o in objects do
						if o.name[1] != "!" do
							append ObjArray o

				if Method.selection == 2 do	
					for o in selection do
						if o.name[1] != "!" do
							append ObjArray o

				ObjNames = for o in ObjArray collect o.name
				status3.text = "Sorting..."
				QuickSort2 1 ObjNames.count ObjNames ObjArray
				Sel = #()
				status3.text = "Collecting Garbage..."
				-- gc()
				oc = ObjNames.count
				ocInv = (1.0 / oc) * 100
				status3.text = "Searching..."
				ProgressStart "Searching..."
				for i = 1 to (ObjNames.count - 1) do
					(
						ProgressUpdate (ocInv * i)
						if ObjNames[i] == ObjNames[i + 1] do
							(
								append Sel ObjArray[i]
								append Sel ObjArray[i + 1]
							)
					)
				status3.text = "Selecting Objects..."
				unhide Sel
				select Sel
				status3.text = "Collecting Garbage..."
				-- gc()
				ProgressEnd()
				status3.text = ""
				MessageBox (Sel.count as string + " objects selected.")
			)

		on SelWOProps changed state do
			if SelWOProps.checked then
				ignoreVeg.enabled = true
			else
				ignoreVeg.enabled = false

		on SpecificSearch changed state do
			if SpecificSearch.checked then
				SearchBy.enabled = SearchText.enabled = true
			else
				SearchBy.enabled = SearchText.enabled = false

		on GenSel pressed do
			(
				status2.text = "Gathering object data..."
				if Method.selection == 1 do ObjArray = objects as array
				if Method.selection == 2 do	ObjArray = Selection as array

				if KeepSel.checked then
					SelArray = selection as array
				else	
					SelArray = #()
				unhideArray = #()
				ChildrenArray = #()
				oc = ObjArray.count
				ocInv = (1.0 / oc) * 100
				ProgressStart "Generating Selection...."
				status2.text = "Generating Selection"
				MasterKeyNames = #()

				for i = 1 to oc do
					(
--						if (mod i 100) == 0 do
--							(
--								gc()
--								status2.text = ("Object" + i as string)
--							)
						ProgressUpdate (ocInv * i)
						CObj = ObjArray[i]
						CKeyName = Keyname CObj.name
						-- if (FindItem SelArray CObj) == 0 do
							(
								-- Select "-00" Objects
								if SelectZeroth.checked do
									if (IsMaster CObj.name) do
										append SelArray CObj

								-- Masters without Children
								if WOChildren.checked do
									if (substring CObj.name (CObj.name.count - 2) 3) == "-00" then
										(
											if (findItem ChildrenArray CKeyName) != 0 then
												append SelArray CObj
											else
												for o in ObjArray where o != CObj do
													if CKeyName == (Keyname o.name) do
														(
															append SelArray CObj
															EXIT
														)
										) else (
											if (findItem ChildrenArray CKeyName) == 0 do 
												append ChildrenArray CKeyname
										)

								-- Select objects that have no Master Instance
								if non00Objects.checked do
									(
										if (IsMaster CObj.name) == false do
											(
												CKey = KeyName CObj.name
												fm = false
												for obj in ObjArray do
													if obj.name == (CKey + "-00") do
														(
															fm = true
															Exit
														)
												if fm == false do
													if FindItem SelArray CObj == 0 do
														append SelArray CObj
											)
									)

								-- Select Non Objects
								if selectNonObjects.checked do
									if (CObj = convertToMesh CObj) != undefined do
										if CObj.numfaces == 0 or CObj.numverts == 0 do
											append SelArray CObj
								
								-- Select without properties
								if SelWOProps.checked do
									(
										if ignoreVeg.checked then
											(
												if CObj.name[1] != "v" and CObj.name[1] != "V" do
													if (getUserPropBuffer CObj).count < 5 do
														append SelArray CObj
											) else (
												if (getUserPropBuffer CObj).count < 5 do
													append SelArray CObj
											)
									)
	
								-- Too many Properties
								if TooManyProps.checked do
									if (getUserPropBuffer CObj).count > 500 do
										append SelArray CObj

								-- Has a bump map
								if HasBumps.checked do
									if CObj.mat != undefined do
										(
											if classof CObj.mat == standardMaterial do
												if CObj.mat.bumpMap != undefined do
													append SelArray CObj
										
											if classof CObj.mat == multiMaterial do
												(
													ns = CObj.mat.numsubs
													for n = 1 to ns do
														if CObj.mat[n].bumpmap != undefined do
															(
																append SelArray CObj
																Exit
															)
												)
										)
									
								-- Select by Wirecolor
								if SelectByWC.checked do
									if CObj.wirecolor == cWC.color do
										append SelArray CObj

								-- Select By Height
								if ByHeight.checked do
									if SelBy.state == 1 then
										if (CObj.max.z - CObj.min.z) > ObjHeight.value do
											append SelArray CObj
									else
										if (CObj.max.z - CObj.min.z) < ObjHeight.value do
											append SelArray CObj

								if SpecificSearch.checked do
									(
										if SearchBy.state == 1 do
											if (KeyName CObj.name) == SearchText.text do
												append SelArray CObj

										if SearchBy.state == 2 do
											if (FindString CObj.name SearchText.text) != undefined do
												append SelArray CObj

										if SearchBy.state == 3 do
											if CObj.name[1] == SearchText.text[1] do
												append SelArray CObj
									)
							)
					)
				status2.text = "Selecting Objects..."
				unhide SelArray
				select SelArray
				ProgressEnd()
				status2.text = ""
				messageBox ((SelArray.count as string) + " objects selected.")
			)

		on Method selected s do
			if s == 1 then
				(
					KeepSel.enabled = true
				) else (
					KeepSel.checked = false
					KeepSel.enabled = false
				)

		on SelectByWC changed state do
			if SelectByWC.checked then
				cWC.enabled = GetWC.enabled = true
			else
				cWC.enabled = GetWC.enabled = false

		on ByHeight changed state do
			if ByHeight.checked then
				ObjHeight.enabled = SelBy.enabled = true
			else
				ObjHeight.enabled = SelBy.enabled = false

		on ReadMind pressed do	
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
				if Method.selection == 1 do
					ObjArray = objects as array

				if Method.selection == 2 do	
					ObjArray = Selection as array

				if KeepSelection.checked == false do
					MAX Select None

				oc = ObjArray.count
				cnt = 0
				ProgressStart "Generating Selection..."
				for i = 1 to oc do
					(
						o = ObjArray[i]
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

		on GetWc picked obj do
			cWC.color = obj.wirecolor

		on PropagateInstances pressed do
			(
				KeyNameArray = #()
				for obj in selection do
					if findItem KeyNameArray (KeyName obj.name) == 0 do
						append KeyNameArray (KeyName obj.name)
				oc = (1.0 / objects.count * 100)
				cnt = 0
				ProgressStart "Generating Selection...."
				sel = #()
				for o in objects where o.isselected == false do
					(
						cnt += 1
						progressUpdate (oc * cnt)
						if findItem KeyNameArray (KeyName o.name) != 0 do
							(
								if o.isHidden do unhide o
								append Sel o
							)
					)
				unhide sel
				if (QueryBox "Propagated instances have been unhidden\n Do you want them selected for you?\n ...this may take a while.") do
					selectmore sel
				ProgressEnd()
			)

		on PropagateMasters pressed do
			(
				KeyNameArray = #()
				for obj in selection do
					if findItem KeyNameArray (KeyName obj.name) == 0 do
						append KeyNameArray (KeyName obj.name)
				oc = (1.0 / objects.count * 100)
				cnt = 0
				ProgressStart "Generating Selection...."
				sel = #()
				for o in objects where o.isselected == false do
					(
						cnt += 1
						progressUpdate (oc * cnt)
						if (findItem KeyNameArray (KeyName o.name) != 0) and (IsMaster o.name) do
							(
								if o.isHidden do unhide o
								append Sel o
							)
					)
				unhide sel
				if (QueryBox "Propagated Masters have been unhidden\n Do you want them selected for you?\n ...this may take a while.") do
					selectmore sel
				ProgressEnd()
			)

	)