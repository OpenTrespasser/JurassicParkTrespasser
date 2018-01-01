fn intAsString int = 
	(
		if int >= 0 and int <= 9 then
			return ("0" + int as string)
		else
			return (int as string)
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

-- *********************************************************************
-- Start Utility
-- *********************************************************************

Utility NamingTools "Naming Tools"
	(
		local debug = false

		group "Name Manager"
			(
				Radiobuttons ObjType "Object Type:" labels:#("Vegetation", "Animal", "Structure", "Props", "Terrain", "Other") align:#left columns:2 default:6
				edittext prefix "Prefix:" fieldwidth:80
				checkbutton prefixnum "#" offset:[60,-23]
				DropDownList delimiter "Delimiters:" items:#("","_", "+",":",";") fieldwidth:30
				edittext Suffix "Suffix:" fieldwidth:80
				checkbutton suffixnum "#" offset:[60,-23]
				checkbutton instanced "Instanced" align:#left
				spinner InstanceStartNum range:[0,99999,0] type:#integer fieldwidth:50 offset:[0,-23] enabled:false
				label NamePreview
				checkbox SmartMode "Smart Mode"
				button renameObjects "Rename Selected" width:140
			)
		
		group "Search and Destroy"
			(
				radioButtons EditType labels:#("String", "Count")
				editText SearchFor "Search For:"
				spinner startPos type:#integer range:[1,100,1] enabled:false
				spinner Length type:#integer range:[1,100,1] enabled:false
				editText ReplaceWith "Replace With:"
				button RenameSelection "Rename Selection" width:140
			)
		
		group "Renaming from Maps"
			(
				checkbox Ren_Obj_chkbox "Rename Objects" 
				checkbox Ren_Multi_chkbox "Rename Multi/Subs"
				button Rename_Selected_btn "Rename Selected" width:135
			)
		
		group "Scene Merge Tools"
			(
				button ProcessSelection "Cleanup Selection" width:140
				spinner InstanceStartNum2 "Inst # Starts @:" range:[0,99999,0] type:#integer fieldwidth:45
				label status1
			)

		on ProcessSelection pressed do
			(
				KeyNameArray = #()
				NewObjArray = #()
				ObjArray = Selection as array
				oc = ObjArray.count
				-- Make a list of the keynames of Master objects found
				status1.text = "Finding Keynames..."
				for i = 1 to oc do
					(
						CName = ObjArray[i].name
						CKeyName = Keyname CName
						if (findItem KeyNameArray CKeyName) == 0 do
							if (substring CName (CName.count - 2) 3) == "-00" do
								(
									append NewObjArray #(ObjArray[i])
									append KeyNameArray CKeyName
								)
					)

				status1.text = "Filling object array..."
				for i = 1 to oc do
					(
						CName = ObjArray[i].name
						CKeyName = Keyname CName
						-- Do we have the keyname?
						if (CIndex = findItem KeynameArray CKeyName) != 0 do
							if (findItem NewObjArray[CIndex] ObjArray[i]) == 0 do
								append NewObjArray[CIndex] ObjArray[i]
					)

				status1.text = "Renaming Objects..."
				for i = 1 to KeyNameArray.count do
					(
						CPrefix = KeyNameArray[i]
						for j = 1 to NewObjArray[i].count do
							(
								if j == 1 then
									IncrementNum = 0
								else
									IncrementNum = (InstanceStartNum2.value + j - 1)
								Csuffix = ("-" + (intAsString IncrementNum))
								NewObjArray[i][j].name = CPrefix + CSuffix
							)
					)
				status1.text = ""
				KeyNameArray = NewObjArray = ObjArray = oc = CName = CKeyName = undefined
			)


on RenameSelection pressed do
	(
		Objarray = for obj in selection collect obj
		format "ObjArray = %\n" Objarray
		for i = 1 to ObjArray.count do
			(
				CObj = ObjArray[i]
				CName = CObj.name
				NewName = "undefined"
				if EditType.state == 1 then
					(
						SearchString = SearchFor.text
						-- we're in string mode
						if (CIndex = findString CName SearchString) != undefined then
							NewName = (replace CName CIndex SearchString.count ReplaceWith.text)
						else
							NewName = CObj.name
					) else (
						CIndex = startPos.value
						-- we're in count mode
						NewName = (replace CName CIndex Length.value ReplaceWith.text)
					)
				CObj.name = NewName
			)
	)

on EditType changed state do
	if EditType.state == 1 then
		(
			SearchFor.enabled = true
			startPos.enabled = Length.enabled = false
		) else (
			SearchFor.enabled = false
			startPos.enabled = Length.enabled = true
		)
		

on Rename_Selected_btn pressed do
	(
		undo on
			(
				Sel_Object_count = 0;
				for o in objects do 
					(
						if o.isselected then
							(
								Sel_Object_count = (Sel_Object_Count +1);
								if classof o.mat == multimaterial do 
									(
										for i = 1 to o.mat.numsubs do
											(
												if ClassOf o.mat[i].diffusemap == bitmaptexture do
													(
														diffuse_map_name = (FileNameFromPath o.mat[i].diffusemap.filename);
														mat_name_length = (diffuse_map_name.count - 6);
														single_mat_name = (substring diffuse_map_name 1 mat_name_length);
														o.mat[i].name = single_mat_name;
														o.mat[i].diffusemap.name = diffuse_map_name;
													)
			
												if ClassOf o.mat[i].opacitymap == bitmaptexture do (o.mat[i].opacitymap.name = (FileNameFromPath o.mat[i].opacitymap.filename))
												if ClassOf o.mat[i].bumpmap == bitmaptexture then (o.mat[i].bumpmap.name = (FileNameFromPath o.mat[i].bumpmap.filename))
			
												-- hack for object name
												diffuse_map_name = (FileNameFromPath o.mat[1].diffusemap.filename);
												mat_name_length = (diffuse_map_name.count - 6);
												single_mat_name = (substring diffuse_map_name 1 mat_name_length);
												Object_name_tag = "-00";
												Object_name = (substring single_mat_name 1 (single_mat_name.count - 2));
												if Sel_Object_Count <= 9 do (NameCount = ("0" + Sel_Object_Count as string))
												if Sel_Object_Count >= 10 do (NameCount = Sel_Object_Count as string)
												if Ren_Obj_chkbox.checked do (o.name = (Object_name + NameCount + Object_name_tag));
												if Ren_Multi_chkbox.checked do (o.mat.name = (Object_name + NameCount));
											)
									)

								if classof o.mat == standardmaterial do
									(
										diffuse_map_name = (FileNameFromPath o.mat.diffusemap.filename)
										mat_name_length = (diffuse_map_name.count - 6)
										single_mat_name = (substring diffuse_map_name 1 mat_name_length)
										o.mat.name = single_mat_name
										o.mat.diffusemap.name = diffuse_map_name
										o.name = (o.mat.name + "-00")
									)
							)
					)
			)
	)


on ObjType changed state do
	(
		if ObjType.state == 1 do prefix1 = "V"
		if ObjType.state == 2 do prefix1 = "A"
		if ObjType.state == 3 do prefix1 = "S"
		if ObjType.state == 4 do prefix1 = "P"
		if ObjType.state == 5 do prefix1 = "T"
		if ObjType.state == 6 do prefix1 = ""
		if delimiter.selection == 1 do DelimStr = ""
		if delimiter.selection == 2 do DelimStr = "_"
		if delimiter.selection == 3 do DelimStr = "+"
		if delimiter.selection == 4 do DelimStr = ":"
		if delimiter.selection == 5 do DelimStr = ";"
		if prefixnum.checked then PreNum = "#" else PreNum = ""
		if suffixnum.checked then SufNum = "#" else SufNum = ""
		if instanced.checked then Suffix2 = "-##" else Suffix2 = ""
		NamePreview.text = (Prefix1 + Prefix.text + PreNum + DelimStr + Suffix.text + SufNum + Suffix2)
	)
	
on prefix entered text do	
	(
		if ObjType.state == 1 do prefix1 = "V"
		if ObjType.state == 2 do prefix1 = "A"
		if ObjType.state == 3 do prefix1 = "S"
		if ObjType.state == 4 do prefix1 = "P"
		if ObjType.state == 5 do prefix1 = "T"
		if ObjType.state == 6 do prefix1 = ""
		if delimiter.selection == 1 do DelimStr = ""
		if delimiter.selection == 2 do DelimStr = "_"
		if delimiter.selection == 3 do DelimStr = "+"
		if delimiter.selection == 4 do DelimStr = ":"
		if delimiter.selection == 5 do DelimStr = ";"
		if prefixnum.checked then PreNum = "#" else PreNum = ""
		if suffixnum.checked then SufNum = "#" else SufNum = ""
		if instanced.checked then Suffix2 = "-##" else Suffix2 = ""
		NamePreview.text = (Prefix1 + Prefix.text + PreNum + DelimStr + Suffix.text + SufNum + Suffix2)
	)

on prefixnum changed state do
	(
		if ObjType.state == 1 do prefix1 = "V"
		if ObjType.state == 2 do prefix1 = "A"
		if ObjType.state == 3 do prefix1 = "S"
		if ObjType.state == 4 do prefix1 = "P"
		if ObjType.state == 5 do prefix1 = "T"
		if ObjType.state == 6 do prefix1 = ""
		if delimiter.selection == 1 do DelimStr = ""
		if delimiter.selection == 2 do DelimStr = "_"
		if delimiter.selection == 3 do DelimStr = "+"
		if delimiter.selection == 4 do DelimStr = ":"
		if delimiter.selection == 5 do DelimStr = ";"
		if prefixnum.checked then PreNum = "#" else PreNum = ""
		if suffixnum.checked then SufNum = "#" else SufNum = ""
		if instanced.checked then Suffix2 = "-##" else Suffix2 = ""
		NamePreview.text = (Prefix1 + Prefix.text + PreNum + DelimStr + Suffix.text + SufNum + Suffix2)
	)

on delimiter selected i do
	(
		if ObjType.state == 1 do prefix1 = "V"
		if ObjType.state == 2 do prefix1 = "A"
		if ObjType.state == 3 do prefix1 = "S"
		if ObjType.state == 4 do prefix1 = "P"
		if ObjType.state == 5 do prefix1 = "T"
		if ObjType.state == 6 do prefix1 = ""
		if delimiter.selection == 1 do DelimStr = ""
		if delimiter.selection == 2 do DelimStr = "_"
		if delimiter.selection == 3 do DelimStr = "+"
		if delimiter.selection == 4 do DelimStr = ":"
		if delimiter.selection == 5 do DelimStr = ";"
		if prefixnum.checked then PreNum = "#" else PreNum = ""
		if suffixnum.checked then SufNum = "#" else SufNum = ""
		if instanced.checked then Suffix2 = "-##" else Suffix2 = ""
		NamePreview.text = (Prefix1 + Prefix.text + PreNum + DelimStr + Suffix.text + SufNum + Suffix2)
	)

on Suffix entered text do
	(
		if ObjType.state == 1 do prefix1 = "V"
		if ObjType.state == 2 do prefix1 = "A"
		if ObjType.state == 3 do prefix1 = "S"
		if ObjType.state == 4 do prefix1 = "P"
		if ObjType.state == 5 do prefix1 = "T"
		if ObjType.state == 6 do prefix1 = ""
		if delimiter.selection == 1 do DelimStr = ""
		if delimiter.selection == 2 do DelimStr = "_"
		if delimiter.selection == 3 do DelimStr = "+"
		if delimiter.selection == 4 do DelimStr = ":"
		if delimiter.selection == 5 do DelimStr = ";"
		if prefixnum.checked then PreNum = "#" else PreNum = ""
		if suffixnum.checked then SufNum = "#" else SufNum = ""
		if instanced.checked then Suffix2 = "-##" else Suffix2 = ""
		NamePreview.text = (Prefix1 + Prefix.text + PreNum + DelimStr + Suffix.text + SufNum + Suffix2)
	)

on suffixnum changed state do
	(
		if ObjType.state == 1 do prefix1 = "V"
		if ObjType.state == 2 do prefix1 = "A"
		if ObjType.state == 3 do prefix1 = "S"
		if ObjType.state == 4 do prefix1 = "P"
		if ObjType.state == 5 do prefix1 = "T"
		if ObjType.state == 6 do prefix1 = ""
		if delimiter.selection == 1 do DelimStr = ""
		if delimiter.selection == 2 do DelimStr = "_"
		if delimiter.selection == 3 do DelimStr = "+"
		if delimiter.selection == 4 do DelimStr = ":"
		if delimiter.selection == 5 do DelimStr = ";"
		if prefixnum.checked then PreNum = "#" else PreNum = ""
		if suffixnum.checked then SufNum = "#" else SufNum = ""
		if instanced.checked then Suffix2 = "-##" else Suffix2 = ""
		NamePreview.text = (Prefix1 + Prefix.text + PreNum + DelimStr + Suffix.text + SufNum + Suffix2)
	)
	
on instanced changed state do
	(
		if instanced.checked then
			InstanceStartNum.enabled = true
		else
			InstanceStartNum.enabled = false
		if ObjType.state == 1 do prefix1 = "V"
		if ObjType.state == 2 do prefix1 = "A"
		if ObjType.state == 3 do prefix1 = "S"
		if ObjType.state == 4 do prefix1 = "P"
		if ObjType.state == 5 do prefix1 = "T"
		if ObjType.state == 6 do prefix1 = ""
		if delimiter.selection == 1 do DelimStr = ""
		if delimiter.selection == 2 do DelimStr = "_"
		if delimiter.selection == 3 do DelimStr = "+"
		if delimiter.selection == 4 do DelimStr = ":"
		if delimiter.selection == 5 do DelimStr = ";"
		if prefixnum.checked then PreNum = "#" else PreNum = ""
		if suffixnum.checked then SufNum = "#" else SufNum = ""
		if instanced.checked then Suffix2 = "-##" else Suffix2 = ""
		NamePreview.text = (Prefix1 + Prefix.text + PreNum + DelimStr + Suffix.text + SufNum + Suffix2)
	)

on renameObjects pressed do
	(
		undo on
		(
			ExitCode = false
			if SmartMode.checked then
				(
					SelObjArray = #()
					for obj in selection do
						(
							if (substring obj.name (obj.name.count - 2) 3) == "-00" do
								append SelObjArray obj
						)
					if SelObjArray.count == 1 then
						(
							for obj in selection do
								if obj != SelObjArray[1] do
									append SelObjArray obj
						) else (
							if (QueryBox "Smart Mode requires that only one object\nin your selection to contain the '-00' extension.\n\nPress Yes to set those objects to your current\nMAX selection and exit,or press No to exit.") == true then
								(
									MAX select none
									for i = 1 to SelObjArray.count do selectmore SelObjArray[i]
									ExitCode = true
								) else (
									ExitCode = true
								)
						)
				) else (
					SelObjArray = for obj in selection collect obj
				)
			if ExitCode == false do
				(
					oc = SelObjArray.count
					if ObjType.state == 1 do prefix1 = "V"
					if ObjType.state == 2 do prefix1 = "A"
					if ObjType.state == 3 do prefix1 = "S"
					if ObjType.state == 4 do prefix1 = "P"
					if ObjType.state == 5 do prefix1 = "T"
					if ObjType.state == 6 do prefix1 = ""
					if delimiter.selection == 1 do DelimStr = ""
					if delimiter.selection == 2 do DelimStr = "_"
					if delimiter.selection == 3 do DelimStr = "+"
					if delimiter.selection == 4 do DelimStr = ":"
					if delimiter.selection == 5 do DelimStr = ";"
					for i = 0 to (oc - 1) do
					(
						CObj = SelObjArray[i + 1]
						if prefixnum.checked then PreNum = (i as string) else PreNum = ""
						if suffixnum.checked then SufNum = (i as string) else SufNum = ""
						if instanced.checked then
							(
								InstNum = (InstanceStartNum.value + i)
								if prefixnum.checked or suffixnum.checked then
									Suffix2 = "-00"
								else
									if InstNum <= 9 then
										Suffix2 = ("-0" + InstNum as string)
									else
										Suffix2 = ("-" + InstNum as string)
							) else (
								Suffix2 = ""
							)
						CObj.name = (Prefix1 + Prefix.text + PreNum + DelimStr + Suffix.text + SufNum + Suffix2)
					)
				)
		)
	)

) -- end utility