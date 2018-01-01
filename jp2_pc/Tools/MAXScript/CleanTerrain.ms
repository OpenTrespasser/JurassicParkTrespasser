fn roundTo val n = 
	(
		local mult = 10.0 ^ n
		(floor ((val * mult) + 0.5)) / mult
	)

Utility CleanTerrainMesh "Clean Terrain Mesh"
(
	dropDownList TerrainResolution "Terrain Resolution" items:#("64","32","16","8","4","2","1","1/2","1/4")
	
	pickbutton ChooseTerrain "Choose Terrain"
	
	on ChooseTerrain picked obj do	
		(
			if classof obj == editable_mesh then
				(
					undo on
						(
							if TerrainResolution.selection == 1 do (TerrainRez = 64)
							if TerrainResolution.selection == 2 do (TerrainRez = 32)
							if TerrainResolution.selection == 3 do (TerrainRez = 16)
							if TerrainResolution.selection == 4 do (TerrainRez = 8)
							if TerrainResolution.selection == 5 do (TerrainRez = 4)
							if TerrainResolution.selection == 6 do (TerrainRez = 2)
							if TerrainResolution.selection == 7 do (TerrainRez = 1)
							if TerrainResolution.selection == 8 do (TerrainRez = 0.5)
							if TerrainResolution.selection == 9 do (TerrainRez = 0.25)
							CObj = obj
							nv = CObj.numverts
							for i = 1 to nv do
								(
									CVert = (getvert CObj i)
									-- first the x coordinate...
									x = CVert.x
									x = (roundTo (x as float / TerrainRez as float) 0) * TerrainRez
		
									y = CVert.y
									y = (roundTo (y as float / TerrainRez as float) 0) * TerrainRez
									Cvert = [x,y,CVert.z]
									SetVert CObj i CVert
								)
							update CObj
						)
				) else (
					messageBox "Needs an Editable Mesh object"
				)
		)
)