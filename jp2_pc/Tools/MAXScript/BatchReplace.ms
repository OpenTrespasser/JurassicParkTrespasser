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

fn GetInstances CObj =
	(
		local SelArray = #()
		CKeyName = KeyName CObj.name
		for o in objects where o != CObj do
			if (Keyname o.name) == CKeyName do
				append SelArray o
		return SelArray
	)

fn PropagateSelection = 
	(
		KeyNameArray = #()
		for obj in selection do
			if findItem KeyNameArray (KeyName obj.name) == 0 do
				append KeyNameArray (KeyName obj.name)
		oc = 1 / objects.count as float
		cnt = 0
		ProgressStart "Generating Selection...."
		sel = #()
		for o in objects where o.isselected == false do
			(
				cnt += 1
				progressUpdate ((cnt * oc) * 100)
				if findItem KeyNameArray (KeyName o.name) != 0 do
					(
						if o.isHidden do unhide o
						append Sel o
					)
			)
		selectmore sel
		ProgressEnd()
	)

fn getObjNames foo a = (for i = 1 to a.count collect a[i].name)

fn RemArrayItem Index a = 
	(
		b = #()
		for i = 1 to a.count do
			if i != index do
				append b a[i]
		return b
	)

fn findInGroup o g =
	(
		local found = false
		for i = 1 to g.count do
			if (findItem g[i] o) != 0 do
				(
					found = true
					Exit
				)
		return found
	)

Utility BatchReplaceObj "Batch Replace"
(
	local ReplaceObject, Sel, ReplWireColor,
				SourceObjectArray = #(),
				GroupArray = #(),
				GroupData = #()

	local debug = false

	group "Batch Replace"
		(
			listBox SourceObjectList "Source Object List:" height:5
			PickButton AddSourceObject "Add" width:70 offset:[-37,0]
			button RemoveSourceObject "Remove" width:70 offset:[37,-26]
			ListBox ReplaceGroupList "Replace Group List:" height:10
			button AddGroup "Add Sel" width:70 offset:[-37,0]
			button AddInstances "Add Inst." width:70 offset:[37,-26]
			button ClearGroup "Clear" width:70 offset:[-37,0]
			button RemoveGroupItem "Remove" width:70 offset:[37,-26]
		)

	checkbox AcquireProps "Aquire Replacee props"
	button BatchReplace "Batch Replace" width:145
	button Propagate "Propagate to Instances" width:145
	label status1

	on Propagate pressed do PropagateSelection()
	on BatchReplaceSelection pressed do
		(
			true
		)

	on AddSourceObject picked obj do
		(
			CSource = obj
			if (finditem SourceObjectArray CSource) == 0 and (findInGroup CSource GroupArray) == false then
				(
					append SourceObjectArray CSource
					append GroupArray #()
					ObjectList = SourceObjectList.items
					append ObjectList (((ObjectList.count + 1) as string) + ": " + CSource.name)
					SourceObjectList.items = ObjectList
					SourceObjectList.selection = ObjectList.count
					ReplaceGroupList.items = #()
				) else (
					messagebox "this object is already used in this tool."
				)
		)

	on RemoveSourceObject pressed do
		(
					s = SourceObjectList.selection
			SourceObjectArray = (RemArrayItem s SourceObjectArray)
			SourceObjectList.items = RemArrayItem s SourceObjectList.items
			GroupArray[s] = #()
			ReplaceGroupList.items = #()
		)

	on AddGroup pressed do
		(
			-- what source object index is selected?
			s = SourceObjectList.selection
			CGroup = GroupArray[s]
			ObjArray = selection as array
			oc = ObjArray.count
			for i =1 to oc do
				if (finditem CGroup ObjArray[i]) == 0 do
					append CGroup ObjArray[i]
			GroupArray[s] = CGroup
			ReplaceGroupList.items = (getObjNames true CGroup)
		)

	on AddInstances pressed do
		(
			-- what source object index is selected?
			s = SourceObjectList.selection
			CGroup = GroupArray[s]
			ObjArray = GetInstances SourceObjectArray[s]
			oc = ObjArray.count
			for i =1 to oc do
				if (finditem CGroup ObjArray[i]) == 0 do
					append CGroup ObjArray[i]
			GroupArray[s] = CGroup
			ReplaceGroupList.items = (getObjNames true CGroup)
		)

	on RemoveGroupItem pressed do
		(
			s = SourceObjectList.selection
			GroupArray[s] = (RemArrayItem ReplaceGroupList.selection GroupArray[s])
			ReplaceGroupList.items = RemArrayItem ReplaceGroupList.selection ReplaceGroupList.items
		)

	on ClearGroup pressed do
		(
			GroupArray[SourceObjectList.selection] = #()
			ReplaceGroupList.items = #()
		)

	on SourceObjectList selected s do
		ReplaceGroupList.items = (getObjNames true GroupArray[s])

	on BatchReplace pressed do
		(
			GroupData = #()
			-- Collect all the position rotation and scale data from the objets we need to replace.
			for i = 1 to GroupArray.count do
				(
					append GroupData #()
					CGroup = GroupArray[i]
					local CData
					if debug do CHeap = heapFree
					for j = 1 to CGroup.count do
						(
							CData = #(CGroup[j].pos, CGroup[j].rotation, CGroup[j].scale, CGroup[j].wirecolor,(getUserPropBuffer CGroup[i]))
							append GroupData[i] CData
						)
					if debug do format "Heap Used:%\n" ((CHeap - HeapFree) as float / 1024)
				)
			MAX select none
			status1.text = ("Selecting...")
			for i = 1 to GroupArray.count do selectmore GroupArray[i]
			status1.text = ("Deleting...")
			MAX delete
			sc = SourceObjectArray.count
			status1.text = ("Replacing...")
			for i = 1 to sc do
				(
					status1.text = ("Replacing Source Object " + i as string + " of " + sc as string)
					CSource = SourceObjectArray[i]
					ProgressStart "Replacing...."
					if debug do
						format "GroupData: %\n" GroupData
					gcount = GroupData[i].count
					gcInv = (1.0 / gcount) * 100
					for j = 1 to gcount do
						(
							ProgressUpdate (j * gcInv)
							CObj = instance CSource
							CObj.rotation = GroupData[i][j][2]
							CObj.scale = GroupData[i][j][3]
							CObj.pos = GroupData[i][j][1]
							CObj.wirecolor = GroupData[i][j][4]
							if AcquireProps.checked do setUserPropBuffer CObj GroupData[i][j][5]
						)
					ProgressEnd()
				)
			status1.text = ""
			SourceObjectList.items = #()
			ReplaceGroupList.items = #()
			SourceObjectArray = #()
			GroupArray = #()
			GroupData = #()
			
		)
) -- Utility End