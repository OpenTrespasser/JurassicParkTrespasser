fn HardReset obj = 
(
gc ()

local CObj = obj
ConvertToMesh CObj
CObj.pivot = CObj.center
Props = GetUserPropBuffer CObj

-- Copy the object.  Center the pivot.
local o_pos = obj.pos
local o_rot = obj.rotation
local o_scale = obj.scale

-- move the object to the origin, strip off rotation, scale
obj.pos = [0,0,0]
obj.rotation = (quat 0 z_axis)
obj.scale = [1,1,1]

-------

local VertArray = for i = 1 to CObj.numverts collect (getvert CObj i)
local FaceArray = for i = 1 to CObj.numfaces collect (getface CObj i)
local MatIDArray = for i = 1 to CObj.numfaces collect (GetFaceMatID CObj i)
local TVertArray = #()

local i = 0
for i = 1 to FaceArray.count do
	(
		CFace = (GetTVFace CObj i)
		if FindItem TVertArray CFace.x == 0 do (append TVertArray CFace.x)
		if FindItem TVertArray CFace.y == 0 do (append TVertArray CFace.y)
		if FindItem TVertArray CFace.z == 0 do (append TVertArray CFace.z)
	)
TVertUVArray = (for i = 1 to TVertArray.count collect (GetTvert CObj TVertArray[i]))
local m = mesh vertices:VertArray faces:FaceArray MaterialIDs:MatIDArray
for i = 1 to CObj.numfaces do
	(
		SetEdgeVis m i 1 (GetEdgeVis CObj i 1)
		SetEdgeVis m i 2 (GetEdgeVis CObj i 2)
		SetEdgeVis m i 3 (GetEdgeVis CObj i 3)
	)
for i = 1 to CObj.numfaces do
	SetFaceSmoothGroup m i (getFaceSmoothGroup CObj i)
addmodifier m (uvwMap maptype:1)
ConvertToMesh m

local Ntv = getNumTVerts CObj
SetNumTverts m Ntv true

for i = 1 to CObj.numfaces do
	setTVFace m i (GetTVFace CObj i)

update m
for i = 1 to Ntv do
	SetTvert m i (GetTvert CObj i)
m.name = (CObj.name + "_regen")
m.pivot = m.center

-- fix location of new object

m.scale = o_scale
m.rotation = o_rot
m.pos = o_pos

-- other stuff

m.wirecolor = CObj.wirecolor


update m
if CObj.mat != undefined do
	m.mat = CObj.mat

local oname = CObj.name
delete CObj
m.name = oname
setUserPropBuffer m Props
return m
)

fn roundTo val n = 
	(
		local mult = 10.0 ^ n
		(floor ((val * mult) + 0.5)) / mult
	)

fn MinMax foo a = 
	(
		mn = a[1]
		mx = a[1]
		for i = 2 to a.count do
			(
				if a[i] > mx do mx = a[i]
				if a[i] < mn do mn = a[i]
			)
		return #(mn, mx)
	)

fn Min foo a = 
	(
		local mn = a[1]
		for i = 2 to a.count do
			if a[i] < mn do mn = a[i]
		return mn
	)

fn ClampW CObj = 
	(
		for j = 1 to CObj.numfaces do
			(
				CFace = (GetTVFace cObj j)
				c1 = getTvert CObj CFace.x
				c2 = getTvert CObj CFace.y
				c3 = getTvert CObj CFace.z
				if c1.z >= 0.0 and c1.z <= 0.0001 do SetTVert CObj CFace.x [c1.x, c1.y, 1]
				if c2.z >= 0.0 and c2.z <= 0.0001 do SetTVert CObj CFace.y [c2.x, c2.y, 1]
				if c3.z >= 0.0 and c3.z <= 0.0001 do SetTVert CObj CFace.z [c3.x, c3.y, 1]
			)
		update CObj
	)


fn reset_transform obj =
(
--	local xf = xform ()
--	local pos = obj.pos
--	xf.gizmo.transform = matrix3 1
--	addModifier obj xf
--	obj.pos = pos

------------------old reset code-------------
	-- grab current transform, reset it, add a new xform
	-- modifier & set the xform's tm to the object's original tm
	local ftm = obj.transform
	obj.transform = matrix3 1
	local xf = xform ()
	addModifier obj xf
	xf.gizmo.transform = ftm
)


Utility MeshTools "Mesh Tools"
	(
		
		group "UV Editing Tools"
			(
				button ClampSelected "Clamp Selected" width:145
				spinner ClampThresh "UV Coord Thresh: " range:[0, 1, 0.1] fieldwidth:35
				checkbox clampFaceSelection "Clamp Face Selection"
				button NormalizeTVerts "Normalize TVerts" width:145
				button NumTiles "Num Tiles" width:145
			)
		
		group "Material ID's"
			(
				pickbutton PickObjectMatID "Select out of range ID's" width:145
				button set_face_ids_btn "Set Selected Mat ID's" width: 145
			)
		
		group "Coplanar Test"
			(
				pickbutton ChooseCoplanar "Choose Object" width:145
			)
		
		group "Pivots/Transforms"
			(
				button CenterPivot "Center Pivots Only" width:145
				button CenterSelection "Cntr Pivots w/Reset Xform" width:145 enabled:false
				button ResetSelection "Reset Transform Only" width:145 enabled:false
				button HardResetSelection "Hard Reset-Save First!" width:145 enabled:true
			)
		
		group "General tools"
			(
				button Collapse_Selected_btn "Collapse Selected" width:145
				button selectNonObjects "Select Non-Objects" width:145
				pickbutton getdegeneratefaces "Select Degenerate Faces" width:145 offset:[-4,0]
			)
		
		group "Vertex Selection"
			(
				PickButton SelVertsByThresh "Select by Threshold" width:145
				spinner VertSelectionThresh "Threshold (M):" range:[0,1,0.01] type:#float fieldwidth:35
			)
		
		group "SubObject Selection"
			(
				spinner Index "Index:" type:#integer range:[1,4096,1] fieldWidth:45
				radioButtons SubObjectType "Type:" labels:#("Faces", "Verts") columns:1 align:#left offset:[0,-20]
				button Select "Select" width:140
			)

		group "Mesh Regenerator"
			(
				colorpicker NewWireColor "New WireColor:" color:(color 255 255 0) offset:[20,0]
				Label newPosLabel "New Position:" align:#left
				spinner PosX "X:" range:[-9999,9999,0] type:#float fieldwidth:45 align:#left
				spinner PosY "Y:" range:[-9999,9999,0] type:#float fieldwidth:45 align:#left
				spinner PosZ "Z:" range:[-9999,9999,0] type:#float fieldwidth:45 align:#left
				checkbox deleteOld "Delete old object"
				pickbutton ChooseObject "Choose Object" width:140
			)
		
		group "Migrate Selection"
			(
				checkbox ClearSel "Keep initial selection"
				radiobuttons fromSel "From:" labels:#("Vertices", "Faces") columns:1 offset:[-40,0] default:1
				radiobuttons toSel "To:" labels:#("Vertices", "Faces") columns:1 offset:[40,-50] default:2
				label methodlabel "Method:" align:#left
				CheckButton MethodWindow "Window" offset:[-37,0] width:70
				CheckButton MethodCrossing "Crossing" offset:[37,-26] width:70 checked:true
				pickButton PickObject "Pick Object" width:140
			)
		
		group "Object Matrix Info"
			(
				button DoScene "Get matrix info for selected" width:145
			)

		on Select Pressed do
			(
				sel = selection as array
				oc = sel.count 
				for i = 1 to oc do
					(
						if SubObjectType.state == 1 do
							setFaceSelection sel[i] #(index.value)

						if SubObjectType.state == 2 do
							setVertSelection sel[i] #(index.value)
					)
			)		

		on NumTiles pressed do
			(
				ObjArray = for o in selection collect o
				oc = ObjArray.count
				for i = 1 to oc do
					(
						CObj = ObjArray[i]
						format "\nObject:%\n" CObj.name
						ClampW CObj
						if CObj.mat != undefined do
							(
								if classOf CObj.mat == standardMaterial do
									(
										av3TV = #()
										aiTVi = #()
										for j = 1 to CObj.numfaces do
											(
												CFace = (GetTVFace cObj j)
												if FindItem aiTVi CFace.x == 0 do
													(
														append aiTVi CFace.x
														append av3TV (getTVert CObj CFace.x)
													)
				
												if FindItem aiTVi CFace.y == 0 do
													(
														append aiTVi CFace.y
														append av3TV (getTVert CObj CFace.y)
													)
				
												if FindItem aiTVi CFace.z == 0 do
													(
														append aiTVi CFace.z
														append av3TV (getTVert CObj CFace.z)
													)
											)
										afTVu = for j = 1 to av3TV.count collect av3TV[j].x
										afTVv = for j = 1 to av3TV.count collect av3TV[j].y
										mmu = (MinMax true afTVu)
										mmv = (MinMax true afTVv)
										
										MinU = mmu[1]
										MaxU = mmu[2]
				
										MinV = mmv[1]
										MaxV = mmv[2]
				
										format "\tNumTilesU: %\n" (MaxU - MinU)
										format "\tNumTilesV: %\n\n" (MaxV - MinV)
										-- do it based on sets of material IDs
									)
								if ClassOf CObj.mat == MultiMaterial do
									(
										for k = 1 to CObj.mat.numsubs do
											(
												av3TV = #()
												aiTVi = #()
												ValidFace = false
												for j = 1 to CObj.numfaces where (getFaceMatID CObj j) == k do
													(
														ValidFace = true
														CFace = (GetTVFace cObj j)
														if FindItem aiTVi CFace.x == 0 do
															(
																append aiTVi CFace.x
																append av3TV (getTVert CObj CFace.x)
															)
						
														if FindItem aiTVi CFace.y == 0 do
															(
																append aiTVi CFace.y
																append av3TV (getTVert CObj CFace.y)
															)
						
														if FindItem aiTVi CFace.z == 0 do
															(
																append aiTVi CFace.z
																append av3TV (getTVert CObj CFace.z)
															)
													)
												if ValidFace == true do
													(
														afTVu = for j = 1 to av3TV.count collect av3TV[j].x
														afTVv = for j = 1 to av3TV.count collect av3TV[j].y
														mmu = (MinMax true afTVu)
														mmv = (MinMax true afTVv)
														MinU = mmu[1]
														MaxU = mmu[2]
														MinV = mmv[1]
														MaxV = mmv[2]
														format "\tMaterial ID:%\n" k
														format "\tNumTilesU: %\n" (MaxU - MinU)
														format "\tNumTilesV: %\n\n" (MaxV - MinV)
													)
												-- do it based on sets of material IDs
											)
									)
							)
					)
			)

		
		on NormalizeTVerts pressed do
			(
				ObjArray = for o in selection collect o
				oc = ObjArray.count
				for i = 1 to oc do
					(
						CObj = ObjArray[i]
						ClampW CObj
						av3TV = #()
						aiTVi = #()
						for j = 1 to CObj.numfaces do
							(
								CFace = (GetTVFace cObj j)
								if FindItem aiTVi CFace.x == 0 do
									(
										append aiTVi CFace.x
										append av3TV (getTVert CObj CFace.x)
									)

								if FindItem aiTVi CFace.y == 0 do
									(
										append aiTVi CFace.y
										append av3TV (getTVert CObj CFace.y)
									)

								if FindItem aiTVi CFace.z == 0 do
									(
										append aiTVi CFace.z
										append av3TV (getTVert CObj CFace.z)
									)
							)
						afTVu = for j = 1 to av3TV.count collect av3TV[j].x
						afTVv = for j = 1 to av3TV.count collect av3TV[j].y
						MinU = (Min true afTVu)
						MinV = (Min true afTVv)

						if MinU < 0.0 then
							MinU = (MinU as integer) - 1
						else
							MinU = (MinU as integer)

						if MinV < 0.0 then
							MinV = (MinV as integer) - 1
						else
							MinV = (MinV as integer)
						
						for j = 1 to av3TV.count do
							(
								av3TV[j].x = av3TV[j].x - MinU
								av3TV[j].y = av3TV[j].y - MinV
							)
						for j = 1 to av3TV.count do
							setTVert CObj aiTVi[j] av3TV[j]
					)
			)

		on SelVertsByThresh picked obj do
			(
				if classOf obj == editable_mesh then
					(
						CObj = obj
						ConvertToMesh CObj
						nv = CObj.numverts
						VertSelection = #()
						for i = 1 to nv do
							(
								SourceVert = (getvert CObj i)
								for j = 1 to nv do
									(
										if i != j do
											(
												CompareVert = (getVert CObj j)
												if (distance SourceVert CompareVert) <= VertSelectionThresh.value do
													if (findItem VertSelection j) == 0 do (append VertSelection j)
											)
									)
							)
						setVertSelection CObj VertSelection
						update CObj
					) else (
						MessageBox "Object needs to be an EditableMesh."
					)
			)
		
		
		 on CenterSelection pressed do
		 	(
				ObjArray = for obj in selection collect obj
				for i = 1 to ObjArray.count do
					(
						CObj = ObjArray[i]
						ConvertToMesh CObj
						CObj.pivot = CObj.center
						CObjPos = CObj.pos
						CObj.pos = [0,0,0]
						reset_transform CObj
						CObj.pivot = CObj.center
		 				ConvertToMesh CObj
						CObj.pos = CObjPos
					)
			)
		
		 on HardResetSelection pressed do
		 	(
				ObjArray = for obj in selection collect obj
				ProgressStart "Resetting..."
				Progscale = 100.0 / ObjArray.count
				for i = 1 to ObjArray.count do
					(
						if classof ObjArray[i] != dummy do
							(
								m = (HardReset ObjArray[i])
								if (progressupdate (i * progscale)) == false then exit
							)
					)
				ProgressEnd()
			)
		
		 on ResetSelection pressed do
		 	(
				ObjArray = for obj in selection collect obj
				for i = 1 to ObjArray.count do
					(
						CObj = ObjArray[i]
						ConvertToMesh CObj
						CObjPos = CObj.pos
						CObj.pos = [0,0,0]
						reset_transform CObj
		 				ConvertToMesh CObj
						CObj.pos = CObjPos
					)
			)
		
		on set_face_ids_btn pressed do
			(
				ObjSelection = for obj in selection collect obj
				ObjCount = ObjSelection.count
				for i = 1 to ObjCount do
					(
						CObj = ObjSelection[i]
						if (CObj = (ConvertToMesh CObj)) != undefined do
							if classof CObj.mat == standardMaterial do
								(
									nf = CObj.numfaces
									for i = 1 to nf do (setfacematid CObj i 1)
								)
					)
			)
		
		on selectNonObjects pressed do
		(
			max select none
			for o in objects do
				if o.numfaces == 0 do (selectmore o)
		)
		
		on getdegeneratefaces picked obj do
			(
				o = obj
				ConvertToMesh o
				setfaceselection o #()
				BadFaces = False
				for i = 1 to o.numfaces do
					(
						currentface = getface o i
						NewX = getvert o currentface.x
						NewY = getvert o currentface.y
						NewZ = getvert o currentface.z
						if NewX == NewY do 
							(
								setfaceselection o #(i) keep:true
								BadFaces = true
							)
						if NewX == NewZ do
							(
								setfaceselection o #(i) keep:true
								BadFaces = true
							)
						if NewZ == NewY do
							(
								setfaceselection o #(i) keep:true
								BadFaces = true
							)
					)
				if BadFaces == false do
					MessageBox "No degenerate faces found!"
				if badfaces == true do
					MessageBox "Degenerate faces were found!\n Degenerate faces are current sub-object selection"
			)
		
		
		on CenterPivot pressed do
			(
				objs = selection as array
				NumObjs = objs.count
				for i = 1 to NumObjs do
					(
						o = Objs[i]
						o.pivot = o.center
					)
			)
		
		on Collapse_Selected_btn pressed do
			(
				undo on 
					(
						ObjSelection = selection as array
						ConvertToMesh ObjSelection
						for obj in ObjSelection do selectmore obj
					)
			)
		
		on pickObjectMatID picked obj do
			(
				CObj = obj
				if classof CObj.mat == MultiMaterial then
					(
						convertToMesh CObj
						setfaceselection CObj #()
						ns = CObj.mat.numsubs
						nf = CObj.numfaces
						FaceSel = #()
						OutOfRange = false
						for i = 1 to nf do
							(
								Cid = (getfacematID cObj i)
								if Cid > ns do
									(
										append FaceSel i
										OutOfRange = true
									)
							)
						if OutOfRange == true then
							(
								setFaceSelection CObj FaceSel
								update CObj
								MessageBox "Out of range faces set to\ncurrent SubObject face selection"
							) else (
								MessageBox "No out of range faces detected"
							)
					) else (
						MessageBox "Picked Object does not\nhave a Multi Material assigned to it"
					)
			)
		
			on ChooseCoplanar picked obj do
				(
					if classof obj == editable_mesh then
						(
							CObj = obj
							ConvertToMesh CObj
							NotCoplanar = false
							FaceArray = getFaceSelection CObj
							CNormal = GetFaceNormal CObj Facearray[1]
							CNormal = [(RoundTo CNormal.x 3),(RoundTo CNormal.y 3),(RoundTo CNormal.z 3)]
							format "Checking against: %\n" CNormal
							for i = 1 to FaceArray.count do
								(
									NewNormal = GetFaceNormal CObj FaceArray[i]
									NewNormal = [(RoundTo NewNormal.x 3),(RoundTo NewNormal.y 3),(RoundTo NewNormal.z 3)]
									if NewNormal != CNormal do NotCoplanar = true
									format "NewNormal: %\n" NewNormal
								)
							if NotCoplanar == true then
								MessageBox "The selected faces are NOT coplanar."
							else
								MessageBox "The selected faces are coplanar!"
						) else (
							Messagebox "This tool can only use an EditableMesh object.\n Please collapse your mesh."
						)
				)
		
		on ClampSelected pressed do
			(
				ObjArray = for obj in selection collect obj
				for o in ObjArray do
					(
						if ClampFaceSelection.checked then
							FaceArray = getFaceSelection o
						else
							FaceArray = for i = 1 to o.numfaces collect i
		
						-- if there's nothing in the face array, it means they wanted to clamp selection, but there are no selected faces, so clamp all of them
						if FaceArray.count == 0 do
							FaceArray = for i = 1 to o.numfaces collect i
							
						nf = FaceArray.count
						TFaces = for i = 1 to nf collect (gettvface o FaceArray[i])
						Tverts = #()
						for i = 1 to nf do
							(
								if finditem Tverts Tfaces[i].x == 0 do (append Tverts Tfaces[i].x)
								if finditem Tverts Tfaces[i].y == 0 do (append Tverts Tfaces[i].y)
								if finditem Tverts Tfaces[i].z == 0 do (append Tverts Tfaces[i].z)
							)
						for i = 1 to Tverts.count do
							(
								CTvert = gettvert o Tverts[i]
								if CTvert.x > 1.0 and CTvert.x <= (1.0 + ClampThresh.value) do setTvert o Tverts[i] [1.0,CTVert.y, CTVert.z]
								if CTvert.x < 0.0 and CTvert.x >= (0.0 - ClampThresh.value) do setTvert o Tverts[i] [0.0,CTVert.y, CTVert.z]
								if CTvert.y > 1.0 and CTvert.y <= (1.0 + ClampThresh.value) do setTvert o Tverts[i] [CTvert.x, 1.0, CTVert.z]
								if CTvert.y < 0.0 and CTvert.y >= (0.0 - ClampThresh.value) do setTvert o Tverts[i] [CTvert.x, 0.0, CTVert.z]
							)
						update o
					)
			)
		
			on ChooseObject picked Obj do
				(
					if classof obj == editable_mesh then
						(
							CObj = obj
							VertArray = for i = 1 to CObj.numverts collect (getvert CObj i)
							FaceArray = for i = 1 to CObj.numfaces collect (getface CObj i)
							MatIDArray = for i = 1 to CObj.numfaces collect (GetFaceMatID CObj i)
							TVertArray = #()
							for i = 1 to FaceArray.count do
								(
									CFace = (GetTVFace CObj i)
									if FindItem TVertArray CFace.x == 0 do (append TVertArray CFace.x)
									if FindItem TVertArray CFace.y == 0 do (append TVertArray CFace.y)
									if FindItem TVertArray CFace.z == 0 do (append TVertArray CFace.z)
								)
							TVertUVArray = (for i = 1 to TVertArray.count collect (GetTvert CObj TVertArray[i]))
							m = mesh vertices:VertArray faces:FaceArray MaterialIDs:MatIDArray
							for i = 1 to CObj.numfaces do
								(
									SetEdgeVis m i 1 (GetEdgeVis CObj i 1)
									SetEdgeVis m i 2 (GetEdgeVis CObj i 2)
									SetEdgeVis m i 3 (GetEdgeVis CObj i 3)
								)
							for i = 1 to CObj.numfaces do
								SetFaceSmoothGroup m i (getFaceSmoothGroup CObj i)
							addmodifier m (uvwMap maptype:1)
							ConvertToMesh m
		
							Ntv = getNumTVerts CObj
							SetNumTverts m Ntv true
		
							for i = 1 to CObj.numfaces do
								setTVFace m i (GetTVFace CObj i)
		
							update m
							for i = 1 to Ntv do
								SetTvert m i (GetTvert CObj i)
							m.name = (CObj.name + "_regen")
							m.pivot = m.center
							m.pos = [PosX.value,PosY.value,PosZ.value]
							m.wirecolor = NewWireColor.color
							update m
							if CObj.mat != undefined do
								m.mat = CObj.mat
							if deleteOld.checked do delete CObj
						) else (
							MessageBox "Needs and Editable Mesh object\n please collapse your mesh"
						)
				)
		
		
		on MethodWindow changed state do
			(
				if MethodWindow.state == on then 
					MethodCrossing.state = off
				else
					MethodCrossing.state = on
			)
		
		on MethodCrossing changed state do
			(
				if MethodCrossing.state == on then 
					MethodWindow.state = off
				else 
					MethodWindow.state = on
			)
		
		
		on fromSel changed state do 
			(
				if fromSel.state == 1 do 
					(
						toSel.state = 2
						MethodWindow.enabled = true
						MethodCrossing.enabled = true
						MethodCrossing.checked = true
					)
			
				if fromSel.state == 2 do 
					(
						toSel.state = 1
						MethodWindow.checked = false
						MethodWindow.enabled = false
						MethodCrossing.checked = false
						MethodCrossing.enabled = false
					)
			)
		
		on ToSel changed state do 
			(
				if ToSel.state == 1 do 
					(
						FromSel.state = 2
						MethodWindow.checked = false
						MethodWindow.enabled = false
						MethodCrossing.checked = false
						MethodCrossing.enabled = false
					)
				
				if ToSel.state == 2 do 
					(
						FromSel.state = 1
						MethodWindow.enabled = true
						MethodCrossing.enabled = true
						MethodCrossing.checked = true
					)
			)
		
		
		On PickObject picked obj do
		(
			o = obj
			undo on (
			
			if FromSel.state == 1 do
				(
					if MethodCrossing.checked == true then 
						(
							VertSel = getvertselection o
							AllFaces = (for i = 1 to o.numfaces collect (getface o i))
							VSc = VertSel.count
							AFc = AllFaces.count
							if ClearSel.checked == false do (setfaceselection o #())
							for i = 1 to VSc do
								(
									for j = 1 to AFc do
										(
											if VertSel[i] == (Allfaces[j]).x do (setfaceselection o #(j) keep:true)
											if VertSel[i] == (Allfaces[j]).y do (setfaceselection o #(j) keep:true)
											if VertSel[i] == (Allfaces[j]).z do (setfaceselection o #(j) keep:true)
										)
								)
							update o
						) 
							else 
						(
							VertSel = getvertselection o
							AllFaces = (for i = 1 to o.numfaces collect (getface o i))
							AFc = AllFaces.count
							if ClearSel.checked == false do (setfaceselection o #())
							for j = 1 to AFc do
								(
									Cnt = 0
									if (findItem VertSel (AllFaces[j]).x) != 0 do (Cnt = Cnt + 1)
									if (findItem VertSel (AllFaces[j]).y) != 0 do (Cnt = Cnt + 1)
									if (findItem VertSel (AllFaces[j]).z) != 0 do 
										(
											Cnt = Cnt + 1
											if Cnt == 3 do (setfaceselection o #(j) keep:true)
										)
								)
							update o
						)
				)
			
			if FromSel.state == 2 do
				(
					faceSel = getfaceselection o
					NewVertSel = (for i = 1 to faceSel.count collect (getface o faceSel[i]))
					NVSc = NewVertSel.count
					if ClearSel.checked == false do (setvertselection o #())
					for i = 1 to NVSc do 
						(
							setvertselection o #((NewVertSel[i]).x) keep:true
							setvertselection o #((NewVertSel[i]).y) keep:true
							setvertselection o #((NewVertSel[i]).z) keep:true
						)
					update o
				)
			)
		)
		
		
		on DoScene pressed do
			(
				save_filename = getsavefilename caption:"Specify Scene Matrix Dump File"
				f = createfile save_filename
				for o in selection do
					(
						format "\n"
						format "****************************************************************************\n" to:f
						format "Object Data for %\n" o.name to:f
						format "****************************************************************************\n" to:f
						format "Object Pivot:\t\t\t\t%\n" o.pivot to:f
						format "Object Geometrical Center:\t\t%\n" o.center to:f
						format "Pivot/Center Offset:\t\t\t%\n" (o.pivot - o.center) to:f
						format "Object Position:\t\t\t\t%\n" o.pos to:f
						format "Object Extents:\t\t\t\t%\n" (o.max - o.min) to:f
						format "\n" to:f
						format "Main Node Transformation Matrix:\n" to:f
						format "\t\t\t\t\t%\n" o.transform.row1 to:f
						format "\t\t\t\t\t%\n" o.transform.row2 to:f
						format "\t\t\t\t\t%\n" o.transform.row3 to:f
						format "\t\t\t\t\t%\n" o.transform.row4 to:f
						format "\n" to:f
						format "Translation:\t\t\t\t%\n" o.transform.translation to:f
						format "Rotation Part:\t\t\t\t%\n" o.transform.rotationpart to:f
						format "Translation Part:\t\t\t%\n" o.transform.translationpart to:f
						format "Scale\Rotation Part:\t\t\t%\n" o.transform.scalerotationpart to:f
						format "Scale Part:\t\t\t\t%\n" o.transform.scalepart to:f
						format "Determinant Sign:\t\t\t%\n" o.transform.determinantsign to:f
						format "Z Axis direction Vector:\t\t\t%\n" o.dir to:f
						format "Object Offset Position:\t\t\t%\n" o.objectoffsetpos to:f
						format "Object Offset Rotation:\t\t\t%\n" o.objectoffsetrot to:f
						-- (not yet implemented) format "Object Offset Scale: %\n" o.objectoffsetscale to:f
						format "\n" to:f
						format "Object Transformation Matrix:\n" to:f
						format "\t\t\t\t\t%\n" o.objecttransform.row1 to:f
						format "\t\t\t\t\t%\n" o.objecttransform.row2 to:f
						format "\t\t\t\t\t%\n" o.objecttransform.row3 to:f
						format "\t\t\t\t\t%\n" o.objecttransform.row4 to:f
						format "\n" to:f
						format "Translation:\t\t\t\t%\n" o.objecttransform.translation to:f
						format "Rotation Part:\t\t\t\t%\n" o.objecttransform.rotationpart to:f
						format "Translation Part:\t\t\t%\n" o.objecttransform.translationpart to:f
						format "Scale/Rotation Part:\t\t\t%\n" o.objecttransform.scalerotationpart to:f
						format "Scale Part:\t\t\t\t%\n" o.objecttransform.scalepart to:f
						format "Determinant Sign:\t\t\t%\n" o.objecttransform.determinantsign to:f
					)
				close f
			)
	)