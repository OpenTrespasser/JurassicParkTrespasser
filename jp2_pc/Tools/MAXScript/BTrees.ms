fn LowestVert obj =
	(
		-- returns the index of the vertex with the lowest pos.z value
		local LowVertPos = obj.max.z
		local LowestVert = -1
		nv = obj.numverts
		for i = 1 to nv do
			(	
				local Cvert = getvert obj i
				if Cvert.z < LowVertPos do
					(
						LowVertPos = Cvert.z
						LowestVert = i
					)
			)	
		return LowestVert
	)

Utility BGTrees "BG Trees Manager"
	(

		local Terrain, ViewPoint, ViewPos

		label TerrainLabel "Terrain:" align:#left offset:[-8,0]
		pickbutton ChooseTerrain width:90 offset:[8,-20] align:#right
		label ViewerLabel "Player Pos:" align:#left offset:[-8,0]
		pickbutton ChooseViewPoint width:90 offset:[8,-20] align:#right
		group "Placement Options"
			(
				checkbox Attract "Attract to Player" enabled:false
				checkbox IgnoreZ "Ignore Z" offset:[10,0] enabled:false
				checkbox ScaleViaDistance "Scale for Distance"	
				spinner StartScale "Start:" range:[10,200,100] enabled:false
				spinner EndScale "End:" range:[10,400,100] enabled:false
			)
		button Place "Place BG Objects" width:140

		on Attract changed state do
			if Attract.checked then
				IgnoreZ.enabled = true
			else
				IgnoreZ.enabled = false

		on ScaleViaDistance changed state do
			if ScaleViaDistance.checked then
				StartScale.enabled = EndScale.enabled = true
			else
				StartScale.enabled = EndScale.enabled = false

		on ChooseViewPoint picked obj do
			(
				ViewPoint = obj
				ChooseViewPoint.text = ViewPoint.name
			)

		on ChooseTerrain picked obj do
			(
				Terrain = obj
				ChooseTerrain.text = Terrain.name
			)

		on Place pressed do
			if Terrain != undefined and ViewPoint != undefined do
				(
					ViewPos = ViewPoint.pos
					tapeRay = ray [0,0,0] [0,0,-1]
					Sel = selection as array
					KeyObjects = #()
					KeyNames = #()
					MAXDist = -1

					sc = (1.0 / Sel.count) * 100.0
					ProgressStart "Placing...."
					if ScaleViaDistance.checked do
						(
							for i = 1 to Sel.count do
								if (CDist = distance ViewPos Sel[i].pos) > MAXDist do
									MAXDist = CDist
							
							MAXDist = 1.0 / MAXDist
							MAXScale = EndScale.value - StartScale.value
						)

					ap = ViewPoint.pos
					
					for i = 1 to Sel.count do
						(
							ProgressUpdate (i * sc)
							CObj = Sel[i]
							
							if Attract.checked do
								(
									CPos = CObj.pos
									CScale = CObj.scale
									if IgnoreZ.checked do CPos = [CPos.x, CPos.y, ap.z]
									CObj.dir = (ap - Cpos)
									CDir = CObj.dir
									if Cdir.x < 0 do in coordsys local rotate CObj 180 z_axis
									CObj.scale = CScale
								)

							if ScaleViaDistance.checked do
								(
									-- get the distance as a percentage from the tree to the viewer position
									CDist = (distance ViewPos CObj.pos)
									p = (CDist * MAXDist)
									NScale = (MAXScale * p + StartScale.value) * 0.01
									CObj.scale = [NScale, NScale, NScale]
								)
							LowVertPos = (getvert CObj (LowestVert CObj))
							LowVertOffset = (LowVertPos - CObj.pivot)
							Taperay.pos = [LowVertPos.x, LowVertPos.y, 10000.0]
							if (ir = intersectray terrain tapeRay) != undefined do
								(
									ObjHeightOffset =((CObj.pivot).z - LowVertPos.z)
									CObj.pos = [(ir.pos).x - LowVertOffset.x, (ir.pos).y - LowVertOffset.y, ((ir.pos).z + ObjHeightOffset)]
								)
						)
					ProgressEnd()
				)
	)

