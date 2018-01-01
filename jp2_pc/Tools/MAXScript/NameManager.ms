Utility NameManager "Name Manager"
	(
		label version "v1.0"
		group "Name Options:"
			(
				Radiobuttons ObjType "Object Type:" labels:#("Vegetation", "Animal", "Structure", "Props", "Terrain", "Other") align:#left columns:2
				edittext prefix "Prefix:" fieldwidth:80
				checkbutton prefixnum "#" offset:[60,-23]
				DropDownList delimiter "Delimiters:" items:#("","_", "+",":",";") fieldwidth:30
				edittext Suffix "Suffix:" fieldwidth:80
				checkbutton suffixnum "#" offset:[60,-23]
				checkbutton instanced "Instanced"
			)

		group "Other Processes"
			(
				dropDownList GameArea "Game Area:" items:#("Beach", "Jungle Road", "Platation House", "Industrial Jungle", "InGEN Town", "Plains", "PineValley", "Shore", "InGEN Lab", "Ascent", "Summit") selection:4 height:12
				spinner RotationValue "Rotation Value:" range:[1,3,1] type:#integer fieldwidth:35
				spinner DRLevel "Detail Level:" range:[0,5,1] type:#integer fieldwidth:35
				button ExtraProcess "Process Selection"
			)
		
			button renameObjects "Rename Selected" width:140
			
			on instanced changed state do
				if state do prefixnum.state = suffixnum.state = off
			
			on prefixnum changed state do
				instanced.state = off
			
			on suffixnum changed state do
				instanced.state = off
			
			on renameObjects pressed do
				(
					SelObjArray = for obj in selection collect obj
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
					for i = 1 to oc do
						(
							CObj = SelObjArray[i]
							if prefixnum.checked then PreNum = (i as string) else PreNum = ""
							if suffixnum.checked then SufNum = (i as string) else SufNum = ""
							if instanced.checked then Suffix2 = ("-" + i as string) else Suffix2 = ""
							CObj.name = (Prefix1 + Prefix.text + PreNum + DelimStr + Suffix.text + SufNum +Suffix2)
						)
				)

			on ExtraProcess pressed do
				(
					SelObjArray = for obj in selection collect obj
					oc = SelObjArray.count
					for i = 1 to oc do
						(
							CObj = SelObjArray[i]
							DashPos = 0
							for j = 1 to CObj.name.count do
								if CObj.name[j] == "-" do
									DashPos = j
							if DashPos != 0 do
								(
									MyPrefix = substring CObj.name 1 (DashPos - 1)
									MySuffix = substring CObj.name DashPos (CObj.name.count - DashPos)
									AreaKey = ""
									if GameArea.selection == 1 do (AreaKey = "BE")
									if GameArea.selection == 2 do (AreaKey = "JR")
									if GameArea.selection == 3 do (AreaKey = "PH")
									if GameArea.selection == 4 do (AreaKey = "IJ")
									if GameArea.selection == 5 do (AreaKey = "TO")
									if GameArea.selection == 6 do (AreaKey = "PL")
									if GameArea.selection == 7 do (AreaKey = "PV")
									if GameArea.selection == 8 do (AreaKey = "SH")
									if GameArea.selection == 9 do (AreaKey = "LA")
									if GameArea.selection == 10 do (AreaKey = "AS")
									if GameArea.selection == 11 do (AreaKey = "SU")
									Stuff = ("~" + AreaKey + (RotationValue.value as integer) as string + (DRLevel.value as integer) as string)
									ObjName = (MyPreFix + Stuff + MySuffix)
									CObj.name = ObjName
								)
						)
				)





	) -- end utility