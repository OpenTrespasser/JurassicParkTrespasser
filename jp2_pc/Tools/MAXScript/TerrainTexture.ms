-- TerrainTexture.ms
Utility TerrainTexturePlacer "TerrainTexture"
(

local TerrainObj, OffsetZ, taperay, NewVertPos, tapeobj, Objectcount, CurrentObjCount;


group "Choose Objects"
(
Pickbutton PickTape "Choose Tape"
label tape_label "-- none --"
Pickbutton PickTerrain "Choose Terrain"
label terrain_label "-- none --"
)

group "Distribution Opitions"
(
spinner OffsetC_spin "Offset C (High)" range:[.5,1,.75] type: #float
spinner OffsetB_spin "Offset B (Med)" range:[.3,.5,.4] type: #float
spinner OffsetA_spin "Offset A (Low)" range:[0,.3,.1] type: #float
)

group "Distribution"
(
button placeSel "Place Selected"
)

on PickTape picked obj do
(
tapeobj = obj
taperay = tapeobj as ray
taperay.dir = [0,0,-1]
tape_label.text = tapeobj.name
)

on PickTerrain picked obj do
(
TerrainObj = obj
terrain_label.text = TerrainObj.name
)

on PlaceSel pressed do
(
CurrentObjCount = 0
Objectcount = 0
for o in objects do (if o.isselected do (Objectcount = Objectcount +1))
ProgressStart "Placing Texture"
for o in objects do
	(
		if o.isselected do
		(	
			CurrentObjCount = CurrentObjCount +1
			progress = ((CurrentObjCount as float / Objectcount as float) * 100)
			ProgressUpdate progress
			nv = o.numverts
			objectname = o.name
			ONameCount = objectname.count
			if substring objectname Onamecount 1 == "A" do (OffsetZ = OffsetA_spin.value)
			if substring objectname Onamecount 1 == "B" do (OffsetZ = OffsetB_spin.value)
			if substring objectname Onamecount 1 == "C" do (OffsetZ = OffsetC_spin.value)
			for i = 1 to nv do
			(taperay.pos = [(getvert o i).x, (getvert o i).y, 1000]
			NewVertPos = (intersectray TerrainObj Taperay)
			setvert o i [(NewVertPos.pos).x, (NewVertPos.pos).y, (NewVertPos.pos).z + OffsetZ]
			)
		update o
		)
	)
ProgressEnd()
)
)


