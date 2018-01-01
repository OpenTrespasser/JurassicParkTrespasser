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

Utility TriggerTools "TriggerTools"
(

local 	CTerrain,
				CRay = ray [0,0,0] [0,0,-1]

	group "Placement Tools"
		(
			label TerrainLebal "Terrain:" align:#left offset:[-4,0]
			pickButton PickTerrain width:100 align:#right offset:[0,-20]
			button PlaceSelected "Place Selected" width:140
		)

	on PickTerrain picked obj do
		(
			if classOf obj == editable_mesh then
				(
					CTerrain = obj
					PickTerrain.text = Cterrain.name
				) else (
					messageBox "Needs an Editable Mesh Object.\n Please collapse your mesh"
				)
		)


	on PlaceSelected pressed do
		(
			if CTerrain != undefined then
				(
					ObjArray = for obj in selection collect obj
					oc = ObjArray.count
					ocInv = (1.0 / oc) * 100
					ProgressStart "Placing Triggers....."
					for i = 1 to oc do
						(
							progressUpdate (i * ocInv)
							CObj = ObjArray[i]
							if CObj != CTerrain do
								(
									CRay.pos = [CObj.pos.x, CObj.pos.y, CTerrain.max.z + 100] 
									if (ir = intersectRay CTerrain CRay) != undefined do
										CObj.pos = ir.pos
								)
						)
					ProgressEnd()
				) else (
					messageBox "Terrain is not defined!"
				)
		)
)