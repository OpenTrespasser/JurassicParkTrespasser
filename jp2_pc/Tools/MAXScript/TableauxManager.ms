-- *********************************************************
-- Function Definitions
-- *********************************************************
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

fn FindObject nm = 
	(
		for o in objects do
			if o.name == nm do
				return o
		return undefined
	)

fn cpTransform a b = 
	(
		b.rotation = a.rotation
		b.scale = a.scale
		b.pos = a.pos
	)

fn UnifyObjectArray foo a = 
(
	-- find the sub array with that largest count
	local HighCount = -1
	for i = 1 to a.count do
		if a[i].count > HighCount do
			HighCount = a[i].count
	
	for i = 1 to a.count do
		if a[i].count < HighCount do
		(
			cnt =  a[i].count
			for j = a[i].count to HighCount do
				a[i][j] = a[i][cnt]
		)
	return a
)

fn PruneObjectArray foo a = 
	(
		local b = #()
		local UsedKeyNames = #()
		for i = 1 to a.count do
			if (findItem UsedKeyNames (KeyName a[i][1].name)) == 0 do -- If !found
				(
					append b a[i]
					append UsedKeyNames (KeyName a[i][1].name)
				)
		return b
	)

-- *********************************************************
-- Start Utility
-- *********************************************************
Utility TableauManager "Tableau Manager"
	(
		local TableauRollout,
			SourceObjectArray = #(),
			SourceObjNames = #(),
			TableauObjectArray = #(),
			TableauObjNames = #(),
			DetailStrings = #("Detail1", "Detail2", "Detail3", "Detail4"),
			wc = #((color 255 0 255), (color 255 255 0), (color 0 255 255), (color 0 255 0), (color 0 0 255))

		local debug = false

		group "Source Initialization"
			(
				listbox SourceObjects "Prelit Objects:" height:6
				button AddSource "Add" width:70 offset:[-37,0]
				button RemoveSource "Remove" width:70 offset:[37,-26]
				button clearSource "Clear" width:70 offset:[-37,0]
				listbox TableauObjects "Tableau Objects:" height:6
				button AddTableau "Add" width:70 offset:[-37,0]
				button RemoveTableau "Remove" width:70 offset:[37,-26]
				button clearTableau "Clear" width:70 offset:[-37,0]
				checkbox AutoClear "AutoClear"
				checkbox DeleteOld "Delete Old Tableaux"
			)
		
		button GenerateNewTableau "Generate New Tableaux" width:140 height:30

		group "Selection Generation"
			(
				spinner DRLevel "Detail Level:" range:[0,4,0] type:#integer
				button SelbyDRLevel "Select"
			)

		on SelbyDRLevel pressed do
			(
				MAX Select none
				swc = wc[DRLevel.value + 1]
				for o in objects do
					if o.wirecolor == swc do
						selectmore o
			)
	
		on AddSource pressed do
			(
				ObjArray = selection as array
				for i = 1 to ObjArray.count do
					(
						-- Make sure it's not in one of the lists
						if (findItem SourceObjectArray ObjArray[i]) == 0 and (findItem TableauObjectArray ObjArray[i]) == 0 do
							(
								-- Add it to the lists
								append SourceObjectArray ObjArray[i]
								append SourceObjNames ObjArray[i].name
							)
					)
				-- Update the listbox interface
				SourceObjects.items = SourceObjNames
			)

		-- Remove the selected element....
		on RemoveSource pressed do
			(
				-- Is there something in the listbox?
				if SourceObjects.items.count > 0 do
					(
						s = SourceObjects.selection
						-- temporary arrays
						TempArray = #()
						TempNames = #()
						for i = 1 to SourceObjects.items.count do
							if i != s do
								(
									append TempArray SourceObjectArray[i]
									append TempNames SourceObjectArray[i].name
								)
						-- Update the data and the interface
						SourceObjectArray = TempArray
						SourceObjNames = TempNames
						SourceObjects.items = SourceObjNames
					)
			)

		-- Clear the listbox and it's data
		on ClearSource pressed do
			(
				SourceObjNames = #()
				SourceObjectArray = #()
				SourceObjects.items = #()
			)

		on AddTableau pressed do
			(
				ObjArray = selection as array
				for i = 1 to ObjArray.count do
					(
						-- Make sure it's not in one of the lists
						if (findItem TableauObjectArray ObjArray[i]) == 0 and (findItem SourceObjectArray ObjArray[i]) == 0 do
							(
								-- Add it to the lists
								append TableauObjectArray ObjArray[i]
								append TableauObjNames ObjArray[i].name
							)
					)
				-- Update the listbox interface
				TableauObjects.items = TableauObjNames
			)

		-- Remove the selected element....
		on RemoveTableau pressed do
			(
				-- Is there something in the listbox?
				if TableauObjects.items.count > 0 do
					(
						s = TableauObjects.selection
						-- temporary arrays
						TempArray = #()
						TempNames = #()
						for i = 1 to TableauObjects.items.count do
							if i != s do
								(
									append TempArray TableauObjectArray[i]
									append TempNames TableauObjectArray[i].name
								)
						-- Update the data and the interface
						TableauObjectArray = TempArray
						TableauObjNames = TempNames
						TableauObjects.items = TableauObjNames
					)
			)

		-- Clear the listbox and it's data
		on ClearTableau pressed do
			(
				TableauObjNames = #()
				TableauObjectArray = #()
				TableauObjects.items = #()
			)

		on GenerateNewTableau pressed do
			(
				FinalObjects = #()
				if debug do format "Debug Mode Tableau Manager @ %\n" localtime -- Start Time
				UsedTableaux = for i = 1 to TableauObjectArray.count collect false
				if TableauObjectArray.count > 0 and SourceObjectArray.count > 0 do
				(
					ProgressStart "Processing objects..."
					tc = (1.0 / TableauObjectArray.count) * 100
					-- gain the data about the object we will have and the objects we won't
					for i = 1 to TableauObjectArray.count do
					(
						ProgressUpdate (i * tc)
						TableauKeyName = KeyName TableauObjectArray[i].name
						CObjects = #()
						for j = 1 to SourceObjectArray.count do
						(
							SourceKeyName = KeyName SourceObjectArray[j].name
							if TableauKeyname == SourceKeyName do
							(
								UsedTableaux[i] = true
								if debug do format "Found Match...%\n" SourceObjectArray[j].name
								CObjects[1] = SourceObjectArray[j]
								CBuffer = getUserPropBuffer SourceObjectArray[j]
								for k = 1 to 4 do
								(
									if debug do format "k = %\n" k
									EndIndex = -1
									-- Check for the Detail<#> text prop
									if debug do format "Checking for %\n" DetailStrings[k]
									if (CIndex = findString CBuffer DetailStrings[k]) != undefined then
									(
										if debug do format "% found...\n" DetailStrings[k]
										for l = CIndex to CBuffer.count do
											if CBuffer[l] == ";" do
											(
												EndIndex = l
												Exit
											)
										if EndIndex != -1 do
										(
											sDetailName = (substring CBuffer (CIndex + 11) ((EndIndex - 1) - (CIndex + 11)))
											if debug do format "sDetailName = %\n" sDetailName
											if (oDetail = FindObject SDetailName) != undefined then
											(
												CObjects[k + 1] = oDetail
											) else (
												if debug do format "Detail reduced object not found... Object \"%\"\n" SDetailName
											)
										)
									) else (
										if debug do format "% was not found, Exiting loop.\n" DetailStrings[k]
										Exit
									)
								)
							)
						)
						if CObjects.count != 0 do
						(
							append FinalObjects CObjects
						)
					)
					ProgressEnd()

					if debug do
						for i = 1 to FinalObjects.count do
						(
							a = for j = 1 to FinalObjects[i].count collect FinalObjects[i][j].name
							format "%: %\n" i a
						)
			
					FinalObjects = UnifyObjectArray true FinalObjects
					FinalObjects = PruneObjectArray true FinalObjects
					-- set tha maximun detail reduction level for the tableau based on the sub array lengths
					MaxDRLevel = FinalObjects[1].count 
					ProgressStart "Replacing Tableaux......"
					tc = (1.0 / TableauObjectArray.count) * 100
					for i = 1 to TableauObjectArray.count do
					(
						ProgressUpdate (tc * i)
						TableauKeyName = KeyName TableauObjectArray[i].name
						for j = 1 to FinalObjects.count do
							if (KeyName FinalObjects[j][1].name) == TableauKeyName do
								for k = 1 to MaxDRLevel do
								(
									-- copy the object
									oTemp = copy FinalObjects[j][k]
									-- copy the transform
									cpTransform TableauObjectArray[i] oTemp
									-- set the new wire color
									oTemp.wirecolor = wc[k]
								)
					)
					ProgressEnd()
				)
			if DeleteOld.checked do
				(
					for i = 1 to UsedTableaux.count do
						if UsedTableaux[i] == true do
							delete TableauObjectArray[i]
				)

			if AutoClear.checked do
			(
				TableauObjNames = #()
				TableauObjectArray = #()
				TableauObjects.items = #()
				SourceObjNames = #()
				SourceObjectArray = #()
				SourceObjects.items = #()
			)
			-- make copies of detail reduced versions of objects to new locations
			if debug do format "Debug Mode Tableau Manager Finished @ %\n\n" localtime
			)
	)-- Utility End
