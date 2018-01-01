Utility ObjMergeAssist "Obj Merge Assist"
	(
		local 	tmpNew = #(),
						tmpOld = #(),
						New = #(),
						Old = #()

		button Clear "Clear"
		
		group "Setup"
			(
				button GetSelOld "Get Sel Old" width:120
				label groupOldLabel "Old: 0 objects"
				button GetSelNew "Get Sel New" width:120
				label groupNewLabel "New: 0 objects"
			)
		group "Operations"
			(
				button SelectCommon "Select Common" width:140
				button CopyOldtoNew "Copy Props Old to New" width:140
				button CopyNewtoOld "Copy Props New to Old" width:140
				button DeleteOldfromNew "Delete Obj Old from New" width:140
				button DeleteNewfromOld "Delete Obj New from Old" width:140
			)
		
		on clear pressed do
			(
				tmpNew = #()
				tmpOld = #()
				New = #()
				Old = #()
				groupOldLabel.text = "Old: 0 objects"
				groupNewLabel.text = "New: 0 objects"
			)

		on SelectCommon pressed do
			(
				if (Old.count > 0) and (New.count > 0) then
					(
						sel = #()
						for i = 1 to Old.count do
							for j = 1 to New.count do
								if Old[i].name == New[j].name do
									append sel new[j]
						select sel
						messageBox ((sel.count as string) + " objects selected.")
					) else (
						messageBox "One or more groups are empty"
					)
			)
		
		on GetSelOld pressed do
			(
				tmpOld = selection as array
				if tmpOld.count > 0 then
					(
						Old = #()
						dup = false
						for i = 1 to tmpOld.count do
							if ((findItem Old tmpOld[i]) == 0) then
								append Old tmpOld[i]
							else
								dup = true
						if dup do (MessageBox "Some duplicate objects were found in group \"New\" and were not added to group \"Old\"" title:"Obj Merge Assist Warning")
						groupOldLabel.text = ("Old: " + Old.count as string + " objects")
					) else (
						MessageBox "Select something please"
					)
			)

		on GetSelNew pressed do
			(
				tmpNew = selection as array
				if tmpNew.count > 0 then
					(
						New = #()
						dup = false
						for i = 1 to tmpNew.count do
							if ((findItem Old tmpNew[i]) == 0) then
								append New tmpNew[i]
							else
								dup = true
						if dup do (MessageBox "Some duplicate objects were found in group \"Old\" and were not added to group \"New\"" title:"Obj Merge Assist Warning")
						groupNewLabel.text = ("New: " + New.count as string + " objects")
					) else (
						MessageBox "Select something please"
					)
			)

		on CopyOldtoNew pressed do
			(
				if (Old != #()) and (New != #()) then
					(
						for i = 1 to Old.count do
							(
								OldName = Old[i].name
								for j = 1 to New.count do
									(
										NewName = New[j].name
										if OldName == NewName do setUserPropBuffer New[j] (getUserPropBuffer Old[i])
									)
							)
					) else (
						MessageBox "One or more groups are not defined" title:"Obj Merge Assist Warning"
					)
			)

		on CopyNewtoOld pressed do
			(
				if (Old != #()) and (New != #()) then
					(
						for i = 1 to New.count do
							(
								NewName = New[i].name
								for j = 1 to Old.count do
									(
										OldName = Old[j].name
										if OldName == NewName do setUserPropBuffer Old[j] (getUserPropBuffer New[i])
									)
							)
					) else (
						MessageBox "One or more groups are not defined" title:"Obj Merge Assist Warning"
					)
			)

		on DeleteOldfromNew pressed do
			(
				if (Old != #()) and (New != #()) then
					(
						del = #()
						for i = 1 to New.count do
							(
								NewName = New[i].name
								for j = 1 to Old.count do
									(
										OldName = Old[j].name
										if OldName == NewName do append del Old[j]
									)
							)
						delete del
					) else (
						MessageBox "One or more groups are not defined" title:"Obj Merge Assist Warning"
					)
			)

		on DeleteNewfromOld pressed do
			(
				if (Old != #()) and (New != #()) then
					(
						del = #()
						for i = 1 to Old.count do
							(
								OldName = Old[i].name
								for j = 1 to New.count do
									(
										NewName = New[j].name
										if NewName == OldName do append del New[j]
									)
							)
						delete del
					) else (
						MessageBox "One or more groups are not defined" title:"Obj Merge Assist Warning"
					)
			)
	)