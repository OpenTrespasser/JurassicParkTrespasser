Utility MapInfo "Map Info"
(

radiobuttons infotype labels:#("All", "Selected")
button getMapInfo "Get Bitmap Info" width:140
label foo "Map Paths are Displayed"
label foo2 "in Listener Window"

on getMapInfo pressed do
(
ObjArray = #()
if infotype.state == 1 do (ObjArray = for o in objects collect o)
if infotype.state == 2 do (ObjArray = for obj in selection collect obj)
numObj = ObjArray.count

for o in objArray do
(
	format "Object: %\n" o.name
	if classof o.mat == standardmaterial do
	(
	if classof o.mat.diffusemap == bitmaptexture do (format "%\n" o.mat.diffusemap.filename)
	if classof o.mat.opacitymap == bitmaptexture do (format "%\n" o.mat.opacitymap.filename)
	if classof o.mat.bumpmap == bitmaptexture do (format "%\n" o.mat.bumpmap.filename)
	)

	if classof o.mat == multimaterial do
	(
	ns = o.mat.numsubs
	for i = 1 to ns do
		(
		if classof o.mat[i].diffusemap == bitmaptexture do (format "%\n" o.mat[i].diffusemap.filename)
		if classof o.mat[i].opacitymap == bitmaptexture do (format "%\n" o.mat[i].opacitymap.filename)
		if classof o.mat[i].bumpmap == bitmaptexture do (format "%\n" o.mat[i].bumpmap.filename)
		)
	)
)
)

)
