filein "\\\JPII_PC\\Trespass\Art\\Tools\Scripts\SortFns.ms"
Utility FastDetectDups "Detect Dups"
	(
		radiobuttons SelFrom labels:#("All", "Selection")
		button detect "Detect"
		
		on detect pressed do
			(
				if SelFrom.state == 1 do
					ObjArray = objects as array

				if SelFrom.state == 2 do
					ObjArray = selection as array
				
				NameArray = for i = 1 to ObjArray.count collect ObjArray[i].name
			
				oc = ObjArray.count
				ocInv = (1.0 / oc) * 100

				ProgressStart "Sorting...."
				for i = 1 to NameArray.count do
					(
						ProgressUpdate (i * ocInv)
						min = i
						for j = 1 to NameArray.count do
							if NameArray[j] > NameArray[min] do
								(
									min = j
									t = NameArray[min]
									NameArray[min] = NameArray[i]
									NameArray[i] = t
				
									t = ObjArray[min]
									ObjArray[min] = ObjArray[i]
									ObjArray[i] = t
								)
					)
				ProgressEnd()
				SelArray = #()
				ProgressStart "Searching...."
				for i = 1 to oc do
					(
						ProgressUpdate (i * ocInv)
						if NameArray[i] == NameArray[i+1] do
							(
								append SelArray ObjArray[i]
								append SelArray ObjArray[i+1]
							)
					)
				if SelArray.count > 0 do
					select SelArray
				ProgressEnd()
				messageBox (SelArray.count as string + " objects selected")
			)
	)