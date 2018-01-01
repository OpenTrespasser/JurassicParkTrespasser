--**********************************************************************************************
-- *
-- * Copyright © DreamWorks Interactive, 1997
-- *
-- * Contents:
-- *		Implementation of Zrelax.ms
-- * Bugs:
-- *
-- * To do:
-- *
-- * Notes:
-- *		Uses the Mesh Modifier, Relax, to alter the mesh, but 
-- *		retains the positions of the vertecies in XY space.
-- **********************************************************************************************

Utility ZRelax "ZRelax"
(
	local debug = true
	
	-- bitmap titleMap filename:"C:\Zrelax.bmp" offset:[-27,0]
	group "ZRelax Options"
		(
			Spinner relax_itr "Relax Iterations: " range:[0,40,3] type:#integer fieldwidth:25 offset:[-25,0]
			label affect "Relax:" align:#left
			radiobuttons relaxWhat labels:#("Whole Object", "Selected Faces") offset:[-25,0] default:2
		)
	pickbutton PickObject "Pick Object" width:140
	label status1

	on PickObject picked obj do
		(
			NumIterations = relax_itr.value as integer
			terrain = obj
			nv = terrain.numverts
			progscale = 100.0 / nv

			if RelaxWhat.state == 1 do
				(
					ProgressStart "Relaxing..."
					InitVertPos = (for i = 1 to nv collect (getvert terrain i))
					addmodifier terrain (relax())
					terrain.modifiers[1].iterations = NumIterations
					terrain.modifiers[1].keep_boundary_pts_fixed = 0
					ConvertToMesh terrain
					NewVertPos = for i = 1 to nv collect getvert terrain i
					for i = 1 to nv do 
						(
							setvert terrain i [InitVertPos[i].x, InitVertPos[i].y, NewVertPos[i].z]
							if ProgressUpdate (i * progscale) == false then exit
						)
					update terrain
					ProgressEnd()
				)
			
			if RelaxWhat.state == 2 do
				(
					ProgressStart "Initializing..."
					InitVertPos = for i = 1 to nv collect getvert terrain i
					Faces = getfaceselection terrain
					nf = Faces.count
					progscale = 100.0 / nf
					Faces2 = (for i = 1 to nf collect (getface terrain Faces[i]))
					setVertSelection terrain #()
					for i = 1 to nf do
						(
							setVertSelection Terrain #(Faces2[i].x) keep:true
							setVertSelection Terrain #(Faces2[i].y) keep:true
							setVertSelection Terrain #(Faces2[i].z) keep:true
							if ProgressUpdate (i * progscale) == false then exit
						)
					ProgressEnd()
					status1.text = "Updating mesh..."
					update Terrain
					status1.text = "Garbage collecting..."
					-- gc()
					status1.text = "getting the vertex selection..."
					SelVertIndexlist = getVertSelection Terrain
					status1.text = "adding modifier...."
					addmodifier terrain (relax())
					terrain.modifiers[1].iterations = NumIterations
					terrain.modifiers[1].keep_boundary_pts_fixed = 0
					status1.text = "converting terrain back into a mesh..."
					ConvertToMesh terrain
					status1.text = "collecting vertex information..."
					ProgressStart "Collecting Vertex information..."
					NewVertPos = #()
					if heapfree < 12000000 do
						(
							format "heapFree = %\n Adding 16 MB\n" heapFree
							heapsize += 16000000
						)
					for i = 1 to nv do
						(
							if mod i 100 == 0 do
								progressUpdate ((i / nv as float) * 100)
							append NewVertPos (getvert terrain i)
						)
					ProgressEnd()
					progscale = 100.0 / nv
					status1.text = "Relaxing..."
					ProgressStart "Relaxing..."
					for i = 1 to nv do
						(
							if (finditem SelVertIndexlist i) != 0 then
								setvert terrain i [(InitVertPos[i]).x, (InitVertPos[i]).y, (NewVertPos[i]).z]
							else
								setvert terrain i [(InitVertPos[i]).x, (InitVertPos[i]).y, (InitVertPos[i]).z]
							if ProgressUpdate (i * progscale) == false then exit
						)
					update terrain
					ProgressEnd()
					-- gc()
					MessageBox "Done Relaxing!"
					status1.text = ""
				)
		)
)