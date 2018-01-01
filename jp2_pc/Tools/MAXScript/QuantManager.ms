fn getTextureMaps verbose a = 
	(
		local maps = #()
		for i = 1 to a.count do
			(
				o = a[i]
				if o.mat != undefined do
					(
						if classOf o.mat == standardmaterial do
							if classOf o.mat.diffusemap == bitmaptexture do
								(
									CName = FileNamefromPath o.mat.diffusemap.filename
									if findItem maps CName == 0 do
										append Maps CName
								)

						if classOf o.mat == MultiMaterial do
							(
								IDs = for j = 1 to o.numfaces collect (getFaceMatID o j)
								ns = o.mat.numsubs
								for j = 1 to ns do
									if classOf o.mat[j].diffusemap == bitmaptexture do
										(
											CName = FileNamefromPath o.mat[j].diffusemap.filename
											if (findItem IDs j) != 0 then
												if findItem maps CName == 0 do
													append Maps CName
											else
												if verbose do format "Skipping map % because it is not used by object %\n" CName o.name
										)
							)
					)
			)
		return maps
	)

fn RemExtraNewLines Buffer = 
	(
		nIndex = 0
		if (nIndex = findString Buffer "\r\r\n\r\r\n") != undefined then
			(
				Buffer = (replace Buffer nIndex 6 "\r\r\n")
				RemExtraNewLines Buffer
			) else (
				if (nIndex = findString Buffer "\r\r\n\r\n") != undefined then
					(
						Buffer = (replace Buffer nIndex 5 "\r\r\n")
						RemExtraNewLines Buffer
					) else (
						if (nIndex = findString Buffer "\r\n\r\n") != undefined then
							(
								Buffer = (replace Buffer nIndex 4 "\r\r\n")
								RemExtraNewLines Buffer
							) else (
								if (nIndex = findString Buffer "\r\r\n") == 1 then
									(
										Buffer = (replace Buffer 1 3 "")
										RemExtraNewLines Buffer
									) else (
										return Buffer
									)
							)
					)
			)
	)

fn QueryBuffer b = 
	(
		for i = 1 to b.count do
			(
				if b[i] == "\r" do
					format "%:\\r\n" i
				if b[i] == "\n" do	
					format "%:\\n\n" i
				if b[i] != "\r" and b[i] != "\n" do
					format "%:%\n" i b[i]
			)
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
			if (substring CName (DashPos) 3) == "-00" then
				return true
			else
				return false
	)

Utility QuantManager "Quantization Manager"
	(
		local groupNames = #()
		
		local debug = true

		group "Setup"
			(
				listbox QuantGroups "Groups" height:5
				button remove "Remove" width:70 offset:[-37,0]
				button clear "Clear" width:70 offset:[37,-26]
				editText groupName "New Name:"	
			)

		button parseScene "Parse Scene" width:140
		button ApplySelection "Apply to Selection" width:140
		button RemoveQuantFromSelection "Remove from Selection" width:140

		group "Bitmap Collisions"
			(
				listbox QuantGroups1 height:2
				listbox QuantGroups2 height:2
				button getGroups "Get Groups" width:140
				button findCollisions "Find Collisions" width:140
				checkbox verbose "Verbose"
			)

		group "Selctions"
			(
				button SelByQuantGroup "Select by Group" width:140
				radiobuttons method labels:#("From Selection", "From Scene")	
			)

		on getGroups pressed do
			(
				NewItems = QuantGroups.items
				append NewItems "NO GROUP"
				QuantGroups1.items = QuantGroups2.items = NewItems
			)

		-- Find collisions between bitmaps used by a different quantization group
		on findCollisions pressed do
			(
				if QuantGroups1.selection != QuantGroups2.selection then
					(
						ObjArray = for o in geometry collect o
						oc = ObjArray.count
						ocInv = ((1.0 / oc) * 100)
						ProgressStart "Generating Selection..."
						group1 = #()
						group2 = #()
						-- get the objects used in the two quantization group
						for i = 1 to oc do
							(
								progressUpdate (ocInv * i)
								if (IsMaster ObjArray[i].name) do
									(
										CBuffer = getUserPropBuffer ObjArray[i]
										-- if we're looking for no quantization group
										if QuantGroups1.selection == QuantGroups1.items.count then
											(
												if findString CBuffer "#quant_group" == undefined do												
													append group1 ObjArray[i]
											) else (
												if findString CBuffer ("#quant_group(" + groupNames[QuantGroups1.selection] + ")") != undefined do
													append group1 ObjArray[i]
											)

										if QuantGroups2.selection == QuantGroups2.items.count then
											(
												if findString CBuffer "#quant_group" == undefined do												
													append group2 ObjArray[i]
											) else (
												if findString CBuffer ("#quant_group(" + groupNames[QuantGroups2.selection] + ")") != undefined do
													append group2 ObjArray[i]
											)
									)
							)
						ProgressEnd()
						if (group1.count > 0) and (group2.count > 0) do
							(
								g1Maps = getTextureMaps verbose.checked group1
								g2Maps = getTextureMaps verbose.checked group2
							)
						if debug do
							(
								format "g1Maps: %\n" g1maps.count
								format "g2Maps: %\n" g2maps.count
							)
						CollidingMaps = #()
						for i = 1 to g1Maps.count do
							(
								iMap = g1Maps[i]
								for j = 1 to g2Maps.count do
									if iMap == g2Maps[j] do
										(
											-- Collision!
											if FindItem collidingMaps iMap == 0 do
												append collidingMaps iMap
										)
							)

						if collidingMaps.count > 0 then
							for i = 1 to collidingMaps.count do
								format "Map Collision: %\n" collidingMaps[i]
						else
							format "No Map collisions...\n"
						-- gc()
					) else (
						messageBox "Same quant group specified in both lists!"
					)
			)
		
		on SelByQuantGroup pressed do
			(
				if method.state == 1 then
					ObjArray = for o in selection collect o
				else
					ObjArray = for o in geometry collect o

				oc = ObjArray.count
				ocInv = ((1.0 / oc) * 100)
				ProgressStart "Generating Selection..."
				sel = #()
				for i = 1 to oc do
					(
						progressUpdate (ocInv * i)
						CBuffer = getUserPropBuffer ObjArray[i]
						if findString CBuffer ("#quant_group(" + groupNames[QuantGroups.selection] + ")") != undefined do
							append sel ObjArray[i]
					
					)
				ProgressEnd()
				unhide Sel
				select Sel
				messagebox ((sel.count as string) + " Objects selected.")
				-- gc()
			)
		
		on groupName entered text do
			if groupName.text != "" do
				(
					qGroup = groupName.text
					if (findItem groupNames qGroup) == 0 do
						append groupNames qGroup
					QuantGroups.items = groupNames
					groupName.text = ""
					QuantGroups.selection = QuantGroups.items.count
				)

		on remove pressed do
			if groupNames.count != 0 do
				(
					deleteItem groupNames QuantGroups.selection
					QuantGroups.items = groupNames
				)

		on clear pressed do
			(
				groupNames = #()
				QuantGroups.items = groupNames
			)
		
		on RemoveQuantFromSelection pressed do
			(
				ObjArray = selection as array
				oc = ObjArray.count
				if oc > 0 then
					(
						for i = 1 to oc do
							(
								CBuffer = getUserPropBuffer ObjArray[i]
								if (qIndex = findString CBuffer "#quant_group(") != undefined do
									(
										print qIndex
										for j = qIndex to CBuffer.count do
											if CBuffer[j] == ")" do
												(
													CBuffer = (replace CBuffer qIndex (j - qIndex + 1) "")
													Exit
												)
										setUserPropBuffer ObjArray[i] (RemExtraNewLines CBuffer)
									)
							)
					) else (
						messageBox "umm... select something please"
					)
			)

		on ApplySelection pressed do
			(
				ObjArray = selection as array
				oc = ObjArray.count
				if oc > 0 then
					(
						for i = 1 to oc do
							if (IsMaster ObjArray[i].name) do
								(
									CBuffer = getUserPropBuffer ObjArray[i]
									if (qIndex = findString CBuffer "#quant_group") == undefined then
										(
											setUserPropBuffer ObjArray[i] (RemExtraNewLines (CBuffer + "\r\r\n#quant_group(" + groupNames[QuantGroups.selection] + ")\r\r\n"))
										) else (
											token = ""
											endIndex = -1
											for j = qIndex to CBuffer.count do
												if CBuffer[j] == ")" do
													(	
														endIndex = j
														Exit
													)
											if endIndex != -1 then
												(
													Token = (substring CBuffer (qIndex + 13) (endIndex - (qIndex + 13)))
													-- format "Token: %\n" token
												)
											-- token has been set? token is the currently selected one, there's no need to change it.
											if token != "" do
												if Token != groupNames[QuantGroups.selection] do
													(
														CBuffer = ("\r\r\n" + (replace CBuffer (qIndex + 13) token.count groupNames[QuantGroups.selection]))
														setUserPropBuffer ObjArray[i] (RemExtraNewLines  CBuffer)
													)
										)
								)
						) else (
							messageBox "umm... select something please"
						)
			)

		on ParseScene pressed do
			(
				oc = objects.count
				ocInv = ((1.0 / oc) * 100)
				i = 0
				ProgressStart "Generating Selection..."
				for o in objects where (isMaster o.name) do
					(
						i += 1
						progressUpdate (ocInv * i)
						CBuffer = getUserPropBuffer o
						if (qIndex = findString CBuffer "#quant_group") != undefined do
							(
								endIndex = -1
								for j = qIndex to CBuffer.count do
									if CBuffer[j] == ")" do
										(	
											endIndex = j
											Exit
										)
								if endIndex != -1 then
									(
										Token = (substring CBuffer (qIndex + 13) (endIndex - (qIndex + 13)))
										-- format "Token: %\n" token
										if findItem groupNames token == 0 do append groupNames token
									)
							)
					)
				QuantGroups.items = groupNames
				ProgressEnd()
				-- gc()
			)

	) -- End Utility