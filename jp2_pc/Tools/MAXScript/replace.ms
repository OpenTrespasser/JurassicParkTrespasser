-- **********************************************************************************************
-- *
-- * Copyright © DreamWorks Interactive, 1997
-- *
-- * Contents:
-- *		Implementation of replace.ms.ms 
-- *
-- * Bugs:
-- *
-- * To do:
-- *
-- * Notes:
-- *
-- **********************************************************************************************

Utility ReplaceObj "Replace"
(
	local ReplaceObject, Sel, ReplWireColor;
	
	Group "Replace What?"
		(
			radiobuttons replaceRadio labels:#("All Objects", "Selected Objects") default:2
		)
	
	group "Options"
		(
			Checkbox migrateRotation "Keep Rotation" checked:true
			Checkbox MigrateScale "Keep Scale" checked:true
			Checkbox KeepWire "Keep Wire Color?" checked:true
			Checkbox DeleteOld "Delete Old Objects?" checked:true
			label replacelable1 "Clone as:" align:#left
			radiobuttons CopyType labels:#("Copy", "Instance") default:2
		)
	
		Label replacelabel2 "---Pick Source Object---"
		pickbutton GetNewObject "-- none --" width:135
		button ReplaceObjects "Replace" width:135
		button Replace2 "Fast Replace" width:135
	
	group "Status" (label status1 "")
	
	on GetNewObject picked obj do
		(
			ReplaceObject = obj
			GetNewObject.text = ReplaceObject.name
			ReplWireColor = ReplaceObject.wirecolor as point3
			format "Wire Color: %\n" ReplWireColor
		)

	on Replace2 pressed do
		(
			if ReplaceObject != undefined then
				(
					wc = ReplaceObject.wirecolor
		
					if replaceRadio.state == 1 then
						ObjArray = objects as array
					else
						ObjArray = selection as array
		
					oc = ObjArray.count
					ObjData = #()
					for i = 1 to oc do
						(
							CObj = ObjArray[i]
							TmpData = #()
							TmpData[1] = ObjArray[i].rotation
							TmpData[2] = ObjArray[i].scale
							TmpData[3] = ObjArray[i].pos
							append ObjData TmpData
						)
					if DeleteOld.checked do MAX delete
					for i = 1 to oc do
						(
							Status1.text  = ("Replacing object " + i as string + " of " + oc as string)
							if CopyType.state == 1 then
								NObj = copy ReplaceObject
							else
								NObj = Instance ReplaceObject
		
							if MigrateRotation.checked do
								NObj.rotation = ObjData[i][1]
		
							if MigrateScale.checked do
								NObj.scale = ObjData[i][2]
		
							NObj.pos = ObjData[i][3]
							if KeepWire.checked do NObj.wirecolor = wc
						)
				) else (
					messageBox "Must define the source object!"
				)
		)

	on replaceObjects pressed do
		(
			if ReplaceObject == undefined then exit
			if replaceradio.state == 1 do (Sel = objects as array)
			if replaceradio.state == 2 do (Sel = selection as array)
			numobjects = Sel.count
			undo on
				(
					if ReplaceObject.children.count != 0 then
						(
							-- we have a group
							for i = 1 to numobjects where Sel[i] != ReplaceObject do
							(
								Status1.text  = ("Replacing object " + i as string + " of " + numobjects as string)
								OldObject = Sel[i]
								if isdeleted replaceobject do (print "boo"; exit)
								NewObject = copy ReplaceObject
								for i = 1 to ReplaceObject.children.count do
									(
										append NewObject.children (copy ReplaceObject.children[i])
										NewObject.Children[i].wirecolor = ReplaceObject.children[i].wirecolor
									)
								if MigrateRotation.checked == true do (NewObject.rotation = OldObject.rotation)
								if MigrateScale.checked == true do (NewObject.scale = OldObject.scale)
								NewObject.pos = OldObject.Pos
								if KeepWire.checked == true do (NewObject.wirecolor = color ReplWireColor.x ReplWireColor.y ReplWireColor.z)
								if DeleteOld.checked == true do (delete OldObject)
							)
						) else (
								for i = 1 to numobjects where Sel[i] != ReplaceObject do
								(
									Status1.text  = ("Replacing object " + i as string + " of " + numobjects as string)
									OldObject = Sel[i]
									if isdeleted replaceobject do (print "boo"; exit)
									if CopyType.state == 1 do (NewObject = Copy ReplaceObject)
									if CopyType.state == 2 do (NewObject = Instance ReplaceObject)
									if MigrateRotation.checked == true do (NewObject.rotation = OldObject.rotation)
									if MigrateScale.checked == true do (NewObject.scale = OldObject.scale)
									NewObject.pos = OldObject.Pos
									if KeepWire.checked == true do (NewObject.wirecolor = color ReplWireColor.x ReplWireColor.y ReplWireColor.z)
									if DeleteOld.checked == true do (delete OldObject)
								)
						)
				) -- undo ()
		) -- ReplaceSelected()
) -- Utility End