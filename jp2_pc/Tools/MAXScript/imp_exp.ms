Utility ImportExport "Import Export"
(
	Button ExportObject "Export Selected" width:140
	button LoadObject "Import File" width:140
	label status1
	label status2
-- ********************************************************************************************************
-- Load Object
-- ********************************************************************************************************
	on LoadObject pressed do
		(
			if (ObjectFileName = (getOpenFileName Caption:"Choose Object File:")) != undefined do
				(
					f = openFile ObjectFileName
					while not eof f do
						(
							status2.text = "Object Info..."
							ObjectName = readline f
							ObjectWireColor = [(readValue f), (readValue f),(readValue f)] as color
							ObjectPivotPos = [(readValue f), (readValue f),(readValue f)]
							NumVerts = readvalue f
							NumFaces = readvalue f
							NumTVerts = readvalue f
							status2.text = "Loading Vertices..."
							VertArray = #()
							for i = 1 to NumVerts do
								(
									CVert = [(readValue f), (readValue f), (readValue f)]
									append VertArray CVert
								)
							status2.text = "Loading Faces..."
							FaceArray = #()
							for i = 1 to NumFaces do
								(
									CFace = [(readValue f), (readValue f), (readValue f)]
									append FaceArray CFace
								)
							m = mesh vertices:VertArray faces:FaceArray
							MatIDs = #()

							status2.text = "Loading Face Mat IDs..."
							for i = 1 to numFaces do
								setFaceMatID m i (readValue f)

							addModifier m (UVWMap())
							convertToMesh m
							SetNumTverts m NumTVerts true
							update m

							status2.text = "Loading Texture Faces..."
							for i = 1 to NumFaces do
									SetTVFace m i [(readValue f), (readValue f), (readValue f)]
							update m

							status2.text = "Loading Texture Verts..."
							for i = 1 to NumTVerts do
								SetTvert m i [(readvalue f), (readvalue f), (readvalue f)]

							status2.text = "Loading Edge Visibility..."
							for i = 1 to NumFaces do
								(
									SetEdgeVis m i 1 (readValue f)
									SetEdgeVis m i 2 (readValue f)
									SetEdgeVis m i 3 (readValue f)
								)
							status2.text = "Loading Smoothing groups..."
							for i = 1 to NumFaces do
								SetFaceSmoothGroup m i (readValue f)
							m.name = ObjectName
							m.wirecolor = ObjectWireColor
			 				m.pivot = ObjectPivotPos
							status2.text = "Loading Material Defs..."
							MatName = readLine f
							MatType = readValue f
							if MatType == -1 then
								(
									m.mat = standardMaterial()
									if (CLine = (ReadLine f)) != "-1" do
										(
											m.mat.diffusemap = bitmapTexture()
											m.mat.diffusemap.filename = CLine
										)
									if (CLine = (ReadLine f)) != "-1" do
										(
											m.mat.OpacityMap = bitmapTexture()
											m.mat.OpacityMap.filename = CLine
										)
									if (CLine = (ReadLine f)) != "-1" do
										(
											m.mat.Bumpmap = bitmapTexture()
											m.mat.Bumpmap.filename = CLine
										)
								) else (
									m.mat = MultiMaterial()
									m.mat.numsubs = MatType
									for i = 1 to MatType do
										(
											m.mat[i].name = readLine f
											if (CLine = (ReadLine f)) != "-1" do
												(
													m.mat[i].diffusemap = bitmapTexture()
													m.mat[i].diffusemap.filename = CLine
												)
											if (CLine = (ReadLine f)) != "-1" do
												(
													m.mat[i].OpacityMap = bitmapTexture()
													m.mat[i].OpacityMap.filename = CLine
												)
											if (CLine = (ReadLine f)) != "-1" do
												(
													m.mat[i].BumpMap = bitmapTexture()
													m.mat[i].BumpMap.filename = CLine
												)
										)
								)
							m.mat.name = MatName
							update m
							status2.text = "Complete..."
						)
					close f
					status2.text = ""
				)
		)
	
-- ********************************************************************************************************
-- Export Object
-- ********************************************************************************************************
	on ExportObject pressed do
		(
			if (SaveFilename = getSaveFilename caption:"Specify Export File:") != undefined do
				(
					f = createFile SaveFilename
					ObjArray = for obj in selection collect obj
					if ObjArray.count != 0 do
						for i = 1 to ObjArray.count do
							(
								status1.text = ("Exporting " + i as string + " of " + ObjArray.count as string)
								-- convert the object to an Editable Mesh Object
								status2.text = "Obj Attributes..."
								CObj = ObjArray[i]
								ConVertToMesh CObj
								-- Object Name
								format "%\n" (CObj.name as string) to:f
								-- Object WireColor
								CObjWireColor = CObj.WireColor
								CObjWireColor = [CObjWireColor.r as integer, CObjWireColor.g as integer, CObjWireColor.b as integer]
								format "% % %\n" CObjWireColor.y CObjWireColor.x CObjWireColor.z to:f
								-- Object Pivot Point
								format "% % %\n" CObj.pivot.x CObj.pivot.y CObj.pivot.z to:f
								-- Object Stats
								format "% % %\n" CObj.numverts CObj.numfaces CObj.numTverts to:f
								print (gc())
								status2.text = "Vertices..."
								VertArray = for i = 1 to CObj.numverts collect (getvert CObj i)
								VertexArrayString = ""
								for i = 1 to VertArray.count do
									(
										VertexArrayString = (VertexArrayString + (VertArray[i].x as string + " "))
										VertexArrayString = (VertexArrayString + (VertArray[i].y as string + " "))
										VertexArrayString = (VertexArrayString + (VertArray[i].z as string + "\n"))
									)
								VertexArrayString = VertexArrayString
								-- Vertex Array
								format "%" VertexArrayString to:f
								print (gc())
								status2.text = "Faces..."
								FaceArray = for i = 1 to CObj.numfaces collect (getface CObj i)
								FaceArrayString = ""
								for i = 1 to FaceArray.count do
									(
										FaceArrayString = (FaceArrayString + ((FaceArray[i].x as integer) as string + " "))
										FaceArrayString = (FaceArrayString + ((FaceArray[i].y as integer) as string + " "))
										FaceArrayString = (FaceArrayString + ((FaceArray[i].z as integer) as string + "\n"))
									)
								FaceArrayString = FaceArrayString
								-- Face Array
								format "%" FaceArrayString to:f
								print (gc())
								status2.text = "Material IDs..."
								MatIDArray = for i = 1 to CObj.numfaces collect (GetFaceMatID CObj i)
								MATIDString = ""
								for i = 1 to CObj.NumFaces do
									MatIDString = (MATIDString + ((getFaceMatID CObj i) as string) + "\n")
								-- Material IDs Array
								format "%" MatIDString to:f
								print (gc())
								
								status2.text = "Texture Faces..."
								TVfaceString = ""
								TVFaces = #()
								for i = 1 to CObj.NumFaces do
									(
										CTVface = (GetTVFace CObj i)
										TVFaceString = (TVFaceString + (CTVface.x as integer) as string)
										TVFaceString = (TVFaceString + " " + (CTVface.y as integer) as string)
										TVFaceString = (TVFaceString + " " + (CTVface.z as integer) as string + "\n")
									)
								-- TVFaces
								format "%" TVFaceString to:f
								print (gc())
								
								status2.text = "Texture Verts..."
								nTv = CObj.numTVerts
								TVertUVArray = (for i = 1 to nTv collect (GetTvert CObj i))
								TVertUVArrayString = ""
								for i = 1 to TVertUVArray.count do
									(
										CTvert = TVertUVArray[i]
										TVertUVArrayString += ((CTvert.x as string) + " " + (CTvert.y as string) + " " + (CTvert.z as string) + "\n")
									)
								format "%" TVertUVArrayString to:f
								print (gc())
								status2.text = "Edge Visibility..."
	
								EdgeVisString = ""
								for i = 1 to CObj.numfaces do
									(
										EdgeVisString = (EdgeVisString + (GetEdgeVis CObj i 1) as string)
										EdgeVisString = (EdgeVisString + " " + (GetEdgeVis CObj i 2) as string)
										EdgeVisString = (EdgeVisString + " " + (GetEdgeVis CObj i 3) as string + "\n")
									)
								format "%" EdgeVisString to:f
								print (gc())
	
								status2.text = "Smoothing groups..."
								SmoothGroupString = ""
								for i = 1 to CObj.numfaces do
									SmoothGroupString = ((SmoothGroupString + ((getFaceSmoothGroup CObj i) as integer) as string) + "\n")
								format "%" SmoothGroupString to:f 
								print (gc())
	
								status2.text = "Materials..."
								if CObj.mat.name != "" then
									MatName = CObj.mat.name
								else
									MatName = "undefined"
	
								format "%\n" MatName to:f
								if classOf CObj.mat == StandardMaterial do
									(
										format "-1\n" to:f
										if classOf CObj.mat.diffuseMap == bitmapTexture then
											format "%\n" CObj.mat.diffuseMap.filename to:f
										else
											format "-1\n" to:f
	
										if classOf CObj.mat.OpacityMap == bitmapTexture then
											format "%\n" CObj.mat.opacityMap.filename to:f
										else
											format "-1\n" to:f
	
										if classOf CObj.mat.BumpMap == bitmapTexture then
											format "%\n" CObj.mat.BumpMap.filename to:f
										else
											format "-1\n" to:f
									)
								if classOf CObj.mat == MultiMaterial do
									(
										ns = CObj.mat.numsubs
										format "%\n" ns to:f
										for i = 1 to ns do
											(
												format "%\n" (CObj.mat[i].name as string) to:f
												if classOf CObj.mat[i].diffuseMap == bitmapTexture then
													format "%\n" CObj.mat[i].diffuseMap.filename to:f
												else
													format "-1\n" to:f
			
												if classOf CObj.mat[i].OpacityMap == bitmapTexture then
													format "%\n" CObj.mat[i].opacityMap.filename to:f
												else
													format "-1\n" to:f
			
												if classOf CObj.mat[i].BumpMap == bitmapTexture then
													format "%\n" CObj.mat[i].BumpMap.filename to:f
												else
													format "-1\n" to:f
											)
									)
								status2.text = "Complete..."
							)
					close f -- Close the text file
					status1.text = ""
					status2.text = ""
					print (gc())
					messageBox "Done Exporting!"
				)
		)
)