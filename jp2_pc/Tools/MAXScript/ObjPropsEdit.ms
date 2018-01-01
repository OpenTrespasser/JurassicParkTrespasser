fn simplify b kwd acr = 
	(
		if (CIndex = findString b kwd) != undefined then
			(
				b = replace b CIndex kwd.count acr
				simplify b kwd acr
			) else (
				return b
			)
	)

fn simplifyProps o = 
	(
		local CBuffer = getUserPropBuffer o
		if CBuffer.count > 0 do
			(
				CBuffer = simplify CBuffer "string" "s"
				CBuffer = simplify CBuffer "int" "i"
				CBuffer = simplify CBuffer "bool" "b"
				CBuffer = simplify CBuffer "float" "f"
				CBuffer = simplify CBuffer "object" "o"
				setUserPropBuffer o CBuffer
			)
	)

fn OPWordArray textline = 
	(
		local c
		local Line = ""
		local Lines = #()
		for i = 1 to textline.count do
			(
				c = textline[i]
				if c != ";" then
					if (c != "\n") and (c != "\r") do
						Line = Line + c
				else
					(
						Line = Line + ";"
						append Lines Line
						Line = ""
					)
			)
		return Lines
	)

Utility ObjPropsEditing "Obj Props Editing"
	(
		local CEditObject

		group "Object Properties Editing"
		(
			Label EditingObjectLabel "Object:" align:#left
			pickbutton ChooseEditObject width:100 offset:[20,-20]
			ListBox CObjPropsList
			edittext CLineText fieldwidth:143 offset:[-7,0]
			button AddNewLine "Add" width:70 offset:[-37,0]
			button RemoveLine "Remove" width:70 offset:[37,-26]
			button ApplyToSelection "Apply to Selection" width:140
			button AppendToSelection "Append to Selection" width:140
			label PropsCount
			
		)

		group "Parsing Options"
			(
				edittext SearchString "Search for: "
				editText ReplaceString "Replace value: "
				button ParseSelection "Parse Selection"
			)

		group "Search and Replace"
			(
				edittext GeneralSearchString "Search for: "
				editText GeneralReplaceString "Replace value: "
				button SearchReplace "Search & Replace"
			)

		group "Simplification"
			(
				button SimplifySelection "Simplify Selection"
			)

		on SimplifySelection pressed do
			for o in selection do simplifyProps o
		
		on SearchReplace pressed do
			(
				ObjArray = for obj in selection collect obj
				oc = ObjArray.count
				SearchText = GeneralSearchString.text
				stringLen = SearchText.count
				for i = 1 to oc do
					(
						CObj = ObjArray[i]
						if (CBuffer = getUserPropBuffer CObj) != undefined do
							(
								CIndex = undefined
								if (CIndex = findString CBuffer SearchText) != undefined then
									(
										CBuffer = (replace CBuffer CIndex stringLen GeneralReplaceString.text)
										setUserPropBuffer CObj CBuffer
									) else (
										format ("Could not find the search keyword \"" + SearchText + "\" in object " + CObj.name + "\n")
									)
							)
					)
			)

		on AddNewLine pressed do
			(
				CString = ""
				ListBoxtext = CObjPropsList.items
				append ListBoxtext ""
				CObjPropsList.items = ListBoxtext
				for i = 1 to ListBoxText.count do (CString = (CString + ListBoxText[i] + "\r\r\n"))
				CLineText.text = CObjPropsList.items[CObjPropsList.selection]
				if CEditObject != undefined do
					SetUserPropBuffer CEditObject CString
				PropsCount.text = (CString.count as string + " characters in buffer")
			)

		on RemoveLine pressed do
			(
				CString = ""
				s = CObjPropsList.selection
				NewListBoxItems = #()
				ListBoxtext = CObjPropsList.items
				for i = 1 to ListBoxtext.count do
					if i != s do append NewListBoxItems ListBoxtext[i]
				CObjPropsList.items = NewListBoxItems
				if CObjPropsList.selection > CObjPropsList.items.count do
					(
						CObjPropsList.selection -= 1
						if CObjPropsList.selection < 1 do CObjPropsList.selection = 1
					)
				if CObjPropsList.items[CObjPropsList.selection] != undefined then
					CLineText.text = CObjPropsList.items[CObjPropsList.selection]
				else
					CLineText.text = ""
				for i = 1 to NewListBoxItems.count do (CString = (CString + NewListBoxItems[i] + "\r\r\n"))
				if CEditObject != undefined do
					SetUserPropBuffer CEditObject CString
				PropsCount.text = (CString.count as string + " characters in buffer")
			)

		on AppendToSelection pressed do
			(
				ObjArray = for obj in selection collect obj
				CString = ""
				ListBoxtext = CObjPropsList.items
				for i = 1 to ListBoxText.count do (CString = (CString + ListBoxText[i] + "\r\r\n"))
				for i = 1 to ObjArray.count do
					(
						CObj = ObjArray[i]
						CBuffer = getUserPropBuffer CObj
						CBuffer += CString
						SetUserPropBuffer CObj CBuffer
						PropsCount.text = (CBuffer.count as string + " characters in buffer")			
					)
			)

		on ApplyToSelection pressed do
			(
				ObjArray = for obj in selection collect obj
				CString = ""
				ListBoxtext = CObjPropsList.items
				for i = 1 to ListBoxText.count do (CString = (CString + ListBoxText[i] + "\r\r\n"))
				for i = 1 to ObjArray.count do
					(
						CObj = ObjArray[i]
						SetUserPropBuffer CObj CString
						PropsCount.text = (CString.count as string + " characters in buffer")			
					)
			)

		on ChooseEditObject picked obj do
			(
				CEditObject = obj
				if (CEditObjPropBuffer = getUserPropBuffer CEditObject).count != 0 then
					(
						LineArray = OPWordArray CEditObjPropBuffer 
						CObjPropsList.items = LineArray
						CObjPropsList.selection = 1
						CLineText.text = CObjPropsList.items[1]
						PropsCount.text = (CEditObjPropBuffer.count as string + " characters in buffer")
						ChooseEditObject.text = CEditObject.name
					) else (
						MessageBox "Nothing in this properties buffer."
					)
			)

		on CObjPropsList selected s do
			CLineText.text = CObjPropsList.items[s]

		on CLineText entered text do
			(
				s = CObjPropsList.selection
				ListBoxtext = CObjPropsList.items
				ListBoxtext[s] = text
				CObjPropsList.items = ListBoxtext
				ListBoxText = CObjPropsList.items
				CString = ""
				for i = 1 to ListBoxText.count do (CString = (CString + ListBoxText[i] + "\r\r\n"))
				if CEditObject != undefined do
					SetUserPropBuffer CEditObject CString
				PropsCount.text = (CString.count as string + " characters in buffer")
			)
	
		on ParseSelection pressed do
			(
				ObjArray = for obj in selection collect obj
				oc = ObjArray.count
				SearchText = SearchString.text
				for i = 1 to oc do
					(
						CObj = ObjArray[i]
						if (CBuffer = getUserPropBuffer CObj) != undefined do
							CIndex = undefined
							if (CIndex = findString CBuffer SearchString.text) != undefined then
								(
									CIndex = (CIndex + 3 + SearchText.count)
									NewBuffer = substring CBuffer CIndex (CBuffer.count - CIndex)
									CutOffPoint = 0
									ExitCode = false
									for j = 1 to NewBuffer.count do
										if NewBuffer[j] == ";" do 
											if ExitCode != true do
												(
													CutOffPoint = (j - 1)
													ExitCode = true
												)
									format "CutOffPoint:%\n" CutOffPoint
									format "Replacing:%\n" (substring CBuffer CIndex CutOffPoint)
									CBuffer = (replace CBuffer CIndex CutOffPoint ReplaceString.text)
									setUserPropBuffer CObj CBuffer
								) else (
									format ("Could not find the search keyword \"" + SearchString.text + "\" in object " + CObj.name + "\n")
								)
					)
			)
	)