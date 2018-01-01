-- MultiSubManager.ms
-- by Kyle McKisic
--
-- 
--

Utility MultiSubManager "Multi Sub Manager"
(
local CNumsubs = 1, MaterialArray, SubMatNameArray = #(), SubMaterialView, CObj, CFaceArray,
NFaceArray, Cnf, WackyIDs, NMat, CSub, CMat

pickbutton pickObject "Pick Object" width:140
button RebuildSelected "Rebuild Selected" width:140

on RebuildSelected pressed do
	(
		ObjArray = for obj in selection collect obj
		oc = ObjArray.count
		for i = 1 to oc do
			(
				CObj = ObjArray[i]
				if classof CObj.mat == MultiMaterial do
					(
						FaceIDArray = #()
						UniqueFaceIDArray = #()
						ns = CObj.mat.numsubs
						nf = CObj.numfaces
						NMat = MultiMaterial()
						FaceIDArray = for i = 1 to nf collect getFaceMatID CObj i
						for i = 1 to nf do
								if finditem UniqueFaceIDArray FaceIDArray[i] == 0 do (append UniqueFaceIDArray FaceIDArray[i])
						NMat = MultiMaterial()
						NMat.name = CObj.mat.name
						NMat.numsubs = UniqueFaceIDArray.count
						NewSubMatCount = 0
						for i = 1 to ns do
							(
								if findItem FaceIDArray i != 0 do
									(
										NewSubMatCount = NewSubMatCount + 1
										for j = 1 to nf do
											(
												if FaceIDArray[j] == i do (SetFaceMatID CObj j NewSubMatCount) -- reset the face material ID
												NMat[NewSubMatCount] = CObj.mat[i] -- toss the new Sub Material into the new MM
											)
									) 
							)
						if NMat.numsubs == 1 do
							(
								NMat2 = StandardMaterial()
								NMat2.name = NMat.name
								NMat2.diffusemap = bitmaptexture()
								NMat2.diffusemap = NMat[1].diffusemap
								if NMat[1].OpacityMap == bitmaptexture do
									(
										NMat2.Opacitymap = bitmaptexture()
										NMat2.opacitymap = NMat[1].OpacityMap
									)
								if NMat[1].BumpMap == bitmaptexture do
									(
										NMat2.Bumpmap = bitmaptexture()
										NMat2.BumpMap = NMat[1].Bumpmap
									)
								NMat = NMat2
								NMat.name = NMat2.name
							)
						CObj.mat = NMat -- asign the new material to CObj
						if CObj.mat.name == "" do 
							CObj.mat.name = CObj.name
					)
			)
	)

on MultiSubManager open do SubMatNameArray = #()

on MultiSubManager close do removeRollout SubMaterialView

on PickObject Picked obj do
	(
		removeRollout SubMaterialView
		SubMatNameArray = #()
		CFaceArray = #()
		NFaceArray = #()
		if classof obj != editable_mesh then
			(
				if (querybox "Object is not an EditableMesh, do you want to Collapse?\n\n If you click No, a copy of the object will be made") == true then
					(
						ConvertToMesh Obj
						CObj = obj
					) else (
						CObj = copy Obj	
						ConvertToMesh CObj
						CObj.name = (Obj.name + "-Copy")
					)
			) else (
				CObj = obj
			)
		Cnf = CObj.numfaces
		NMat = undefined
-- build FaceID Array
		for i = 1 to Cnf do (CFaceArray[i] = (getfaceMatID CObj i))
		if classof CObj.mat == MultiMaterial then
			(
				CNumSubs = CObj.mat.numsubs
				for i = 1 to CNumSubs do (append SubMatNameArray (i as string + ": " +(CObj.mat[i]).name))
				addrollout SubMaterialView rolledup:false
			) else (
				messagebox "Picked Object does not have\n a MultiMaterial assigned to it!"
			)
	)

rollout SubMaterialView "Sub Materials"
(
	listbox SubMatList "Sub Materials:" height:10 items:SubMatNameArray
	button RemoveSubMaterial "Remove" width:145
	button MoveUp "Bump Up" width:70 offset:[-37,0] enabled:false
	button MoveDown "Bump Down" width:70 offset:[37,-26] enabled:false
	button RebuildMat "Rebuild Material" width:145
	label status1
	
on RemoveSubMaterial pressed do
	(
		CSub = SubMatList.selection
		CMat = CObj.mat

-- build the new Multi Material
		NMat = MultiMaterial()
		NMat.name = CMat.name
		NMat.numsubs = (CNumSubs - 1)
		NMat.name = CMat.name
		for i = 1 to NMat.numsubs do
			(
				if i >= CSub do (NMat[i] = CMat[i+1])
				if i < CSub do (NMat[i] = CMat[i])
			)

-- Modify CObj's face Mat ID's
		WackyIDs = false
		for i = 1 to CFaceArray.count do
			(
				Cid = CFaceArray[i]
				if Cid == CSub do 
					(
						CFaceArray[i] = 1000
						WackyIDs = true
					)
				if Cid > CSub do (CFaceArray[i] = (Cid - 1))
			)
-- apply the FaceMatID Array back to the object
		for i = 1 to CObj.numfaces do
			(
				setfaceMatid CObj i CFaceArray[i]
			)
-- Apply the new material to CObj
		CObj.mat = NMat
		
-- Update all the interface stuff
		removeRollout SubMaterialView
		CNumSubs = CObj.mat.numsubs
		SubMatNameArray = #()
		for i = 1 to CNumSubs do 
			(
				append SubMatNameArray (i as string + ": " +(CObj.mat[i]).name)
			)
		AddRollout SubMaterialView
		if WackyIDs == true do	(MessageBox "Faces found in object that have\nthe Material ID of the deleted Sub Material.\nSuch faces now have the\nMaterial ID of 1000")
	)
	
on RebuildMat pressed do
	(
	FaceIDArray = #()
	UniqueFaceIDArray = #()
	ns = CObj.mat.numsubs
	nf = CObj.numfaces
	NMat = MultiMaterial()
	FaceIDArray = for i = 1 to nf collect getFaceMatID CObj i
	for i = 1 to nf do
		(
			if finditem UniqueFaceIDArray FaceIDArray[i] == 0 do (append UniqueFaceIDArray FaceIDArray[i])
		)
	NMat = MultiMaterial()
	NMat.name = CObj.mat.name
	NMat.numsubs = UniqueFaceIDArray.count
	NewSubMatCount = 0
	for i = 1 to ns do
		(
			status1.text = i as string
			if findItem FaceIDArray i != 0 then
				(
					NewSubMatCount = NewSubMatCount + 1
					for j = 1 to nf do
						(
							if FaceIDArray[j] == i do (SetFaceMatID CObj j NewSubMatCount) -- reset the face material ID
							NMat[NewSubMatCount] = CObj.mat[i] -- toss the new Sub Material into the new MM
						)
				) else (
				1 == 1
				)
		)
	standard = false
	if NMat.numsubs == 1 do
		(
			NMat2 = StandardMaterial()
			NMat2.name = NMat.name
			NMat2.diffusemap = bitmaptexture()
			NMat2.diffusemap = NMat[1].diffusemap
			if NMat[1].OpacityMap == bitmaptexture do
				(
					NMat2.Opacitymap = bitmaptexture()
					NMat2.opacitymap = NMat[1].OpacityMap
				)
			if NMat[1].BumpMap == bitmaptexture do
				(
					NMat2.Bumpmap = bitmaptexture()
					NMat2.BumpMap = NMat[1].Bumpmap
				)
			NMat = NMat2
			NMat.name = NMat2.name
			standard = true
		)
	CObj.mat = NMat -- asign the new material to CObj
	if CObj.mat.name == "" do 
		CObj.mat.name = CObj.name
-- rebuild the interface
	removeRollout SubMaterialView
	if standard == false do
		(
			CNumSubs = CObj.mat.numsubs
			SubMatNameArray = #()
			for i = 1 to CNumSubs do 
				append SubMatNameArray (i as string + ": " +(CObj.mat[i]).name)
			AddRollout SubMaterialView
		)
	)
) -- end rollout

) -- end Utility
