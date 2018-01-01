Utility TerrainTexture "Terrain Texture"
(

label label1 "Target Path: E:\\TrnObjTemp\\"
button GrabMaps "Grab Maps" width:140

on GrabMaps pressed do
(
NewPath = "E:\\TrnObjTemp\\"
TextureObjects = for obj in $TrnObj* collect obj
TexObjCount = TextureObjects.count
for i = 1 to TexObjCount do
	(	
	o = TextureObjects[i]
	MapName = FilenameFromPath o.mat.diffusemap.filename
	copyfile o.mat.diffusemap.filename (NewPath + MapName)
	if o.mat.opacitymap != undefined do
		(
		MapName = FilenameFromPath o.mat.opacitymap.filename
		copyfile o.mat.opacitymap.filename (NewPath + MapName)
		)
	)
)
)

