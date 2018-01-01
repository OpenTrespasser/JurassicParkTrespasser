fn Power_of_two_num num = 
(
validNum = false
if num == 8 do (ValidNum = true)
if num == 16 do (ValidNum = true)
if num == 32 do (ValidNum = true)
if num == 64 do (ValidNum = true)
if num == 128 do (ValidNum = true)
if num == 256 do (ValidNum = true)
return ValidNum
)

-- takes a bitmap and returns wether it's power of two or not
fn Power_of_two_Map BMap = 
(
ValidWidth = Power_of_two_num BMap.width
ValidHeight = Power_of_two_num BMap.height
if validWidth == validHeight == true then return true else return false
)

Utility SuperMapInfo "Scene Map Info"
(

group "Options"
(
	radiobuttons DataFrom labels:#("All", "Selected") default:2
	label checkfor "Check for:" align:#left
	checkbox CheckMapSizes "Check Map Sizes" checked:true
	checkbox CheckPow2 "Check power of 2" checked:true
)

button GatherData "Gather Data" width:140

on GatherData pressed do
	(
		if (SaveFilename = getsaveFilename caption:"Specify Data File...") != undefined do
			(
				f = createfile SaveFilename
				CDiffuseMap = COpacityMap = CBumpMap = undefined
				if Datafrom.state == 1 do (ObjArray = for o in objects collect o)
				if Datafrom.state == 2 do (ObjArray = for obj in selection collect Obj)
				oc = ObjArray.count
				for i = 1 to oc do
					(
						o = ObjArray[i]

						UseBump = UseOPacity = UseDiffuse = false

						if classof o.mat == standardMaterial do
							(
								
								if classof o.mat.diffusemap == bitmaptexture do
									(
										CDiffuseMap = openbitmap o.mat.diffusemap.filename
										CDiffuseSize = [CDiffuseMap.width, CDiffuseMap.height]
										if CheckPow2.checked do
											if Power_of_two_Map CDiffuseMap == false do
												format "Map:% on object % is size %\n" CDiffuseMap.filename o.name CDiffuseSize to:f
										UseDiffuse = true
									)

								if classof o.mat.opacitymap == bitmaptexture do
									(
										COpacityMap = openbitmap o.mat.diffusemap.filename
										COpacitySize = [COpacityMap.width, COpacityMap.height]
										if CheckPow2.checked do
											if Power_of_two_Map COPacityMap == false do
												format "Map:% on object % is size %\n" COpacityMap.filename o.name COpacitySize to:f
										UseOPacity = true
									)

								if classof o.mat.bumpmap == bitmaptexture do
									(
										CBumpMap = openbitmap o.mat.diffusemap.filename
										CBumpSize = [CBumpMap.width, CBumpMap.height]
										if CheckPow2.checked do
											if Power_of_two_Map CBumpMap == false do
												format "Map:% on object % is size %\n" CBumpMap.filename o.name CBumpSize to:f
										UseBump = true
									)
								
								if CheckMapSizes.checked do
									(
										if UseDiffuse == UseOPacity and UseDiffuse == UseBump do
											(
												if CDiffuseSize != COpacitySize do
													format "DiffuseMap: %, % and OpacityMap:%, % are not the same size\n" CDiffuseMap.filename CDiffuseSize COpacityMap.filename COpacitySize to:f

												if CDiffuseSize != CBumpSize do
													format "DiffuseMap: %, % and BumpMap:%, % are not the same size\n" CDiffuseMap.filename CDiffuseSize CBumpMap.filename CBumpSize to:f
											)

										if UseDiffuse == UseOPacity and UseDiffuse != UseBump do
											if CDiffuseSize != COpacitySize do
												format "DiffuseMap: %, % and OpacityMap:%, % are not the same size\n" CDiffuseMap.filename CDiffuseSize COpacityMap.filename COpacitySize to:f
	
										if UseDiffuse != UseOPacity and UseDiffuse == UseBump do
											if CDiffuseSize != CBumpSize do
												format "DiffuseMap: %, % and BumpMap:%, % are not the same size\n" CDiffuseMap.filename CDiffuseSize CBumpMap.filename CBumpSize to:f
									)
							)

-- Do MultiMaterials
						if classof o.mat == MultiMaterial do
							(
								ns = o.mat.numsubs
								for i = 1 to ns do
									(
										if classof o.mat[i].diffusemap == bitmaptexture do
											(
												CDiffuseMap = openbitmap o.mat[i].diffusemap.filename
												CDiffuseSize = [CDiffuseMap.width, CDiffuseMap.height]
												if CheckPow2.checked do
													if Power_of_two_Map CDiffuseMap == false do
														format "Map:% on object % is size %\n" CDiffuseMap.filename o.name CDiffuseSize to:f
												UseDiffuse = true
											)
		
										if classof o.mat[i].opacitymap == bitmaptexture do
											(
												COpacityMap = openbitmap o.mat[i].diffusemap.filename
												COpacitySize = [COpacityMap.width, COpacityMap.height]
												if CheckPow2.checked do
													if Power_of_two_Map COPacityMap == false do
														format "Map:% on object % is size %\n" COpacityMap.filename o.name COpacitySize to:f
												UseOPacity = true
											)
		
										if classof o.mat[i].bumpmap == bitmaptexture do
											(
												CBumpMap = openbitmap o.mat[i].diffusemap.filename
												CBumpSize = [CBumpMap.width, CBumpMap.height]
												if CheckPow2.checked do
													if Power_of_two_Map CBumpMap == false do
														format "Map:% on object % is size %\n" CBumpMap.filename o.name CBumpSize to:f
												UseBump = true
											)
								
										if CheckMapSizes.checked do
											(
												if UseDiffuse == UseOPacity and UseDiffuse == UseBump do
													(
														if CDiffuseSize != COpacitySize do
															format "DiffuseMap: %, % and OpacityMap:%, % and not the same size\n" CDiffuseMap.filename CDiffuseSize COpacityMap.filename COpacitySize to:f

														if CDiffuseSize != CBumpSize do
															format "DiffuseMap: %, % and BumpMap:%, % and not the same size\n" CDiffuseMap.filename CDiffuseSize CBumpMap.filename CBumpSize to:f
													)

												if UseDiffuse == UseOPacity and UseDiffuse != UseBump do
													if CDiffuseSize != COpacitySize do
														format "DiffuseMap: %, % and OpacityMap:%, % and not the same size\n" CDiffuseMap.filename CDiffuseSize COpacityMap.filename COpacitySize to:f
	
												if UseDiffuse != UseOPacity and UseDiffuse == UseBump do
													if CDiffuseSize != CBumpSize do
														format "DiffuseMap: %, % and BumpMap:%, % and not the same size\n" CDiffuseMap.filename CDiffuseSize CBumpMap.filename CBumpSize to:f
											)
									)	
							)
					)
			)	
		close f
	)
) -- End Utility