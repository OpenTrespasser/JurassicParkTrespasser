-- draws a small point into a bitmap
fn drawPoint bmap p c = 
	(
		setPixels bmap [p.x, (p.y + 2)] #(c)
		setPixels bmap [p.x, (p.y + 1)] #(c)
		setPixels bmap [(p.x - 2),p.y] #(c,c,c,c,c)
		setPixels bmap [p.x, (p.y - 1)] #(c)
		setPixels bmap [p.x, (p.y - 2)] #(c)
	)

-- draws a big point into a bitmap
fn drawBigPoint bmap p c = 
	(
		setPixels bmap [p.x - 1, (p.y + 3)] #(c,c,c)
		setPixels bmap [p.x - 1, (p.y + 2)] #(c,c,c)
		setPixels bmap [(p.x - 3),p.y + 1] #(c,c,c,c,c,c,c)
		setPixels bmap [(p.x - 3),p.y] #(c,c,c,c,c,c,c)
		setPixels bmap [(p.x - 3),p.y - 1] #(c,c,c,c,c,c,c)
		setPixels bmap [p.x - 1, (p.y - 2)] #(c,c,c)
		setPixels bmap [p.x - 1, (p.y - 3)] #(c,c,c)
	)

	-- We need the BLine fn in this script. I've embedded it
	-- for completeness' sake
	--
	-- Bresenham line implimentation for MAXScript
	-- by Harry Denholm, Kinetix 1998
	function BLine bmp x1 y1 x2 y2 c =
	(
	-- stick the colour into an array
	local ary = #(c)
	
	-- assign originals
	local Xb = x1 as integer
	local Yb = y1 as integer
	
	-- build the line deltas
	local dX = x2-x1 as float
	local dY = y2-y1 as float
	
	-- straight horiz 
	if dy == 0.0f do (
		local xsign = 1
		if xb > x2 then xsign = 1 as integer
		if x2 < xb then xsign = -1 as integer
		
				setPixels bmp [xb,yb] ary
				while xb != x2 do
					(
						xb += xsign 
						setPixels bmp [xb,yb] ary
					)
					return true
	)
	-- straight vertical 
	if dx == 0.0f do (
		local ysign = 1
		if yb > y2 then ysign = 1 as integer
		if y2 < yb then ysign = -1 as integer
		
				setPixels bmp [xb,yb] ary
				while yb != y2 do
					(
						yb += ysign 
						setPixels bmp [xb,yb] ary
					)
					return true
	
	)
	
	-- no straights, go for bresenham line slide
	
	-- set up the movements
		local xsign = 1
		if xb > x2 then xsign = 1 as integer
		if x2 < xb then xsign = -1 as integer
		local ysign = 1
		if yb > y2 then ysign = 1 as integer
		if y2 < yb then ysign = -1 as integer
		dx = abs(dx)
		dy = abs(dy)
		setPixels bmp [xb,yb] ary
		
	-- line more vertical than horizontal
		if dx < dy then
		(
			p = 2 * dx - dy
			const1 = 2 * dx
			const2 = 2 * (dx - dy)
			while yb != y2 do
			(
				yb += ysign
				if p < 0 then ( p = p + const1 )
				else
				(	
					p += const2
					xb += xsign
				)
				setPixels bmp [xb,yb] ary
			)
		)
		
	--	line more horizontal than vertical
		else
		(
			p = 2 * dy - dx
			const2 = 2 * (dy - dx)
			const1 = 2 * dy
			while xb != x2 do
			(
				xb = xb + xsign
				if  p < 0 then ( p = p + const1 )
				else
				(	p = p + const2
					yb = yb + ysign
				)
				setPixels bmp [xb,yb] ary
			)
		)
	
	)

-- UVW Unwrapper 
-- MAX Script example by Harry Denholm, Kinetix 98
-- Developer Consulting Group


utility unwrapper "UVW Unwrap"
(
	local Uobj,uvBMP

	button aboutBtn "About" height:15 width:35
	group "Image Options"
		(
			spinner szw "Width: " range:[1,1000,256] type:#integer fieldWidth:40
			spinner szh "Height: " range:[1,1000,256] type:#integer fieldWidth:40
			label h4 height:2
		
			checkbox DrawFaces "Draw Edges" checked:true
			colorpicker cl color:(color 128 128 128)
			checkbox DrawFaceIDs "Draw Mat IDs" checked:false
			spinner MatIDs "Mat ID:" range:[0,100,1] type:#integer
			colorpicker matcl color:(color 200 200 200)
			checkbox selFaces "Draw Selected Faces"
			colorpicker scl color:(color 0 0 255)
			checkbox selVerts "Draw Selected Verts"
			colorpicker vc color:(color 255 255 0)
		)
	
	pickbutton unwrap "UVW Unwrap" width:120
	
	
	
	on aboutBtn pressed do
		(
			Messagebox "\"UVW Unwrap\"\n\nwritten by Harry Denholm\nmodified by Kyle McKisic"
		)

	on unwrap picked UObj do
		(
			-- Do we have Tverts available?
			if (getnumtverts Uobj) == 0 then 
				(
					MessageBox("Object Not Mapped!")
				) else (
			
					-- setup some image stuff
					uvBMP = bitmap szw.value szh.value
					iW = szw.value as float
					iH = szh.value as float
			
					-- start a progress bar & calc a progress delta
					ProgressStart "Unwrapping..."
					delta = (100.0/(getnumfaces Uobj)) as float
		
		
					if DrawFaces.checked do
						(
							cCol = cl.color
							for t=1 to (getnumfaces Uobj) do
								(
									-- update our progress bar
									progressUpdate ((t*delta) as integer)
									
									-- get the faces
									faceN = getface Uobj t
									faceT = getTVFace Uobj t
									
									-- take the TVs out of the face index
									tva = gettvert Uobj faceT.x
									tvb = gettvert Uobj faceT.y
									tvc = gettvert Uobj faceT.z
									
									-- unwrap TVs
									if (getedgevis Uobj t 1) then \
										Bline uvBMP (iW*tva.x) (iH*(1.0-tva.y)) (iW*tvb.x) (iH*(1.0-tvb.y)) cCol
									if (getedgevis Uobj t 2) then \
										Bline uvBMP (iW*tvb.x) (iH*(1.0-tvb.y)) (iW*tvc.x) (iH*(1.0-tvc.y)) cCol
									if (getedgevis Uobj t 3) then \
										Bline uvBMP (iW*tvc.x) (iH*(1.0-tvc.y)) (iW*tva.x) (iH*(1.0-tva.y)) cCol
								)
						)
		
					if DrawFaceIDs.checked do
						(
							cCol = scl.color
							sel = #()
							for i = 1 to UObj.numfaces do
								(
									if (getFaceMatID UObj i) == MatIDs.value do
										append sel i
								)
							if sel.count > 0 do
								for t=1 to sel.count do
									(
										-- update our progress bar
										progressUpdate ((t*delta) as integer)
										
										-- get the faces
										faceN = getface Uobj sel[t]
										faceT = getTVFace Uobj sel[t]
										
										-- take the TVs out of the face index
										tva = gettvert Uobj faceT.x
										tvb = gettvert Uobj faceT.y
										tvc = gettvert Uobj faceT.z
										
										-- unwrap TVs
										if (getedgevis Uobj t 1) then \
											Bline uvBMP (iW*tva.x) (iH*(1.0-tva.y)) (iW*tvb.x) (iH*(1.0-tvb.y)) cCol
										if (getedgevis Uobj t 2) then \
											Bline uvBMP (iW*tvb.x) (iH*(1.0-tvb.y)) (iW*tvc.x) (iH*(1.0-tvc.y)) cCol
										if (getedgevis Uobj t 3) then \
											Bline uvBMP (iW*tvc.x) (iH*(1.0-tvc.y)) (iW*tva.x) (iH*(1.0-tva.y)) cCol
									)

						)
					if selFaces.checked do
						(
							cCol = scl.color
							sel = getFaceSelection UObj
							if sel.count > 0 do
								for t=1 to sel.count do
									(
										-- update our progress bar
										progressUpdate ((t*delta) as integer)
										
										-- get the faces
										faceN = getface Uobj sel[t]
										faceT = getTVFace Uobj sel[t]
										
										-- take the TVs out of the face index
										tva = gettvert Uobj faceT.x
										tvb = gettvert Uobj faceT.y
										tvc = gettvert Uobj faceT.z
										
										-- unwrap TVs
										if (getedgevis Uobj t 1) then \
											Bline uvBMP (iW*tva.x) (iH*(1.0-tva.y)) (iW*tvb.x) (iH*(1.0-tvb.y)) cCol
										if (getedgevis Uobj t 2) then \
											Bline uvBMP (iW*tvb.x) (iH*(1.0-tvb.y)) (iW*tvc.x) (iH*(1.0-tvc.y)) cCol
										if (getedgevis Uobj t 3) then \
											Bline uvBMP (iW*tvc.x) (iH*(1.0-tvc.y)) (iW*tva.x) (iH*(1.0-tva.y)) cCol
									)
						)
		
					if selVerts.checked do
						(
							cCol = vc.color
							sel = getVertSelection UObj
							if sel.count > 0 do
								for t=1 to sel.count do
									(
										-- update our progress bar
										progressUpdate ((t*delta) as integer)
										
										iVert = sel[t]
										-- get the faces used by the vertex selection
										uvPoints = #()
										for i = 1 to UObj.numfaces do
											(
												iFace = getFace UObj i
												iTVFace = getTVFace UObj i
												if iFace.x == iVert do (append uvPoints iTVFace.x)
												if iFace.y == iVert do (append uvPoints iTVFace.y)
												if iFace.z == iVert do (append uvPoints iTVFace.z)
											)
										uvs = for i = 1 to uvPoints.count collect (GetTVert UObj uvPoints[i])
										for i = 1 to uvPoints.count do
											(
												tv = uvs[i]
												drawPoint uvBMP [(iW * tv.x), (iH * (1.0 - tv.y))] cCol
											)
									)
						)
		
					display uvBMP
					progressEnd()
			)
		)
)