-- this function returns an array of three arrays - #(#(TVert indices), #(TVert UV's), #(Vertex positions)).
-- it takes an extra argument - #selection, or #all, which determines all the faces, or the current face selection
fn GetTVs CObj =
	(
		local FaceArray = #()
		local nf = CObj.numfaces
		local TVertArray = #()
		local TVertUVArray = #()
		local TVFaceArray = #()
		local VertArray = #()
		-- get the faces into FaceArray#()
		FaceArray = getFaceSelection CObj
		if FaceArray.count == 0 do
			FaceArray = for i = 1 to CObj.numfaces collect i
		-- get all the Tverts
		for i = 1 to FaceArray.count do append TVFaceArray (getTVFace CObj FaceArray[i])
		for i = 1 to TVFaceArray.count do
			(
				CFace = getFace CObj FaceArray[i]
				if FindItem TVertArray TVFaceArray[i].x == 0 do 
					(
						append TVertArray (TVFaceArray[i].x as integer)
						append VertArray (getvert CObj CFace.x)
					)
				if FindItem TVertArray TVFaceArray[i].y == 0 do 
					(
						append TVertArray (TVFaceArray[i].y as integer)
						append VertArray (getvert CObj CFace.y)
					)
				if FindItem TVertArray TVFaceArray[i].z == 0 do 
					(
						append TVertArray (TVFaceArray[i].z as integer)
						append VertArray (getvert CObj CFace.z)
					)
			)
		-- get the TVert point3's
		TVertUVArray = (for i = 1 to TVertArray.count collect (GetTVert CObj TVertArray[i]))
		AllTVerts = #(TVertArray, TVertUVArray, VertArray)
		return AllTVerts
	)

--*************************************************************************************************
--* Utility Start
--*************************************************************************************************
Utility TrnTexUVs "Terrain Texture Mapper"
(

local TextureObject

group "Setup"
	(
		spinner mapSizeW "Source map width:" range:[16,256,256] type:#integer fieldwidth:35
		spinner mapSizeH "Source map height:" range:[16,256,256] type:#integer fieldwidth:35
		button loadtexture "Load Bitmap"
	)

group "Modifications" 
	(
		spinner PpURatio "Desired Pixels/M:" range:[1,256,32] type:#integer fieldwidth:35
	)

pickbutton GetTexObject "Choose Texture Object" width:140
button ApplyToSelected "Apply to Selected" width:140

on ApplyToSelected pressed do
	(
		ObjArray = for obj in selection collect obj
		for i = 1 to ObjArray.count do
			(
				TextureObject = ObjArray[i]
				if classOf TextureObject == editable_mesh then
					(
						TextureObjectScale = TextureObject.max - TextureObject.min
						format "Texture Object Scale: %\n" TextureObjectScale
						TextureObjectMin = TextureObject.min -- this is the lower left point of the mesh
						TextureObjectMax = TextureObject.max -- this is the upper right point of the mesh
						NumTilesU = TextureObjectScale.x / (mapSizeW.value / PpURatio.value as float)
						NumTilesV = TextureObjectScale.y / (mapSizeH.value / PpURatio.value as float)
						format "NumTilesU: %\n" NumTilesU
						format "NumTilesV: %\n" NumTilesV
						-- get the texture vertices
						Tverts = GetTVs TextureObject
						TvertIndices = TVerts[1]
						TvertUVs = TVerts[2]
						VertPositions = TVerts[3]

						-- bring all the percieved vertex positions into 0 => 1 scale
						for i = 1 to VertPositions.count do
							VertPositions[i] = [(VertPositions[i].x - TextureObjectMin.x), (VertPositions[i].y - TextureObjectMin.y), VertPositions[i].z]
		
						for i = 1 to VertPositions.count do
							VertPositions[i] = [(VertPositions[i].x / TextureObjectScale.x), (VertPositions[i].y / TextureObjectScale.y), VertPositions[i].z]
		
						for i = 1 to TvertIndices.count do
							(
								TvertUVs[i].x = NumTilesU * VertPositions[i].x
								TvertUVs[i].y = NumTilesV * VertPositions[i].y
								setTvert TextureObject TvertIndices[i] TvertUVs[i]
							) 
						update TextureObject
					)
			)
		TextureObject = undefined
	)

on loadtexture pressed do
	(
		if (TexMap = SelectBitmap caption:"Choose Bitmap") != undefined do
			(
				mapSizeW.value = TexMap.width
				mapSizeH.value = TexMap.height
			)
	)
	
on mapSizeW changed state do
	(
		if TextureObject != undefined do
			(
				TextureObjectScale = TextureObject.max - TextureObject.min
				format "Texture Object Scale: %\n" TextureObjectScale
				TextureObjectMin = TextureObject.min -- this is the lower left point of the mesh
				TextureObjectMax = TextureObject.max -- this is the upper right point of the mesh
				NumTilesU = TextureObjectScale.x / (mapSizeW.value / PpURatio.value as float)
				NumTilesV = TextureObjectScale.y / (mapSizeH.value / PpURatio.value as float)
				format "NumTilesU: %\n" NumTilesU
				format "NumTilesV: %\n" NumTilesV
				-- get the texture vertices
				Tverts = GetTverts TextureObject

				TvertIndices = TVerts[1]
				TvertUVs = TVerts[2]
				VertPositions = TVerts[3]

				-- bring all the percieved vertex positions into 0 => 1 scale

				for i = 1 to VertPositions.count do
					VertPositions[i] = [(VertPositions[i].x - TextureObjectMin.x), (VertPositions[i].y - TextureObjectMin.y), VertPositions[i].z]

				for i = 1 to VertPositions.count do
					VertPositions[i] = [(VertPositions[i].x / TextureObjectScale.x), (VertPositions[i].y / TextureObjectScale.y), VertPositions[i].z]

				for i = 1 to TvertIndices.count do
					(
						TvertUVs[i].x = NumTilesU * VertPositions[i].x
						TvertUVs[i].y = NumTilesV * VertPositions[i].y
						setTvert TextureObject TvertIndices[i] TvertUVs[i]
					) 
				update TextureObject
			)
	)


on mapSizeH changed state do
	(
		if TextureObject != undefined do
			(
				TextureObjectScale = TextureObject.max - TextureObject.min
				format "Texture Object Scale: %\n" TextureObjectScale
				TextureObjectMin = TextureObject.min -- this is the lower left point of the mesh
				TextureObjectMax = TextureObject.max -- this is the upper right point of the mesh
				NumTilesU = TextureObjectScale.x / (mapSizeW.value / PpURatio.value as float)
				NumTilesV = TextureObjectScale.y / (mapSizeH.value / PpURatio.value as float)
				format "NumTilesU: %\n" NumTilesU
				format "NumTilesV: %\n" NumTilesV
				-- get the texture vertices
				Tverts = GetTverts TextureObject

				TvertIndices = TVerts[1]
				TvertUVs = TVerts[2]
				VertPositions = TVerts[3]

				-- bring all the percieved vertex positions into 0 => 1 scale

				for i = 1 to VertPositions.count do
					VertPositions[i] = [(VertPositions[i].x - TextureObjectMin.x), (VertPositions[i].y - TextureObjectMin.y), VertPositions[i].z]

				for i = 1 to VertPositions.count do
					VertPositions[i] = [(VertPositions[i].x / TextureObjectScale.x), (VertPositions[i].y / TextureObjectScale.y), VertPositions[i].z]

				for i = 1 to TvertIndices.count do
					(
						TvertUVs[i].x = NumTilesU * VertPositions[i].x
						TvertUVs[i].y = NumTilesV * VertPositions[i].y
						setTvert TextureObject TvertIndices[i] TvertUVs[i]
					) 
				update TextureObject
			)
	)

	
on PpURatio changed state do
	(
		if TextureObject != undefined do
			(
				TextureObjectScale = TextureObject.max - TextureObject.min
				format "Texture Object Scale: %\n" TextureObjectScale
				TextureObjectMin = TextureObject.min -- this is the lower left point of the mesh
				TextureObjectMax = TextureObject.max -- this is the upper right point of the mesh
				NumTilesU = TextureObjectScale.x / (mapSizeW.value / PpURatio.value as float)
				NumTilesV = TextureObjectScale.y / (mapSizeH.value / PpURatio.value as float)
				format "NumTilesU: %\n" NumTilesU
				format "NumTilesV: %\n" NumTilesV
				-- get the texture vertices
				Tverts = GetTverts TextureObject

				TvertIndices = TVerts[1]
				TvertUVs = TVerts[2]
				VertPositions = TVerts[3]

				-- bring all the percieved vertex positions into 0 => 1 scale

				for i = 1 to VertPositions.count do
					VertPositions[i] = [(VertPositions[i].x - TextureObjectMin.x), (VertPositions[i].y - TextureObjectMin.y), VertPositions[i].z]

				for i = 1 to VertPositions.count do
					VertPositions[i] = [(VertPositions[i].x / TextureObjectScale.x), (VertPositions[i].y / TextureObjectScale.y), VertPositions[i].z]

				for i = 1 to TvertIndices.count do
					(
						TvertUVs[i].x = NumTilesU * VertPositions[i].x
						TvertUVs[i].y = NumTilesV * VertPositions[i].y
						setTvert TextureObject TvertIndices[i] TvertUVs[i]
					) 
				update TextureObject
			)
	)

on GetTexObject picked obj do
	(
		if classOf obj == editable_mesh then
			(
				TextureObject = obj
				TextureObjectScale = TextureObject.max - TextureObject.min
				format "Texture Object Scale: %\n" TextureObjectScale
				TextureObjectMin = TextureObject.min -- this is the lower left point of the mesh
				TextureObjectMax = TextureObject.max -- this is the upper right point of the mesh
				NumTilesU = TextureObjectScale.x / (mapSizeW.value / PpURatio.value as float)
				NumTilesV = TextureObjectScale.y / (mapSizeH.value / PpURatio.value as float)
				format "NumTilesU: %\n" NumTilesU
				format "NumTilesV: %\n" NumTilesV
				-- get the texture vertices
				Tverts = GetTverts TextureObject

				TvertIndices = TVerts[1]
				TvertUVs = TVerts[2]
				VertPositions = TVerts[3]

				-- bring all the percieved vertex positions into 0 => 1 scale

				for i = 1 to VertPositions.count do
					VertPositions[i] = [(VertPositions[i].x - TextureObjectMin.x), (VertPositions[i].y - TextureObjectMin.y), VertPositions[i].z]

				for i = 1 to VertPositions.count do
					VertPositions[i] = [(VertPositions[i].x / TextureObjectScale.x), (VertPositions[i].y / TextureObjectScale.y), VertPositions[i].z]

				for i = 1 to TvertIndices.count do
					(
						TvertUVs[i].x = NumTilesU * VertPositions[i].x
						TvertUVs[i].y = NumTilesV * VertPositions[i].y
						setTvert TextureObject TvertIndices[i] TvertUVs[i]
					) 
				update TextureObject
			) else (
				MessageBox "Needs an Editable Mesh Object!"
			)
	
	
	)

)