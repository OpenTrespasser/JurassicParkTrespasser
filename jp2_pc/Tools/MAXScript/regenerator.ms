Utility MeshRegen "Regenerator"
(
	group "New Mesh Options"
	(
		colorpicker NewWireColor "New WireColor:" color:(color 255 255 0) offset:[20,0]
		Label newPosLabel "New Position:" align:#left
		spinner PosX "X:" range:[-9999,9999,0] type:#float fieldwidth:45 align:#left
		spinner PosY "Y:" range:[-9999,9999,0] type:#float fieldwidth:45 align:#left
		spinner PosZ "Z:" range:[-9999,9999,0] type:#float fieldwidth:45 align:#left
		checkbox OriginalPosition "Use it's original position" checked:true
	)
	pickbutton ChooseObject "Choose Object" width:140
	pickButton ExportObject "Export Object" wifth:140

	on ExportObject picked Obj do
		(
			if classof obj == editable_mesh then
				(
					if (SaveFilename = getSaveFilename caption:"Specify Export File:") != undefined do
						(
							-- convert the object to an Editable Mesh Object
							CObj = obj
							ConVertToMesh CObj
							-- setup the file
							createFile SaveFilename
							f = openFile SaveFilename
							-- Object Name
							format "Name:%\n" CObj.name
							-- Object WireColor
							CObjWireColor = CObj.WireColor
							CObjWireColor = [CObjWireColor.r as integer, CObjWireColor.g as integer, CObjWireColor.b as integer]
							format "WireColor:% % %\n" CObjWireColor.y CObjWireColor.x CObjWireColor.z 
							format "Position:% % %\n" CObj.pos.x CObj.pos.y CObj.pos.z 
							
							VertArray = for i = 1 to CObj.numverts collect (getvert CObj i)
							VertexArrayString = ""
							for i = 1 to VertArray.count do
								(	
									VertexArrayString = (VertexArrayString + (VertArray[i].x as string + " "))
									VertexArrayString = (VertexArrayString + (VertArray[i].y as string + " "))
									VertexArrayString = (VertexArrayString + (VertArray[i].z as string + " "))
								)
							VertexArrayString = (VertexArrayString + "\n")
							format "%" VertexArrayString 

							FaceArray = for i = 1 to CObj.numfaces collect (getface CObj i)
							FaceArrayString = ""
							for i = 1 to FaceArray.count do
								(
									FaceArrayString = (FaceArrayString + (FaceArray[i].x as string + " "))
									FaceArrayString = (FaceArrayString + (FaceArray[i].y as string + " "))
									FaceArrayString = (FaceArrayString + (FaceArray[i].z as string + " "))
								)
							FaceArrayString = (FaceArrayString + "\n")
							format "%" FaceArrayString 
							close f

--							MatIDArray = for i = 1 to CObj.numfaces collect (GetFaceMatID CObj i)
--							TVertArray = #()
--							for i = 1 to FaceArray.count do
--								(
--									CFace = (GetTVFace CObj i)
--									if FindItem TVertArray CFace.x == 0 do (append TVertArray CFace.x)
--									if FindItem TVertArray CFace.y == 0 do (append TVertArray CFace.y)
--									if FindItem TVertArray CFace.z == 0 do (append TVertArray CFace.z)
--								)
--							TVertUVArray = (for i = 1 to TVertArray.count collect (GetTvert CObj TVertArray[i]))
--							for i = 1 to CObj.numfaces do
--								(
--									SetEdgeVis m i 1 (GetEdgeVis CObj i 1)
--									SetEdgeVis m i 2 (GetEdgeVis CObj i 2)
--									SetEdgeVis m i 3 (GetEdgeVis CObj i 3)
--								)
--							for i = 1 to CObj.numfaces do
--								SetFaceSmoothGroup m i (getFaceSmoothGroup CObj i)
--							Ntv = getNumTVerts CObj
--							for i = 1 to CObj.numfaces do
--								GetTVFace CObj i
--		
--							for i = 1 to Ntv do
--								GetTvert CObj i
--							CObj.name
--							ObjPivotOffset = m.pivot - m.center
						)
				) else (
					MessageBox "Needs an EditableMesh!"
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
	
					if OriginalPosition.checked then
						m.pos = CObj.pos
					else
						m.pos = [PosX.value,PosY.value,PosZ.value]
		
					m.wirecolor = NewWireColor.color
					update m
					if CObj.mat != undefined do
						m.mat = CObj.mat
				) else (
					MessageBox "Needs and Editable Mesh object\n please collapse your mesh"
				)
		)
)