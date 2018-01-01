-- *********************************
fn Thresh a b t =
	if (distance a  b) < t then
		return true
	else
		return false

-- *********************************
fn rndP3 p3 n = 
	(
		local mult = 10.0 ^ n
		nx = (floor ((p3.x * mult) + 0.5)) / mult
		ny = (floor ((p3.y * mult) + 0.5)) / mult
		nz = (floor ((p3.z * mult) + 0.5)) / mult
		return [nx,ny,nz]
	)

-- *********************************
fn copyArray a = 
	(
		b = #()
		for i = 1 to a.count do
			b[i] = a[i]
		return b
	)
 
-- *********************************
fn InitializeSelection = 
	(
		freeSceneBitmaps()
		SingleObject = false
		AttachedTrunk = false
		MagicIndex = undefined
		ObjArray = selection as array
		if ObjArray.count == 1 or ObjArray.count == 2 then
			(
				if ObjArray.count == 1 then
					(
						CTrunk = ObjArray[1]
						CBranchec = undefined
					) else ( 
						if ObjArray[1].pivot.z < ObjArray[2].pivot.z then
							(
								CTrunk = ObjArray[1]
								CBranches = ObjArray[2]
							) else (
								CTrunk = ObjArray[2]
								CBranches = ObjArray[1]
							)
					)
				for i = 1 to CTrunk.name.count do
					if CTrunk.name[i] == "~" do MagicIndex = i
				-- find the area from the name of the trunk object
				Area = ""
				if MagicIndex != undefined then
					Area = substring CTrunk.name (Magicindex + 1) 2
				else
					MessageBox "Please make sure you have\nthe area information defined!\n\ni.e. \"VMyPlant~IJ10-00\""
				return #(CTrunk, CBranches, Area)
			) else (
				MessageBox "Feed this tool 1 or 2 objects only"
				return undefined
			)
	)
					
-- *********************************
fn projectPoints p3 p r =
	(
		-- usage: projectPoints <point in 3space the plane should intersect> <array of point3 values> <ray>
		-- project the key vertices onto a plane perpendicular to the light
		pp = #()
		V = p3
		N = r.dir
		d = -(dot V N)
		for i = 1 to p.count do
			(
				V = p[i]
				rPos = V + r.dir * 10.0
				NRay = ray rPos r.dir
				t = -((d + (dot N rPos)) / (dot N r.dir))
				P0 = rPos.x + r.dir.x * t
				P1 = rPos.y + r.dir.y * t
				P2 = rPos.z + r.dir.z * t
				Pnt = [P0, P1, P2]
				append pp Pnt
			)
		return pp
	)

-- *********************************
fn projectObject o r = 
	(
		-- projects all the vertices in an object onto a plane normal to the direction of a supplied ray.
		-- usage: projectObject <object> <ray>
		v = for i = 1 to o.numverts collect (getVert o i)
		v2 = projectPoints o.center v r
		for i = 1 to o.numverts do (setVert o i v2[i])
		update o
	)

-- *********************************
fn FlattenObject o r = 
	(
		sel = (getBackFaces o r)
		for i = sel.count to 1 by -1 do
			deleteFace o sel[i]
		projectObject o r		
	)

-- *********************************
fn getBackFaces o r =
	(
		s = #()
		for i = 1 to o.numfaces do
			if (dot (getFaceNormal o i) r.dir) < 0 do
				append s i
		return s	
	)

-- *********************************
fn CopyAndFlipFaces o = 
	(
		sel = getFaceSelection o
		sc = sel.count
		fc = o.numfaces
		setNumFaces o (o.numfaces + sc) true
		for i = 1 to sc do
			(
				iFaceIndex = sel[i]
				iFace = getFace o iFaceIndex
				setFace o (fc + i) [iFace.x, iFace.z, iFace.y]
				setFaceMatID o (fc + i) (getFaceMatID o sel[i])
				setFaceSmoothGroup o (fc + i) (getFaceSmoothGroup o sel[i])
			)
		update o
		for i = 1 to sc do
			(
				iTVFace = getTVFace o sel[i]
				setTVFace o (fc + i) [iTVFace.x, iTVFace.z, iTVFace.y]
			)
		addModifier o (Smooth())
		convertToMesh o
	)

-- *********************************
fn QuickSort l r a = 
	(
		if r > l do
			(
				i = l - 1
				j = r
				do (
					do (i += 1) while a[i] < a[r]
					while j > 1 do
						(
							j -= 1
							if a[j] <= a[r] do Exit
						)
					t = a[i]
					a[i] = a[j]
					a[j] = t
				) while (
					i < j
				)
				a[j] = a[i]
				a[i] = a[r]
				a[r] = t
				QuickSort l (i - 1) a
				QuickSort (i + 1) r a
			)
		return a
	)

-- *********************************
fn QuickSort2 l r a b =
	(
		if r > l do
			(
				i = l - 1
				j = r
				do (
					do (i += 1) while a[i] < a[r]
					while j > 1 do
						(
							j -= 1   
							if a[j] <= a[r] do Exit
						)
					t = a[i]
					a[i] = a[j]
					a[j] = t

					t2 = b[i]
					b[i] = b[j]
					b[j] = t2

				) while (
					i < j
				)
				a[j] = a[i]
				a[i] = a[r]
				a[r] = t

				b[j] = b[i]
				b[i] = b[r]
				b[r] = t2

				QuickSort2 l (i - 1) a b
				QuickSort2 (i + 1) r a b
			)
		return #(a,b)
	)

-- *********************************
fn GetAngle p1 p2 = 
	(
		-- tan(theta) = (p2.x - p1.x)
		dx = (p2.x - p1.x) as float	
		dy = (p2.y - p1.y) as float
		theta = atan (dx / dy)
		return theta
	)

-- *********************************
fn ccw p0 p1 p2 = 
	(
		dx1 = p1.x - p0.x
		dy1 = p1.y - p0.y
		dx2 = p2.x - p0.x
		dy2 = p2.y - p0.y

		if (dx1 * dy2) > (dy1 * dx2) do return 1 -- left turn
		if (dx1 * dy2) < (dy1 * dx2) do return -1 -- right turn
		if (dx1 * dy2) == (dy1 * dx2) do
			if (dx1 * dx2) < 0 or (dy1 * dy2) < 0 then
				return -1
			else
				if ((dx1 * dx1) + (dy1 * dy1)) >= ((dx2 * dx2) + (dy2 * dy2)) then
					return 0
				else
					return 1
	)

-- *********************************
fn IdentifyEdges b i = 
	(
		local p = #()
		iHeight = b.height
		iWidth = b.width

		-- make sure the top point is in the array
		for y = 0 to (iHeight - 1) by 1 do
			(
				CRow = getPixels b [0,y] iWidth
				for x = 1 to iWidth do
					if CRow[x].value >= 253 do (append p [x - 1,y]; Exit)
				if p.count >= 1 do Exit
			)

		-- make sure the bottom point is in the array
		for y = (iHeight - 1) to 0 by -1 do
			(
				CRow = getPixels b [0,y] iWidth
				for x = 1 to iWidth do
					if CRow[x].value >= 253 do (append p [x - 1,y]; Exit)
				if p.count >= 2 do Exit
			)

		for y = 0 to (iHeight - 1) by i do
			(
				CRow = getPixels b [0,y] iWidth
				for x = 1 to iWidth do
					if CRow[x].value >= 253 do (append p [x - 1,y]; Exit)
				for x = iWidth to 1 by -1 do
					if CRow[x].value >= 253 do (append p [x - 1,y]; Exit)
			)
		return p
	)

fn ArrayCompare foo a b =
	(
		same = true
		for i = 1 to a.count do
			if a[i] != b[i] do same = false
		return same
	)

-- *********************************
fn GetElement p M = 
	(
		if M <= 0 do
			M += p.count

		if M > p.count do
			M -= p.count
		return P[M]
	)

-- *********************************
fn rnd num = (num + 0.5) as integer

-- *********************************
fn AvgPoint2 p1 p2 = [rnd ((p1.x + p2.x) as float / 2), rnd ((p1.y + p2.y) as float / 2)]

-- *********************************
fn pruneClosePoints p threshold = 
	(
		n = #()
		local clean = true
		local skip = false
		for i = 1 to p.count do
			(
				if skip then
					skip = false
				else
					(
						-- get the distance between the two points
						d = distance (p1 = (getElement p i)) (p2 = (getElement p (i+1)))
						--format "d: %\n" d
						if d <= threshold then
							(
								append n (AvgPoint2 p1 p2)
								skip = true
								clean = false
							)
						else append n p1
					)
			)
		if (ArrayCompare true p n) do return p
		if clean == false then
			(
				--format "going again...\n"
				pruneClosePoints n threshold
			) else (
				return n
			)
	)

-- *********************************
fn GrahamScanBitmap CBitmap Yi PruneDistance= 
	(
		CBitmap = (BitmapThreshold CBitmap 180)
		p = IdentifyEdges CBitMap Yi
		Hullpoints = GrahamScan p
		HullPoints = pruneClosePoints HullPoints PruneDistance
		return HullPoints
	)

-- *********************************
fn GrahamScan p = 
	(
		N = p.count
		lowPt = p[N]
		AnchorIndex = N
		local HullPoints = #(), AngleArray = #()
		for i = 1 to N do
			if (p[i].y < lowPt.y) then
				(
					lowPt = p[i]
					AnchorIndex = i
				)
			else
				if (p[i].y == lowPt.y) then
					if (p[i].x < lowPt.x) do
						(
							lowPt = p[i]
							AnchorIndex = i
						)
		-- Calculate the angles from the anchorpoint to each other point
		AnchorPt = p[AnchorIndex]
		append HullPoints AnchorPt
		for i = 1 to N do
			if i != AnchorIndex then
				append AngleArray (getAngle AnchorPt p[i])
			else
				append AngleArray -360.0
		-- sort the angles
		quickSort2 1 AngleArray.count AngleArray p
		
		p[1] = p[N]
		M = 3
		for i = 4 to N do
			(
				while (ccw (GetElement p M) (GetElement p (M - 1)) p[i]) <= 0 do (M -= 1)
				M += 1
				t = p[M]
				p[M] = p[i]
				p[i] = t
			)
		for i = 2 to M do append Hullpoints p[i]
		gc()
		return HullPoints
	)

-- *********************************
fn scaleColor c s = (color (c.r * s) (c.g * s) (c.b * s))

fn scaleColor2 c s = 
	(
		r = (c.r * s)
		g = (c.g * s)
		b = (c.b * s)
		if r > 255 do r = 255
		if r < 0 do r = 0
		if g > 255 do g = 255
		if g < 0 do g = 0
		if b > 255 do b = 255
		if b < 0 do b = 0
		return (color r g b)
	)

-- *********************************
fn CreateBitmap w h c = 
	(
		b = bitmap w h
		local cArray = for i = 1 to w collect c
		for y = 0 to (h - 1) do (SetPixels b [0, y] cArray)
		return b
	)

-- *********************************
fn PowOfLimit x y = 
	(
		if x > y then
			(
				SclDn = 0
				while x > 256 do	
					(
						x *= 0.5
						SclDn += 1
					)
				if SClDn > 0 do
					for i = 1 to SClDn do y *= 0.5
			) else (
				SclDn = 0
				while y > 256 do	
					(
						y *= 0.5
						SclDn += 1
					)
				if SClDn > 0 do
					for i = 1 to SClDn do x *= 0.5
			)
		if x < 8 do x = 8
		if y < 8 do y = 8
		return [x,y]
	)

-- *********************************
-- returns a power of two number based on user input of higher, lower, or nearest
fn NearPowOfTwo num weight = 
	(
		-- Highest
		if weight == 1 do
			(
				num2 = num
				counter = 0
				while num >= 1.0 do
					(
						num = (num * 0.5)
						counter = counter + 1
					)
				p2 = 2^(counter)
				return p2
			)

		-- Lowest
		if weight == 2 do
			(
				num2 = num
				counter = 0
				while num >= 1.0 do
					(
						num = (num * 0.5)
						counter = counter + 1
					)
				p2 = 2^(counter - 1)
				return p2	
			)

		-- Nearest
		if weight == 3 do
			(
				num2 = num
				counter = 0
				while num >= 1.0 do
					(
						num = (num * 0.5)
						counter = counter + 1
					)
				p2 = 2^(counter - 1)
				if (num2 > p2 * 1.5) then
					return (p2 * 2)
				else 
					return p2	
			)
	)

-- *********************************
-- returns the index of the vertex with the lowest pos.z value
fn LowestVert obj =
	(
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

-- *********************************
fn trunkCenter CObj thresh = 
	(
		-- first, we find the lowest vertex in the model
		LowVert = lowestVert CObj
		-- searching from the bottom
		BottomPoint = CObj.min.z
		SetVertSelection CObj #()
		update CObj
		nv = CObj.numverts
		Verts = for i = 1 to nv collect (getvert CObj i)
		-- first we find the vertex distance furthest from the center of the object
		FarVert = 0.0
		VertexDistances = #()
		-- we put all the vertex distances into an array, and find the one furthest from the center of the mesh
		for i = 1 to nv do
			(
				append VertexDistances Verts[i].z
				if VertexDistances[i] < FarVert do FarVert = VertexDistances[i]
			)
		-- now we flag any vert within the threshold and put it  into an array
		VertSelection = #()
		for i = 1 to nv do
				if VertexDistances[i] <= (BottomPoint + thresh) do (append VertSelection i)
		SetVertSelection CObj VertSelection
		verts = [0,0,0]
		for i = 1 to VertSelection.count do
			Verts = Verts + (getVert CObj VertSelection[i])
		
		VertCenter = [(Verts.x / VertSelection.count), (Verts.y / VertSelection.count), (Verts.z / VertSelection.count)]
		return VertCenter
	)

-- *********************************
-- crops a bitmap to new supplied dimensions
fn CropBitmap SourceImage StartPos NewImageSize = 
	(
		width = SourceImage.width
		height = SourceImage.height
		bmap = BitMap NewImageSize.x NewImageSize.y
		NewY = 0
		for y = StartPos.y to (StartPos.y + NewImageSize.y) do
			(
				CRow = getPixels SourceImage [StartPos.x, y] NewImageSize.x
				setPixels bmap [0,NewY] CRow
				NewY += 1
			)
		return bmap
	)

-- *********************************
-- takes a bitmap and returns an array of the following:
-- [1] a new bitmap that only contains black or white (on/off transparencey)
-- [2] the start point [x,y] for cropping
-- [3] the area of a closest fit rectangle about the opaque area of the opacity map from the start point at [2]
fn ThresholdBitmap bmap Thresh = 
	(
		local NMap = copy BMap
		StartPoint = [0,0]
		NewSize = [NMap.width, NMap.height]
		width = NMap.width
		height = NMap.height
		AreaArray = #()
		for y = 1 to height do
			(
				AreaArray[y] = #()
				CRow = getpixels NMap [0, (y - 1)] width
				-- convert the row of color pixels to values
				LumRow = for i = 1 to CRow.count collect (CRow[i].value)
				-- convert this row based on the threshold value
				for i = 1 to LumRow.count do
					if LumRow[i] > Thresh then
						(
							AreaArray[y][i] = true
							LumRow[i] = 255
						) else (
							AreaArray[y][i] = false
							LumRow[i] = 0
						)
				-- convert the values back into colors
				for i = 1 to LumRow.count do
					CRow[i] = (color LumRow[i] LumRow[i] LumRow[i])
				-- write the row back into the bitmap
				setpixels NMap [0, (y - 1)] CRow
			)

		maxX = 0
		minX = 256
		maxY = 0
		minY = 256

		for y = 1 to AreaArray.count do
			(
				CRow = AreaArray[y]
				for x = 1 to CRow.count do
					if CRow[x] == true do
						(
							if x > maxX do maxX = x
							if y > maxY do maxY = y
							if x < minX do minX = x
							if y < minY do minY = y
						)
			)
		NewStartPoint = [(minX - 1), (minY - 1)]
		NewMapSize = [(maxX - minX) + 1, (maxY - minY) + 1]
		if NewStartPoint == [255,255] or NewMapSize == [-255,-255] do
			(
				format "Bad OpacityMap data\n"
				NewStartPoint = [0,0]
				NewMapSize = NewSize
			)
		return #(NMap, NewStartPoint, NewMapSize)
	)

-- ************************************************************************************************
-- * Start Utility
-- ************************************************************************************************
Utility DetailReduction "Detail Reduction"
	(
		local DRRollout, WorkingFolder, AttachedTrunk, SingleObject, Flt
		
		local debug = false

		rollout DRRollout "Detail  Reduction"
			(
				local CObj, CTrunk, CBranches, ThinkCount = 0,
							InterfaceState = #(),
							MasterObjData = #(),
							SunArray = #([0.163287,-0.71436,0.680461], [0.150228,-0.623607,0.767167], [0.13906,-0.522188,0.841417], [0.129977,-0.411837,0.90194], [0.123134,-0.294445,0.947703], [0.118652,-0.17202,0.977922], [0.116608,-0.0466583,0.992081], [0.11704,0.0794943,0.989941], [0.11994,0.204279,0.971537], [0.125262,0.32556,0.937187], [0.132916,0.441262,0.88748])

				local DefaultInterface = #(false,\ --renderSeperate
							false,\ --CenterToBase,
							0.5,\ -- TrunkCenterThresh
							127,\ -- OpacityThresh
							true,\ -- RescaleBitmaps
							1,\ -- sizeMaps
							32,\ -- PixelsPerMeter
							4,\ -- YIncrement
							10.0,\ -- PrunePointsThreshold
							0.24,\ -- AngleJitter
							true,\ -- GenTrunk
							false,\ -- CameraAtEyeHeight
							false,\ -- isPrelit
							256,\ -- BitMapSize
							5,\ -- NumViews
							1,\ -- DRLevel
							false,\ -- OverRideBitmapSize
							256,\ -- TrunkWidthOveride
							256,\ -- TrunkHeightOveride
							true,\ -- GenBranches
							true,\ -- BranchCameraAtEyeHeight
							true,\ -- SplitMethod
							256,\ -- BranchBitMapSize
							5,\ -- BranchNumViews
							1,\ -- BranchDRLevel
							false,\ -- BranchOverRideBitmapSize
							256,\ -- BranchWidthOveride
							256,\ -- BranchHeightOveride
							true) -- AutoDetermineMapSizes

				group "Batch Options"
					(
						ListBox BatchList "Batch Processing List:" height:5 width:250
						editText BatchName "Batch Name:" text:"Detail Reduction Set 1" width:250
						checkbutton EditName "Edit" offset:[130,-23] height:19 highlightcolor:(color 69 163 172) width:80
						button Add "Add Selection" width:80 height:32 offset:[130,-99]
						button Remove "Remove" width:80 height:32 offset:[130,0]
						label blank0 offset:[0,5]
						checkbox UseObjName "Use Obj Name" checked:true
						checkbox AutoSelect "Auto Select" offset:[130,-20]
						checkbox ThinkForMe "Think for Me" align:#right offset:[0,-20]
					)

				group "Global Parameters"
					(
						dropDownList GameArea "Game Area Lighting:" items:#("Beach", "Jungle Road", "Platation House", "Industrial Jungle", "InGEN Town", "Plains", "PineValley", "Shore", "InGEN Lab", "Ascent", "Summit") selection:2 height:12
						colorpicker AmbientCol "Ambient Light: " color:(color 90 90 90) offset:[-50,0]
						spinner CMult "Spotlight Multiplier: " range:[0.1, 5.0, 0.5] fieldwidth:35 offset:[-20,-23]
					)

				group "General Options"
					(
						checkBox renderSeperate "Detail reduce branches seperate from trunk"
						checkbox CenterToBase "Center camera based on trunk bottom" checked:false
						spinner TrunkCenterThresh "Threshold:" range:[0,10,0.5] fieldWidth:45 align:#right enabled:false offset:[0,-20]
						spinner OpacityThresh "Opacity Map Threshold: " range:[0,255,127] type:#integer fieldwidth:45 offset:[0,0]
						Checkbox RescaleBitmaps "Rescale bitmaps" offset:[0,-20] checked:true
						radiobuttons sizeMaps labels:#("Higher power of 2", "Lower power of 2", "Nearest power of 2") default:1 columns:3 align:#left
						spinner PixelsPerMeter "Pixels Per Meter: " range:[1,64,32] fieldWidth:45
						checkbox AutoDetermineMapSizes "Auto Determine Map Sizes" offset:[0,-20] checked:true
					)

				group "Convex Hull Parameters"
					(
						spinner YIncrement "Y Increment" range:[1,32,4] type:#integer fieldWidth:35 align:#left
						spinner PrunePointsThreshold "Pixel Threshold:" range:[1,32,10] fieldWidth:35 offset:[0,-20] align:#center
						spinner AngleJitter "Pixel Jitter:" range:[0,1,0.24] fieldWidth:35 align:#right offset:[0,-20]
					)

				group "Trunk Options"
					(
						checkbox GenTrunk "Create" checked:true
						checkbox CameraAtEyeHeight "Camera at eye level" align:#center offset:[0,-20]
						checkbox isPrelit "Trunk is Prelit" align:#right offset:[0,-20]
						spinner BitMapSize "Map Size:" range:[8,512,256] type:#integer align:#left fieldwidth:35
						spinner NumViews "# Views: " range:[2,9,5] type:#integer fieldwidth:35 offset:[0,-21] align:#center
						spinner DRLevel "Detail Level: " range:[1,4,1] type:#integer fieldwidth:25 offset:[0,-21] align:#right
						checkbox OverRideBitmapSize "Overide Bitmap Size"
						spinner TrunkWidthOveride "Width:" range:[8,256,256] type:#integer fieldwidth:45 align:#left enabled:false offset:[130,-20]
						spinner TrunkHeightOveride "Height:" range:[8,256,256] type:#integer fieldwidth:45 offset:[230,-21] enabled:false align:#left
					)

				group "Branch Options"
					(
						checkbox GenBranches "Create" enabled:false checked:true
						checkbox BranchCameraAtEyeHeight "Camera at eye level" align:#center offset:[0,-20] enabled:false checked:true
						checkbox SplitMethod "Use Split Method" checked:true align:#right offset:[0,-20] enabled:false
						spinner BranchBitMapSize "Map Size:" range:[8,512,256] type:#integer align:#left fieldwidth:35 enabled:false
						spinner BranchNumViews "# Views: " range:[2,9,5] type:#integer fieldwidth:35 align:#center offset:[0,-21] enabled:false
						spinner BranchDRLevel "Detail Level: " range:[1,4,1] type:#integer fieldwidth:25 offset:[0,-21] align:#right enabled:false
						checkbox BranchOverRideBitmapSize "Overide Bitmap Size" enabled:false
						spinner BranchWidthOveride "Width:" range:[8,256,256] type:#integer fieldwidth:45 align:#left enabled:false offset:[130,-20]
						spinner BranchHeightOveride "Height:" range:[8,256,256] type:#integer fieldwidth:45 offset:[230,-21] enabled:false align:#left
					)

				Button CreateDRObject "Create Detail Reduced Version" height:40 align:#left offset:[-10,10] width:180
				Button CreateShadowObject "Create Shadow Version" width:180 height:40 align:#right offset:[10,-46]
				label status1

				on BatchName entered text do
					(
						if EditName.checked do
							(
								txt = BatchName.text
								names = BatchList.items
								s = BatchList.selection
								names[s] = txt
								BatchList.items = names
							)
					)

				on EditName changed state do
					(
						if state then
							(
								names = BatchList.items
								if names.count > 0 do
									(
										s = BatchList.selection
										BatchName.text = names[s]
									)
							) else (
								names = BatchList.items
								BatchName.text = ("Detail Reduction Set " + (names.count + 1) as string)
							)
					
					)
				on ThinkForMe changed state do
					(
						ThinkCount += 1
						if ThinkCount >= 0 and ThinkCount <= 3 do messageBox "If computers were so wise...."
						if ThinkCount > 3 and ThinkCount < 7 do messageBox "pesky little fucker...."
						if ThinkCount >= 7 and ThinkCount < 10 do messageBox "just give up!"
						if ThinkCount >= 10 do messageBox "keep it up.....\nhere comes something really cool"
						ThinkForMe.checked = false
					)
				on BatchList selected s do
					(
						renderSeperate.checked = InterfaceState[s][1]
						CenterToBase.checked = InterfaceState[s][2]
						TrunkCenterThresh.value = InterfaceState[s][3]
						OpacityThresh.value = InterfaceState[s][4]
						RescaleBitmaps.checked = InterfaceState[s][5]
						sizeMaps.state = InterfaceState[s][6]
						PixelsPerMeter.value = InterfaceState[s][7]
						YIncrement.value = InterfaceState[s][8]
						PrunePointsThreshold.value = InterfaceState[s][9]
						AngleJitter.value = InterfaceState[s][10]
						GenTrunk.checked = InterfaceState[s][11]
						CameraAtEyeHeight.checked = InterfaceState[s][12]
						isPrelit.checked = InterfaceState[s][13]
						BitMapSize.value = InterfaceState[s][14]
						NumViews.value = InterfaceState[s][15]
						DRLevel.value = InterfaceState[s][16]
						OverRideBitmapSize.checked = InterfaceState[s][17]
						TrunkWidthOveride.value = InterfaceState[s][18]
						TrunkHeightOveride.value = InterfaceState[s][19]
						GenBranches.checked = InterfaceState[s][20]
						BranchCameraAtEyeHeight.checked = InterfaceState[s][21]
						SplitMethod.checked = InterfaceState[s][22]
						BranchBitMapSize.value = InterfaceState[s][23]
						BranchNumViews.value = InterfaceState[s][24]
						BranchDRLevel.value = InterfaceState[s][25]
						BranchOverRideBitmapSize.checked = InterfaceState[s][26]
						BranchWidthOveride.value = InterfaceState[s][27]
						BranchHeightOveride.value = InterfaceState[s][28]
						AutoDetermineMapSizes.checked = InterfaceState[s][29]

						if CenterToBase.checked then
							TrunkCenterThresh.enabled = true
						else
							TrunkCenterThresh.enabled = false

						if renderSeperate.checked then
							SplitMethod.enabled = BranchBitMapSize.enabled = BranchNumViews.enabled = BranchDRLevel.enabled = GenBranches.enabled = BranchCameraAtEyeHeight.enabled = BranchOverRideBitmapSize.enabled = true
						else
							SplitMethod.enabled = BranchBitMapSize.enabled = BranchNumViews.enabled = BranchDRLevel.enabled = GenBranches.enabled = BranchCameraAtEyeHeight.enabled = BranchOverRideBitmapSize.enabled = BranchWidthOveride.enabled = BranchHeightOveride.enabled = false

						if OverRideBitmapSize.checked then
							TrunkWidthOveride.enabled = TrunkHeightOveride .enabled = true
						else					
							TrunkWidthOveride.enabled = TrunkHeightOveride .enabled = false

						if BranchOverRideBitmapSize.checked then
							BranchWidthOveride.enabled = BranchHeightOveride .enabled = true
						else					
							BranchWidthOveride.enabled = BranchHeightOveride .enabled = false

						if AutoSelect.checked do
							(
								select MasterObjData[s][1]
								if MasterObjData[s][2] != undefined do selectmore MasterObjData[s][2]
							)

						if EditName.checked do
							(
								names = BatchList.items
								if names.count > 0 do
									(
										s = BatchList.selection
										BatchName.text = names[s]
									)
							)
					)
				
				on add pressed do
					(
						if (ObjData = InitializeSelection()) != undefined do
							(
								format "%\n" ObjData
								append MasterObjData ObjData
								append InterfaceState (copyArray DefaultInterface)
								BatchListNames = BatchList.items
								if UseObjName.checked then
									(
										nm = ObjData[1].name
										nm = (substring nm 1 (nm.count - 3))
										append BatchListNames nm
									) else (
										append BatchListNames BatchName.text
									)
								BatchList.items = BatchListNames
								BatchName.text = ("Detail Reduction Set " + (BatchListNames.count + 1) as string)
							)
					)

				on remove pressed do
					(
						s = BatchList.selection
						if MasterObjData.count >= 1 do
							(
								deleteItem MasterObjData s
								BatchListNames = BatchList.items
								deleteItem BatchListNames s
								BatchList.items = BatchListNames
								if s >= 2 then
									BatchList.selection = (s - 1)
								else
									BatchList.selection = 1
								BatchName.text = ("Detail Reduction Set " + (BatchListNames.count + 1) as string)
							)
					)

				-- **************************************************************
				-- Interface Control Modifications
				-- **************************************************************

				-- 1
				on renderSeperate changed state do
					(
						if renderSeperate.checked then
							SplitMethod.enabled = BranchBitMapSize.enabled = BranchNumViews.enabled = BranchDRLevel.enabled = GenBranches.enabled = BranchCameraAtEyeHeight.enabled = BranchOverRideBitmapSize.enabled = true
						else
							SplitMethod.enabled = BranchBitMapSize.enabled = BranchNumViews.enabled = BranchDRLevel.enabled = GenBranches.enabled = BranchCameraAtEyeHeight.enabled = BranchOverRideBitmapSize.enabled = BranchWidthOveride.enabled = BranchHeightOveride.enabled = false
						if InterfaceState.count >= 1 do
							InterfaceState[BatchList.selection][1] = renderSeperate.checked
					)

				-- 2
				on CenterToBase changed state do
					(
						if CenterToBase.checked then
							TrunkCenterThresh.enabled = true
						else
							TrunkCenterThresh.enabled = false
						if InterfaceState.count >= 1 do
							InterfaceState[BatchList.selection][2] = CenterToBase.checked
					)

				-- 3
				on TrunkCenterThresh changed state do
					if InterfaceState.count >= 1 do
						InterfaceState[BatchList.selection][3] = TrunkCenterThresh.value
				-- 4
				on OpacityThresh changed state do
					if InterfaceState.count >= 1 do
						InterfaceState[BatchList.selection][4] = OpacityThresh.value
				-- 5
				on RescaleBitmaps changed state do
					if InterfaceState.count >= 1 do
						InterfaceState[BatchList.selection][5] = RescaleBitmaps.checked
				-- 6
				on sizeMaps changed state do
					if InterfaceState.count >= 1 do
						InterfaceState[BatchList.selection][6] = sizeMaps.state
				-- 7
				on PixelsPerMeter changed state do
					if InterfaceState.count >= 1 do
						InterfaceState[BatchList.selection][7] = PixelsPerMeter.value
				-- 8
				on YIncrement changed state do
					if InterfaceState.count >= 1 do
						InterfaceState[BatchList.selection][8] = YIncrement.value
				-- 9
				on PrunePointsThreshold changed state do
					if InterfaceState.count >= 1 do
						InterfaceState[BatchList.selection][9] = PrunePointsThreshold.value
				-- 10
				on AngleJitter changed state do
					if InterfaceState.count >= 1 do
						InterfaceState[BatchList.selection][10] = AngleJitter.value
				-- 11
				on GenTrunk changed state do
					if InterfaceState.count >= 1 do
						InterfaceState[BatchList.selection][11] = GenTrunk.checked
				-- 12
				on CameraAtEyeHeight changed state do
					if InterfaceState.count >= 1 do
						InterfaceState[BatchList.selection][12] = CameraAtEyeHeight.checked
				-- 13					
				on isPrelit changed state do
					if InterfaceState.count >= 1 do
						InterfaceState[BatchList.selection][13] = isPrelit.checked
				-- 14
				on BitMapSize changed state do
					if InterfaceState.count >= 1 do
						InterfaceState[BatchList.selection][14] = BitMapSize.value
				-- 15
				on NumViews changed state do
					if InterfaceState.count >= 1 do
						InterfaceState[BatchList.selection][15] = NumViews.value
				-- 16
				on DRLevel changed state do
					(
						BranchDRLevel.value = DRLevel.value
						if InterfaceState.count >= 1 do
							(
								InterfaceState[BatchList.selection][16] = DRLevel.value
								InterfaceState[BatchList.selection][25] = BranchDRLevel.value
							)
					)
				-- 17
				on OverRideBitmapSize changed state do
					(
						if OverRideBitmapSize.checked then
							TrunkWidthOveride.enabled = TrunkHeightOveride .enabled = true
						else					
							TrunkWidthOveride.enabled = TrunkHeightOveride .enabled = false
						if InterfaceState.count >= 1 do
							InterfaceState[BatchList.selection][17] = OverRideBitmapSize.checked
					)
				-- 18
				on TrunkWidthOveride changed state do
					if InterfaceState.count >= 1 do
						InterfaceState[BatchList.selection][18] = TrunkWidthOveride.value
				-- 19
				on TrunkHeightOveride changed state do
					if InterfaceState.count >= 1 do
						InterfaceState[BatchList.selection][19] = TrunkHeightOveride.value
				-- 20
				on GenBranches changed state do
					if InterfaceState.count >= 1 do
						InterfaceState[BatchList.selection][20] = GenBranches.checked
				-- 21
				on BranchCameraAtEyeHeight changed state do
					if InterfaceState.count >= 1 do
						InterfaceState[BatchList.selection][21] = BranchCameraAtEyeHeight.checked
				-- 22
				on SplitMethod changed state do
					if InterfaceState.count >= 1 do
						InterfaceState[BatchList.selection][22] = SplitMethod.checked
				-- 23
				on BranchBitMapSize changed state do
					if InterfaceState.count >= 1 do
						InterfaceState[BatchList.selection][23] = BranchBitMapSize.value
				-- 24
				on BranchNumViews changed state do
					if InterfaceState.count >= 1 do
						InterfaceState[BatchList.selection][24] = BranchNumViews.value
				-- 25
				on BranchDRLevel changed state do
					(
						DRLevel.value = BranchDRLevel.value
						if InterfaceState.count >= 1 do
							(
								InterfaceState[BatchList.selection][16] = DRLevel.value
								InterfaceState[BatchList.selection][25] = BranchDRLevel.value
							)
					)
				-- 26
				on BranchOverRideBitmapSize changed state do
					(
						if BranchOverRideBitmapSize.checked then
							BranchWidthOveride.enabled = BranchHeightOveride .enabled = true
						else					
							BranchWidthOveride.enabled = BranchHeightOveride .enabled = false
						if InterfaceState.count >= 1 do
							InterfaceState[BatchList.selection][26] = BranchOverRideBitmapSize.checked
					)
				-- 27
				on BranchWidthOveride changed state do
					if InterfaceState.count >= 1 do
						InterfaceState[BatchList.selection][27] = BranchWidthOveride.value
				-- 28
				on BranchHeightOveride changed state do
					if InterfaceState.count >= 1 do
						InterfaceState[BatchList.selection][28] = BranchHeightOveride.value

				on AutoDetermineMapSizes changed state do
					if InterfaceState.count >= 1 do
						 InterfaceState[BatchList.selection][29] = AutoDetermineMapSizes.checked

-- *******************************************************************************************************
-- * Generate Detail Reduced version of the object
-- *******************************************************************************************************
	on CreateDRObject pressed do
		(
			freeSceneBitmaps()
			for s = 1 to InterfaceState.count do
				(
					if debug do format "\n\nStarting Object %\n\n" s
					BatchList.selection = s
					renderSeperate.checked = InterfaceState[s][1]
					CenterToBase.checked = InterfaceState[s][2]
					TrunkCenterThresh.value = InterfaceState[s][3]
					OpacityThresh.value = InterfaceState[s][4]
					RescaleBitmaps.checked = InterfaceState[s][5]
					sizeMaps.state = InterfaceState[s][6]
					PixelsPerMeter.value = InterfaceState[s][7]
					YIncrement.value = InterfaceState[s][8]
					PrunePointsThreshold.value = InterfaceState[s][9]
					AngleJitter.value = InterfaceState[s][10]
					GenTrunk.checked = InterfaceState[s][11]
					CameraAtEyeHeight.checked = InterfaceState[s][12]
					isPrelit.checked = InterfaceState[s][13]
					BitMapSize.value = InterfaceState[s][14]
					NumViews.value = InterfaceState[s][15]
					DRLevel.value = InterfaceState[s][16]
					OverRideBitmapSize.checked = InterfaceState[s][17]
					TrunkWidthOveride.value = InterfaceState[s][18]
					TrunkHeightOveride.value = InterfaceState[s][19]
					GenBranches.checked = InterfaceState[s][20]
					BranchCameraAtEyeHeight.checked = InterfaceState[s][21]
					SplitMethod.checked = InterfaceState[s][22]
					BranchBitMapSize.value = InterfaceState[s][23]
					BranchNumViews.value = InterfaceState[s][24]
					BranchDRLevel.value = InterfaceState[s][25]
					BranchOverRideBitmapSize.checked = InterfaceState[s][26]
					BranchWidthOveride.value = InterfaceState[s][27]
					BranchHeightOveride.value = InterfaceState[s][28]

					try
						(
							Area = MasterObjData[s][3]
							if Area == "BE" do GameArea.selection = 1
							if Area == "JR" do GameArea.selection = 2
							if Area == "PH" do GameArea.selection = 3
							if Area == "IJ" do GameArea.selection = 4
							if Area == "TO" do GameArea.selection = 5
							if Area == "PL" do GameArea.selection = 6
							if Area == "PV" do GameArea.selection = 7
							if Area == "SH" do GameArea.selection = 8
							if Area == "LA" do GameArea.selection = 9
							if Area == "AS" do GameArea.selection = 10
							if Area == "SU" do GameArea.selection = 11
						) catch (
							format "Area test failed...\n"
							-- GameArea.selection = 7
						)

					if renderSeperate.checked then
						(
							CTrunk = MasterObjData[s][1]
							CBranches = MasterObjData[s][2]
							CTrunk = copy MasterObjData[s][1]
							CTrunk.wirecolor = MasterObjData[s][1].wirecolor
							CTrunk.mat = copy MasterObjData[s][1].mat
							if MasterObjData[s][2] != undefined do 
								(
									CBranches = copy MasterObjData[s][2]
									CBranches.wirecolor = MasterObjData[s][2].wirecolor
									CBranches.mat = copy CBranches.mat
								)
						) else (
							format "MasterObjData[s]: %\n" MasterObjData[s]
							CTrunk = copy MasterObjData[s][1]
							try (CTrunk.wirecolor = MasterObjData[s][1].wirecolor) catch ()
							CTrunk.mat = copy MasterObjData[s][1].mat
							SingleObject = true
							hide MasterObjData[s][1]
							--else (
							--	CTrunk = copy MasterObjData[s][1]
							--	CTrunk.wirecolor = MasterObjData[s][1].wirecolor
							--	CTrunk.mat = copy CTrunk.mat
							--	CBranches = copy MasterObjData[s][2]
							--	CBranches.wirecolor = CBranches.wirecolor
							--	CBranches.mat = copy CBranches.mat
							--	hide MasterObjData[s][1]
							--	hide MasterObjData[s][2]
							--	attach CTrunk CBranches
							--	CBranches = undefined
							--	AttachedTrunk = true
							--)
						)

					if CTrunk != undefined then
					(
						for i = 1 to 2 do
							(
								for o in objects do hide o
								if i == 1 do unhide CTrunk
								if CBranches != undefined do
									if i == 2 do unhide CBranches
		
								if renderSeperate.checked and i == 2 then
									NumVs = BranchNumViews.value as integer
								else
									NumVs = NumViews.value as integer
		
								CObj = undefined
								if i == 1 then
									if CTrunk != undefined do
										CObj = CTrunk
								else
									if CBranches != undefined do
										CObj = CBranches
								
								-- these arrays will hold the final scaled/cropped map data
								FinalOpacities = #()
								FinalTextures = #()
								InTexNameArray = #()
								OutTexNameArray = #()
								InOpNameArray = #()
								OutOpNameArray = #()
								
								if CObj != undefined then
								(
									CWireColor = CObj.wirecolor
									if (GenTrunk.checked and i == 1) or (GenBranches.checked and i == 2) do
										(
											status1.text = "Calculating object info..."
											OriginalMat = copy CObj.mat
											-- setup Area object info for tracking
											AreaKey = ""
											if GameArea.selection == 1 do (AreaKey = "BE")
											if GameArea.selection == 2 do (AreaKey = "JR")
											if GameArea.selection == 3 do (AreaKey = "PH")
											if GameArea.selection == 4 do (AreaKey = "IJ")
											if GameArea.selection == 5 do (AreaKey = "TO")
											if GameArea.selection == 6 do (AreaKey = "PL")
											if GameArea.selection == 7 do (AreaKey = "PV")
											if GameArea.selection == 8 do (AreaKey = "SH")
											if GameArea.selection == 9 do (AreaKey = "LA")
											if GameArea.selection == 10 do (AreaKey = "AS")
											if GameArea.selection == 11 do (AreaKey = "SU")
											-- DL is the detail level
											DL = (DRLevel.value as integer) as string
											if renderSeperate.checked and i == 2 do
												DL = (BranchDRLevel.value as integer) as string
											-- initialize some arrays
											BitMapArray = #()
											OpacityMapArray = #()
											DRObjectsArray = #()
											-- Set the Background color to black
											backgroundColor = (color 0 0 0)
											-- grab and set the Ambient light color to what's specified in the interface
											AmbientColor = AmbientCol.color
											-- setup field of vision
											fov = 10
											CObjSize = (CObj.max - CObj.min)
											-- create a cricle for the camera path
											status1.text = "Calculating Camera..."
											CameraPath = circle()
		
											-- if center to base is checked, we determine the center of the lowest set of
											-- vertices in the mesh (the bottom of the trunk) and set the x,y center position to that.
											-- This way, we can make sure that the base of the trunk in each image view lies at the same position.
											-- Otherwise, we just use the object center
											if CenterToBase.checked then
												if i == 2 then
													(
														ObjCenter = CObj.center
													) else (
														ObjCenter = trunkCenter CObj TrunkCenterThresh.value
														ObjCenter = [ObjCenter.x, ObjCenter.y, CObj.center.z]
													)
												else 
													ObjCenter = CObj.center
		
											CameraPath.pos = ObjCenter
											rotate CameraPath 90 Z_Axis
											theta = fov
											offsetHeight = false
											-- here, we find the largest distance of either the height,
											-- or the distance between the x,y coordinates of the .min and .max (thus the maximun diameter)
											if (distance [CObj.max.x,CObj.max.y] [CObj.min.x,CObj.min.y]) > (CObj.max.z - CObj.min.z) then
												(
													d = (distance [CObj.max.x,CObj.max.y] [CObj.min.x,CObj.min.y])
													offsetHeight = true
													if debug do format "Using objects maximum DIAMETER for largest dimension...\n"
												) else (
													d = (CObj.max.z - CObj.min.z)
													if debug do format "Using objects maximum HEIGHT for largest dimension...\n"
												)
		
											-- setup the camera and it's position so that the object lies square in it's viewport
											h = (d / (2 * sin(theta / 2)))
											x = h * (cos(theta))
											NewCamera = targetCamera()
											NewCamera.fov = fov
											NewCamera.target = targetObject()
											NewCamera.target.pos = ObjCenter
											NewDir = normalize (NewCamera.pos - NewCamera.target.pos)
										 	NewCamera.pos = NewCamera.target.pos + NewDir * x
											CameraDist = distance NewCamera.pos Newcamera.target.pos
											CameraPath.radius = CameraDist
											NewCamera.pos.controller = path()
											NewCamera.pos.controller.path = CameraPath
		
											BoxDimension = ((distance NewCamera.pos NewCamera.target.pos) * (tan(fov * 0.5))) * 2
											-- could be used as scale factor???
											fScale = [BoxDimension, BoxDimension]
											
											-- if we're using the largest diameter of the object, we need to change the object height so
											-- that the base of the new detail reduced object matches the base of th object itself.
											if (offsetHeight == true) and (i != 2)  do
												(
													Objheight = (CObj.max.z - CObj.min.z) * 0.5
													ViewHeight = (((distance NewCamera.pos NewCamera.target.pos) * (tan(fov * 0.5))) * 2) * 0.5
													HeightOffset = abs(Objheight - ViewHeight)
													CameraPath.pos = [CameraPath.pos.x, CameraPath.pos.y, CObj.center.z]
													--CameraPath.pos = [CameraPath.pos.x, CameraPath.pos.y, CameraPath.pos.z + HeightOffset]
													NewCamera.target.pos = CameraPath.pos
													if debug do format "Camera Offset height:%\n" HeightOffset
												)
		
											-- set the camera height to be at eye level from the base of the tree
											-- eye level is 1.70 Meters
											if CameraAtEyeHeight.checked and i == 1 do
												(
													newCameraHeight = (CObj.center.z - (CObj.max.z - CObj.min.z) * 0.5) + 1.70
													CameraPath.pos = [CameraPath.pos.x, CameraPath.pos.y, newCameraHeight]
												)
		
											if BranchCameraAtEyeHeight.checked and i == 2 do
												(
													-- we base eye height for the branches off the dimensions of the trunk
													newCameraHeight = (CTrunk.center.z - (CTrunk.max.z - CTrunk.min.z) * 0.5) + 1.65
													CameraPath.pos = [CameraPath.pos.x, CameraPath.pos.y, newCameraHeight]
												)
		
											-- setup the animation of the camera about the path based on the number of views the user wants to use
											deletekeys NewCamera.pos.controller #allkeys
											c = NewCamera.pos.controller
											addNewKey c 0
											addNewKey c NumVs
											animate on
												(
													at time 0 c.percent = 0.0
													at time NumVs c.percent = 100.0
												)
											status1.text = "Generating Light..."
											-- makes sure any and all lights are deleted
											for l in lights do delete l
											CLight = freespot()
											CLight.dir = SunArray[GameArea.selection]
											CLight.HotSpot = 60
											CLight.color = (color 255 255 255)
											CLight.Multiplier = CMult.value
											SunDir = CLight.dir
											theta = CLight.HotSpot
											d = distance CObj.max CObj.min
											x = (d / (2 * sin(theta / 2)))
											CLight.pos = (CObj.center + SunDir * 1000)
											
											if i == 1 and isPrelit.checked do
												(
													ambientColor = (color 255 255 255)
													lts = for l in lights collect l
													delete lts
												)
		
											status1.text = "Calculating Bitmap info and Materials..."
											-- determine the bitmap size
											BitMapSizeValue = ((PixelsPerMeter.value as float) * BoxDimension) as integer
											if BitMapSizeValue > 300 do BitMapSizeValue = 300
		
											if debug do format "Rendered map dimension: %\n" BitMapSizeValue
		
											status1.text = "Rendering texture maps..."
											-- Render all the texture maps
											for j = 1 to NumVs do
												(
													Append BitMapArray (render camera:NewCamera outputwidth:BitMapSizeValue outputheight:BitMapSizeValue frame:j vfb:off)
													if debug do display BitMaparray[j]
												)
		
											if classof CObj.mat == StandardMaterial do
												(
													CObj.mat.diffuse = (color 255 255 255)
													CObj.mat.DiffuseMapEnable = false
													CObj.mat.BumpMapEnable = false
													CObj.mat.shinestrength = 0.0
													CObj.mat.selfIllum = 100
												)
		
											if classof CObj.mat == multiMaterial do
												(
													ns  = CObj.mat.numsubs
													for j = 1 to ns do
														(
															CObj.mat[j].diffuse = (color 255 255 255)
															CObj.mat[j].DiffuseMapEnable = false
															CObj.mat[j].BumpMapEnable = false
															CObj.mat[j].shinestrength = 0.0
															CObj.mat[j].selfIllum = 100
														)
												)
											
											status1.text = "Rendering opacity maps..."
											-- render all the opacity maps
											for j = 1 to NumVs do
												(
													Append OpacityMapArray (render camera:NewCamera outputwidth:BitMapSizeValue outputheight:BitMapSizeValue frame:j vfb:off)
													if debug do display OpacityMaparray[j]
												)
		
											CObj.mat = OriginalMat
		
											if classof CObj.mat == StandardMaterial do
												(
													CObj.mat.DiffuseMapEnable = true
													CObj.mat.BumpMapEnable = true
													CObj.mat.shinestrength = 0.0
													CObj.mat.selfIllum = 0.0
												)
		
											if classof CObj.mat == multiMaterial do
												(
													ns  = CObj.mat.numsubs
													for j = 1 to ns do
														(
															CObj.mat[j].DiffuseMapEnable = true
															CObj.mat[j].BumpMapEnable = true
															CObj.mat[j].shinestrength = 0.0
															CObj.mat[j].selfIllum = 0.0
														)
												)
		
											CNamePrefix = ""
											CNameSuffix = ""
		
											for j = 1 to CObj.name.count do
												if CObj.name[j] == "~" do
													MagicIndex = j
											if MagicIndex != undefined then
												(
													CNamePrefix = substring CObj.name 1 (MagicIndex - 1)
													CNameSuffix = ((substring CObj.name (MagicIndex + 1) 3) + ((DRLevel.value as integer) as string))
												) else (
													for j = 1 to CObj.name.count do
														if CObj.name[j] == "-" do
															MagicIndex = j
		
													if magicIndex == undefined do
														MagicIndex = CObj.name.count
		
													CNamePrefix = substring CObj.name 1 (MagicIndex - 1)
													CNameSuffix = ((substring CObj.name (MagicIndex + 1) 3) + ((DRLevel.value as integer) as string))
												) 
		
											ProgressStart "Generating Views..."
											for t = 1 to NumVs do
												(
													at time t
														(
															ProgressUpdate ((t/NumVs as float) * 100)
															-- ************************************************************
															-- ** Process the bitmaps**************************************
															-- ************************************************************
															if i == 2 and SplitMethod.checked do ()
															-- process the bitmaps for size/scale
															status1.text = "Processing Bitmaps..."
															iStartWidth = OpacityMapArray[t].width
															iStartHeight = OpacityMapArray[t].height
		
															BitmapInfo = ThresholdBitmap OpacityMapArray[t] (OpacityThresh.value as integer)
															-- bitmapInfo = #(map, startpoint, new map size)
															if debug do
																(
																	format "%\n" BitmapInfo
																	display OpacityMapArray[t]
																)
															-- calculate the width and height scalar values for the geometry based on the new cropped areas
															WidthScalar = BitmapInfo[3].x as float / OpacityMapArray[t].width as float
															HeightScalar = BitmapInfo[3].y as float / OpacityMapArray[t].height as float
		
															-- crop the bitmaps
															OpacityMapArray[t] = CropBitmap BitmapInfo[1] BitmapInfo[2] [(BitmapInfo[3].x), BitmapInfo[3].y]
															BitMapArray[t] = CropBitmap BitmapArray[t] BitmapInfo[2] [(BitmapInfo[3].x), BitmapInfo[3].y]
		
															iCropWidth = OpacityMapArray[t].width
															iCropHeight = OpacityMapArray[t].height
															
															if i == 2 and SplitMethod.checked then
																(
																	w1o = ((iStartWidth / 2) - BitmapInfo[2].x) as integer
																	OpacityMapArray[t] = CropBitmap OpacityMapArray[t] [w1o,0] [(iCropWidth - w1o), iCropHeight]
																	BitMapArray[t] = CropBitmap BitMapArray[t] [w1o,0] [(iCropWidth - w1o), iCropHeight]
																	NewBitMapWidth = (BitmapInfo[3].x / 2)
																	NewBitMapHeight = BitmapInfo[3].y
																) else (
																	NewBitMapWidth = BitmapInfo[3].x
																	NewBitMapHeight = BitmapInfo[3].y
																)
		
															-- Extra checking for illegal map sizes (> 256, < 8)
															if NewBitMapHeight > 256 do NewBitMapHeight = 256
															if NewBitMapWidth > 256 do NewBitMapWidth = 256
															if NewBitMapHeight < 8 do NewBitMapHeight = 8
															if NewBitMapWidth < 8 do NewBitMapWidth = 8
		
															if RescaleBitmaps.checked do
																(
																	reCalc = true
																	if i == 1 and OverRideBitmapSize.checked do
																			(
																				NewBitMapWidth = TrunkWidthOveride.value
																				NewBitMapHeight = TrunkHeightOveride.value
																				reCalc = false
																			)
		
																	if i == 2 and BranchOverRideBitmapSize.checked do
																			(
																				NewBitMapWidth = BranchWidthOveride.value
																				NewBitMapHeight = BranchHeightOveride.value
																				reCalc = false
																			)
		
																	if reCalc == true do
																		(
																			-- calculate the new bitmap dimensions to the nearest power of two size
																			NewBitMapWidth = NearPowOfTwo NewBitMapWidth sizeMaps.state
																			NewBitMapHeight = NearPowOfTwo NewBitMapHeight sizeMaps.state
																			NSize = (PowOfLimit NewBitMapWidth NewBitMapHeight)
																			NewBitMapWidth = NSize.x as integer
																			NewBitMapHeight = NSize.y as integer
																	)

																if AutoDetermineMapSizes.checked do
																	(
																		if debug do format "Auto Determining Map Sizes...\n"
																		iHt = (CObj.max.z - CObj.min.z)
																		-- figure out the predetermined height of the map
																		if DRLevel.value == 1 then
																			(
																				if iHt >= 0 and iHt <= 1 do NewBitMapHeight = 64
																				if iHt > 1 and iHt <= 5 do NewBitMapHeight = 128
																				if iHt > 5 do NewBitMapHeight = 256
																				if debug do format "DRLevel:% Bitmap height: %\n" DRLevel.value NewBitMapHeight
																			) else (
																				-- Detail Levels > 2
																				if iHt >= 0 and iHt <= 1 do NewBitMapHeight = 32
																				if iHt > 1 and iHt <= 5 do NewBitMapHeight = 64
																				if iHt > 5 do NewBitMapHeight = 128
																				if debug do format "DRLevel:% Bitmap height: %\n" DRLevel.value NewBitMapHeight
																			)
																		-- now determine the width
																		iWth = (distance [CObj.max.x, CObj.max.y] [CObj.min.x, CObj.min.y])
																		NewBitMapWidth = ((iWth * NewBitMapHeight) / iHt) as integer
																		if debug do format "iHt:%\tiWd:%\tbHt:%\tbWd:%\n" iHt iWth NewBitMapHeight NewBitMapWidth
																		NewBitMapWidth = NearPowOfTwo NewBitMapWidth 2 -- always go lower
																		NSize = (PowOfLimit NewBitMapWidth NewBitMapHeight)
																		NewBitMapWidth = NSize.x as integer
																		NewBitMapHeight = NSize.y as integer
																		if debug do format "BitMap Size calculated to be %\n" NSize
																	)
																)
		
															if debug do format "Initial BitmapSize: %\t%\nNew Bitmap size: %\t%\n" BitmapInfo[3].x BitmapInfo[3].y NewBitMapWidth NewBitMapHeight
		
															-- set the filenames...
															FinalOpName = (WorkingFolder + CNamePrefix + t as string + "~" + CNameSuffix + "o8.bmp")
															FinalTexName = (WorkingFolder + CNamePrefix + t as string + "~" + CNameSuffix + "t2.bmp")
															InTexName = (WorkingFolder + "t" + t as string + ".bmp")
															OutTexName = (WorkingFolder + "t_" + t as string + ".bmp")
															InOpName = (WorkingFolder + "o" + t as string + ".bmp")
															OutOpName = (WorkingFolder + "o_" + t as string + ".bmp")
		
															if i == 2 and SplitMethod.checked then
																(
																	NewOp = bitmap (NewBitMapWidth / 2) NewBitMapHeight
																	NewTex = bitmap (NewBitMapWidth / 2) NewBitMapHeight
																) else (
																	NewOp = bitmap NewBitMapWidth NewBitMapHeight
																	NewTex = bitmap NewBitMapWidth NewBitMapHeight
																)
															
															try
																(
																	copy OpacityMapArray[t] NewOp
																	copy BitMapArray[t] NewTex
																) catch (
																	MessageBox "Something fucked up.\nGo get Kyle."
																)
															
															-- Start the Graham's Scan
															-- Identify all the edge pixels
															status1.text = "Identifying Edges...."
															p = IdentifyEdges NewOp YIncrement.value
		
															-- add some random jitter to the array of points so the angle array will sort properly
															aj = AngleJitter.value
															for j = 1 to p.count do
																(
																	if p[j].x == 0 do (p[j].x += (random 0 aj))
																	if p[j].y == 0 do (p[j].y += (random 0 aj))
																	if p[j].x == NewOp.width do (p[j].x += (random -aj 0))
																	if p[j].y == NewOp.height do (p[j].y += (random -aj 0))
																	if p[j].x < NewOp.width and p[j].x > 0 do (p[j].x += (random -aj aj))
																	if p[j].y < NewOp.height and p[j].y > 0 do (p[j].y += (random -aj aj))
																)
		
															-- do the Graham Scan
															status1.text = "Calculating convex hull..."
															Hullpoints = GrahamScan p
															-- remove the jitter to the array of points are integers again
															for j = 1 to Hullpoints.count do
																(Hullpoints[j] = [(rnd Hullpoints[j].x), (rnd Hullpoints[j].y)])
															status1.text = "Reducing Points..."
															HullPoints = pruneClosePoints HullPoints PrunePointsThreshold.value
		
															-- set the new filenames
															NewOp.filename = InOpName
															NewTex.filename = InTexName
		
															-- Delete the files (if they're already there)
															DeleteFile NewOp.filename
															DeleteFile NewTex.filename
		
															-- and save them...
															save NewTex
															save NewOp
		
															status1.text =  "Starting Alchemy..."
															-- Process Maps via Alchemy
															doscommand ("alchemy -w -o -b " + InOpName + " " + OutOpName)
															doscommand ("alchemy -w -o " + InTexName + " " + OutTexName)
		
															-- doscommand ("alchemy -w -o -b -X" + NewBitMapWidth as string + " -Y" + NewBitMapHeight as string + " " + InOpName + " " + OutOpName)
															-- doscommand ("alchemy -w -o -X" + NewBitMapWidth as string + " -Y" + NewBitMapHeight as string + " " + InTexName + " " + OutTexName)
															if debug do format "Alchemy Done!...\nProcessing Bitmaps Finished!\n"
		
															-- Delete files that may already be there
															deleteFile FinalOpName
															deleteFile FinalTexName
		
															-- Copy the processed bitmaps to their final name/location
															copyFile OutOpName FinalOpName
															copyFile OutTexName FinalTexName
		
															-- ************************************************************
															-- ** Create the new mesh *************************************
															-- ************************************************************
															-- calculate the new uv's for Hullpoints
															status1.text = "Calculating Texture Vertices..."
															minX = HullPoints[1].x
															maxX = HullPoints[1].x
															minY = HullPoints[1].y
															maxY = HullPoints[1].y
															for i = 1 to HullPoints.count do
																(
																	if HullPoints[i].x > maxX do maxX = HullPoints[i].x
																	if HullPoints[i].x < minX do minX = HullPoints[i].x
																	if HullPoints[i].y > maxY do maxY = HullPoints[i].y
																	if HullPoints[i].y < minY do minY = HullPoints[i].y
																)
		
															iWidth = maxX - minX
															iHeight = maxY - minY
															iDimension = [iWidth, iHeight]
															iCorner = [minX, minY]
															
															du = 1.0 / iWidth
															dv = 1.0 / iHeight
															
															-- flip all the y points
															for j = 1 to Hullpoints.count do (Hullpoints[j].y = iHeight - Hullpoints[j].y)
		
															TVertArray = #()
															for j = 1 to Hullpoints.count do
																(
																	Tvert = [(HullPoints[j].x * du), (HullPoints[j].y * dv),1.0]
																	append TVertArray Tvert
																)
		
															BoxDimension = ((distance NewCamera.pos NewCamera.target.pos) * (tan(fov * 0.5))) * 2
		
															if i == 2 and SplitMethod.checked then
																PixelSize = (BoxDimension as float / iCropWidth)
															else
																PixelSize = (BoxDimension as float / BitMapSizeValue)
		
															-- PixelSize = (BoxDimension as float / BitMapSizeValue as float)
															if debug do format "PixelSize:%\n" PixelSize
		
															-- calculate the width and height offsets for the new geometry
															w1 = BitmapInfo[2].x
															w2 = BitmapInfo[1].width - (w1 + BitmapInfo[3].x)
															AvgFreeWidth = (BitmapInfo[1].width - BitmapInfo[3].x) * 0.5
															if w2 >= w1 then
																ObjectOffsetW = (AvgFreeWidth - w1) * PixelSize
															else
																ObjectOffsetW = -(AvgFreeWidth - w2) * PixelSize
		
															if i == 2 and SplitMethod.checked do
																ObjectOffsetW = (iStartWidth - iCropWidth) * PixelSize
		
															h1 = BitmapInfo[2].y
															h2 = BitmapInfo[1].height - (h1 + BitmapInfo[3].y)
															AvgFreeHeight = (BitmapInfo[1].height - BitmapInfo[3].y) * 0.5
		
															if h2 >= h1 then
																ObjectOffsetV = (AvgFreeHeight - h1) * PixelSize
															else
																ObjectOffsetV = -(AvgFreeHeight - h2) * PixelSize
		
															if debug do
																(
																	format "w1:% w2:% h1:% h2:%\n" w1 w2 h1 h2
																	format "Height Offset:%\nWidth Offset:%\nAvg Free: % %\n" ObjectOffsetV ObjectOffsetW AvgFreeWidth AvgFreeHeight
																)
		
															-- width and height of the object in 3space
															NewHeight = BoxDimension as float * HeightScalar
															if i == 2 and SplitMethod.checked then
																NewWidth = (BoxDimension as float * WidthScalar) / 2
															else
																NewWidth = (BoxDimension as float * WidthScalar)
		
															-- make the box
															b = box()
															b.width = NewWidth
															b.height = BoxDimension
															b.length = NewHeight
															b.pos = NewCamera.target.pos
															b.dir = -NewCamera.dir
															if debug do format "Camera Dir vector @ %: %\n" t -NewCamera.dir
															b.mapcoords = true
															convertToMesh b
															-- delete the verts off the box that we don't need.
															for j = 1 to 4 do deleteVert b b.numverts
															if NewCamera.dir.x < 0.0 do
																in coordsys local rotate b 180 z_axis
															in coordsys local rotate b 180 z_axis
															if SplitMethod.checked and i == 2 then
																in coordsys local move b [-(NewWidth * 0.5), -ObjectOffsetV, 0]
															else
																in coordsys local move b [ObjectOffsetW, ObjectOffsetV, 0]
															convertToMesh b																		
															
															-- Calculate the new points in 3space via the uv coordinates
															P0 = getVert b 2
															P1 = getVert b 1
															P2 = getVert b 4
															P3 = getVert b 3
		
															fVertArray = #()
		
															for j = 1 to TVertArray.count do
																(
																	Px = (P0.x + (TVertArray[j].x * (P1.x - P0.x)) + (TVertArray[j].y * (P2.x - P0.x)))
																	Py = (P0.y + (TVertArray[j].x * (P1.y - P0.y)) + (TVertArray[j].y * (P2.y - P0.y)))
																	Pz = (P0.z + (TVertArray[j].x * (P1.z - P0.z)) + (TVertArray[j].y * (P2.z - P0.z)))
																	append fVertArray [Px,Py,Pz]
																)
															iFaceArray = #([1,2,3])
															for i = 3 to (Hullpoints.count - 1) do (append iFaceArray [1,i,i+1])
															-- construct the mesh
															delete b
															b = mesh vertices:fVertArray faces:iFaceArray
															if b.numverts > 16 do (format "Warning: more than 16 verts detected in convex hull object... please check and regenerate if needed...\n")
															-- apply the texture coordinates
															addmodifier b (UVWMap())
															convertToMesh b
															for i = 1 to b.numfaces do
																(
																	iFace = getFace b i
																	iTFace = getTVFace b i
																	setTvert b iTFace.x (TVertArray[iFace.x])
																	setTvert b iTFace.y (TVertArray[iFace.y])
																	setTvert b iTFace.z (TVertArray[iFace.z])
																)
															-- Set the Edge visibility
															for i = 1 to b.numfaces do
																(
																	if i == 1 do setEdgeVis b i 3 off
																	if i == b.numfaces do setEdgeVis b i 1 off
																	if i > 1 and i < b.numfaces do
																		(
																			SetEdgeVis b i 1 off
																			SetEdgeVis b i 3 off
																		)
																)
															update b

															df = #()
															for i = 1 to b.numfaces do
																(
																	Cface = getface b i
																	n = 4
																	NewX = rndP3 (getvert b Cface.x) n
																	NewY = rndP3 (getvert b Cface.y) n
																	NewZ = rndP3 (getvert b Cface.z) n
																	if (Thresh NewX NewY 0.001) do 
																		if findItem df i == 0 do (append df i)
												
																	if (Thresh NewX NewZ 0.001) do
																		if findItem df i == 0 do (append df i)
												
																	if (Thresh NewZ NewY 0.001) do
																		if findItem df i == 0 do (append df i)

																	-- check for a degenerate normal
																	if (GetFaceNormal b i) == [0,0,0] do	
																		if findItem df i == 0 do (append df i)
																)

															if df.count > 0 do 
																(
																	format "found degenerate polygons... deleting them\n"
																	for i = df.count to 1 by -1 do
																		deleteFace b df[i]
																	update b
																)
															
															-- Check for Stray Vertices
															StrayVertArray = (for i = 1 to b.numverts collect i)
															UsedVerts = #()
															NewVertSelection = #()
												
															for i = 1 to b.numfaces do
																(
																	CFace = (getface b i)
																	if (findItem UsedVerts CFace.x) == 0 do (append UsedVerts CFace.x)
																	if (findItem UsedVerts CFace.y) == 0 do (append UsedVerts CFace.y)
																	if (findItem UsedVerts CFace.z) == 0 do (append UsedVerts CFace.z)
																)
												
															for i = 1 to StrayVertArray.count do
																if (findItem UsedVerts i) == 0 do (append NewVertSelection i)

															if NewVertSelection.count > 0 then
																(
																	format "Stray vertices were found... deleting them\n"
																	for i = NewVertSelection.count to 1 by -1 do
																		deleteVert b NewVertSelection[i]
																	update b
																)

															if DRLevel.value == 1 then b.wirecolor = scaleColor CWireColor 0.75
															if DRLevel.value == 2 then b.wirecolor = scaleColor CWireColor 0.65
															if DRLevel.value == 3 then b.wirecolor = scaleColor CWireColor 0.50
															if DRLevel.value == 4 then b.wirecolor = scaleColor CWireColor 0.40
															-- ************************************************************
															-- ** Generate a new material *********************************
															-- ************************************************************
															-- make sure all face IDs are set to 1
															status1.text =  "Building a new material..."
															for j = 1 to b.numfaces do setFaceMatID b j 1
															NMat = standardMaterial()
															-- give it a random color
															NMat.diffuse = random (color 20 20 20) (color 220 220 220)
															NMat.diffuseMap = bitmaptexture()
															NMat.OpacityMap = bitmaptexture()
															-- specify the bitmap names
															NMat.diffuseMap.filename = FinalTexName
															NMat.OpacityMap.filename = FinalOpName
															NMat.shinestrength = 0.0
															NMat.name = substring OutTexName 1 (OutTexName.count - 3)
															NMat.selfIllum = 100
															b.mat = NMat
															append DRObjectsArray b
															if debug do format "Finished building new material!\n"
														)
												)
											ProgressEnd()
											-- Attach the detail reduced objects together
											for j = DRObjectsArray.count to 2 by -1 do
												attach DRObjectsArray[1] DRObjectsArray[j]
											DRObject = DRObjectsArray[1]
											DRObject.mat.name = (CObj.name + "dr")
		
											if i == 1 then
												DRL = DRLevel.value as integer
											else
												DRL = BranchDRLevel.value as integer
												
											DRObject.name = ("$" + (substring CObj.name 1 (CObj.name.count - 4)) + DRL as string + "-00")
											DRObject.pivot = CObj.pivot
											f = for i = 1 to DRObject.numfaces collect i
											setFaceSelection DRObject f
											if flip != undefined do
												if flip then CopyAndFlipFaces DRObject
											try (
												if (i != 1) or (isPrelit.checked == false) do
													(
														delete CLight
														CLight = undefined
													)
											) catch ()
											if debug != true do
												(
													delete NewCamera
													NewCamera = undefined
													delete CameraPath
													CameraPath = undefined
												)
											CObj.mat = OriginalMat
										)
								) else (
									if renderSeperate.checked do
										MessageBox "No Objects Defined!"
								)
								gc() -- collect some garbage
								status1.text = ""
							)
						-- Lonnie, comment the next two lines out "--"
						-- if SingleObject == true do
						-- 	delete CTrunk
						delete CTrunk
						if CBranches != undefined do delete CBranches
						-- unhide everything at the end...
						for o in objects do unhide o
						CTrunk = CBranches = undefined
						status1.text = ""
					) else (
						MessageBox "You must initialize the objects first"
					)
			)
		messageBox "Complete"
	)


				-- *******************************************************************************************************
				-- ******************																							 ***********************
				-- ******************										Generate Shadow Object		             ***********************
				-- ******************																							 ***********************
				-- *******************************************************************************************************

				on CreateShadowObject pressed do
					(
						for s = 1 to InterfaceState.count do
							(
								BatchList.selection = s
								renderSeperate.checked = InterfaceState[s][1]
								CenterToBase.checked = InterfaceState[s][2]
								TrunkCenterThresh.value = InterfaceState[s][3]
								OpacityThresh.value = InterfaceState[s][4]
								RescaleBitmaps.checked = InterfaceState[s][5]
								sizeMaps.state = InterfaceState[s][6]
								PixelsPerMeter.value = InterfaceState[s][7]
								YIncrement.value = InterfaceState[s][8]
								PrunePointsThreshold.value = InterfaceState[s][9]
								AngleJitter.value = InterfaceState[s][10]
								GenTrunk.checked = InterfaceState[s][11]
								CameraAtEyeHeight.checked = InterfaceState[s][12]
								isPrelit.checked = InterfaceState[s][13]
								BitMapSize.value = InterfaceState[s][14]
								NumViews.value = InterfaceState[s][15]
								DRLevel.value = InterfaceState[s][16]
								OverRideBitmapSize.checked = InterfaceState[s][17]
								TrunkWidthOveride.value = InterfaceState[s][18]
								TrunkHeightOveride.value = InterfaceState[s][19]
								GenBranches.checked = InterfaceState[s][20]
								BranchCameraAtEyeHeight.checked = InterfaceState[s][21]
								SplitMethod.checked = InterfaceState[s][22]
								BranchBitMapSize.value = InterfaceState[s][23]
								BranchNumViews.value = InterfaceState[s][24]
								BranchDRLevel.value = InterfaceState[s][25]
								BranchOverRideBitmapSize.checked = InterfaceState[s][26]
								BranchWidthOveride.value = InterfaceState[s][27]
								BranchHeightOveride.value = InterfaceState[s][28]
			
								Area = MasterObjData[s][3]
								if Area == "BE" do GameArea.selection = 1
								if Area == "JR" do GameArea.selection = 2
								if Area == "PH" do GameArea.selection = 3
								if Area == "IJ" do GameArea.selection = 4
								if Area == "TO" do GameArea.selection = 5
								if Area == "PL" do GameArea.selection = 6
								if Area == "PV" do GameArea.selection = 7
								if Area == "SH" do GameArea.selection = 8
								if Area == "LA" do GameArea.selection = 9
								if Area == "AS" do GameArea.selection = 10
								if Area == "SU" do GameArea.selection = 11
			
								if renderSeperate.checked then
									(
										CTrunk = MasterObjData[s][1]
										CTrunk.wirecolor = MasterObjData[s][1].wirecolor
										CBranches = MasterObjData[s][2]
										CTrunk = copy MasterObjData[s][1]
										CTrunk.mat = copy MasterObjData[s][1].mat
										if MasterObjData[s][2] != undefined do 
											(
												CBranches = copy MasterObjData[s][2]
												CBranches.wirecolor = MasterObjData[s][2].wirecolor
												CBranches.mat = copy CBranches.mat
											)
									) else (
										CTrunk = MasterObjData[s][1]
										CTrunk.wirecolor = MasterObjData[s][1].wirecolor
										CBranches = MasterObjData[s][2]
										if CBranches == undefined then
											(
												CTrunk = copy MasterObjData[s][1]
												CTrunk.wirecolor = MasterObjData[s][1].wirecolor
												CTrunk.mat = copy MasterObjData[s][1].mat
												SingleObject = true
												hide MasterObjData[s][1]
											) else (
												CTrunk = copy MasterObjData[s][1]
												CTrunk.wirecolor = MasterObjData[s][1].wirecolor
												CTrunk.mat = copy CTrunk.mat
												CBranches = copy MasterObjData[s][2]
												CBranches.wirecolor = MasterObjData[s][2].wirecolor
												CBranches.mat = copy CBranches.mat
												hide MasterObjData[s][1]
												hide MasterObjData[s][2]
												attach CTrunk CBranches
												CBranches = undefined
												AttachedTrunk = true
											)
									)

								if CTrunk != undefined then
									(
										for i = 1 to 2 do
											(
												for o in objects do hide o
												if i == 1 do unhide CTrunk
												if CBranches != undefined do
													if i == 2 do unhide CBranches
				
												if renderSeperate.checked and i == 2 then
													NumVs = BranchNumViews.value as integer
												else
													NumVs = NumViews.value as integer
				
												CObj = undefined
												if i == 1 then
													if CTrunk != undefined do
														CObj = CTrunk
												else
													if CBranches != undefined do
														CObj = CBranches
				
												if i == 1 and CBranches != undefined then
													(
														-- this is for the trunk
														b = copy CObj
														r = SunArray[GameArea.selection]
														r = normalize [r.x, r.y, 0]
														r = ray [0,0,0] r
														FlattenObject b r
														addmodifier b (UVWMap())
														convertToMesh b
														b.mat = standardMaterial()
														b.name = ("$" + (substring CObj.name 1 (CObj.name.count - 4)) + "5-00")
													) else (
														if i == 1 do (CObj = CTrunk)
														-- these arrays will hold the final scaled/cropped map data
														FinalOpacities = #()
														FinalTextures = #()
														InTexNameArray = #()
														OutTexNameArray = #()
														InOpNameArray = #()
														OutOpNameArray = #()
														
														if CObj != undefined then
															(
																if (GenBranches.checked and (i == 2)) or ((i == 1) and (CObj == CTrunk)) do
																	(
																		status1.text = "Calculating object information..."
																		cw = CObj.wirecolor
																		OriginalMat = copy CObj.mat
																		-- setup Area object info for tracking
																		AreaKey = ""
																		if GameArea.selection == 1 do (AreaKey = "BE")
																		if GameArea.selection == 2 do (AreaKey = "JR")
																		if GameArea.selection == 3 do (AreaKey = "PH")
																		if GameArea.selection == 4 do (AreaKey = "IJ")
																		if GameArea.selection == 5 do (AreaKey = "TO")
																		if GameArea.selection == 6 do (AreaKey = "PL")
																		if GameArea.selection == 7 do (AreaKey = "PV")
																		if GameArea.selection == 8 do (AreaKey = "SH")
																		if GameArea.selection == 9 do (AreaKey = "LA")
																		if GameArea.selection == 10 do (AreaKey = "AS")
																		if GameArea.selection == 11 do (AreaKey = "SU")
																		-- DL is the detail level
																		DL = (DRLevel.value as integer) as string
																		if renderSeperate.checked and i == 2 do
																			DL = (BranchDRLevel.value as integer) as string
																		-- initialize some arrays
																		BitMapArray = #()
																		OpacityMapArray = #()
																		DRObjectsArray = #()
																		-- Set the Background color to black
																		backgroundColor = (color 0 0 0)
																		-- grab and set the Ambient light color to what's specified in the interface
																		AmbientColor = AmbientCol.color
																		-- setup field of vision
																		fov = 10
																		CObjSize = (CObj.max - CObj.min)
						
																		-- if center to base is checked, we determine the center of the lowest set of
																		-- vertices in the mesh (the bottom of the trunk) and set the x,y center position to that.
																		-- This way, we can make sure that the base of the trunk in each image view lies at the same position.
																		-- Otherwise, we just use the object center
																		ObjCenter = CObj.center
		
																		theta = fov
																		offsetHeight = false
																		-- here, we find the largest distance of either the height,
																		-- or the distance between the x,y coordinates of the .min and .max (thus the maximun diameter)
																		if (distance [CObj.max.x,CObj.max.y] [CObj.min.x,CObj.min.y]) > (CObj.max.z - CObj.min.z) then
																			(
																				d = (distance [CObj.max.x,CObj.max.y] [CObj.min.x,CObj.min.y])
																				offsetHeight = true
																				if debug do format "Using objects maximum DIAMETER for largest dimension...\n"
																			) else (
																				d = (CObj.max.z - CObj.min.z)
																				if debug do format "Using objects maximum HEIGHT for largest dimension...\n"
																			)
				
																		status1.text = "Calculating Camera..."
																		-- setup the camera and it's position so that the object lies square in it's viewport
																		h = (d / (2 * sin(theta / 2)))
																		x = h * (cos(theta))
																		NewCamera = targetCamera()
																		NewCamera.fov = fov
																		NewCamera.target = targetObject()
																		NewCamera.target.pos = ObjCenter
																		-- Set the camera direction as the light direction
																		NewDir = SunArray[GameArea.selection]
																		-- NewDir = normalize [NewDir.x, NewDir.y,0]
				
																	 	NewCamera.pos = NewCamera.target.pos + NewDir * x
																		CameraDist = (distance NewCamera.pos Newcamera.target.pos)
				
																		BoxDimension = ((CameraDist * (tan(fov * 0.5))) * 2)
																		BitMapSizeValue = BitMapSize.value as integer
				
																		-- generate the texturemap - this will (should) never be seen so we'll make it bright red.
																		TMap = CreateBitmap BitMapSizeValue BitMapSizeValue (color 20 77 12)
				
																		-- prep the materials for opacity map rendering
																		if classof CObj.mat == StandardMaterial do
																			(
																				CObj.mat.diffuse = (color 255 255 255)
																				CObj.mat.DiffuseMapEnable = false
																				CObj.mat.BumpMapEnable = false
																				CObj.mat.shinestrength = 0.0
																				CObj.mat.selfIllum = 100
																			)
				
																		if classof CObj.mat == multiMaterial do
																			(
																				ns  = CObj.mat.numsubs
																				for j = 1 to ns do
																					(
																						CObj.mat[j].diffuse = (color 255 255 255)
																						CObj.mat[j].DiffuseMapEnable = false
																						CObj.mat[j].BumpMapEnable = false
																						CObj.mat[j].shinestrength = 0.0
																						CObj.mat[j].selfIllum = 100
																					)
																			)
				
																		-- render the opacity map
																		OpacityMap = (render camera:NewCamera outputwidth:BitMapSizeValue outputheight:BitMapSizeValue vfb:off)
				
																		CObj.mat = OriginalMat
				
																		if classof CObj.mat == StandardMaterial do
																			(
																				CObj.mat.DiffuseMapEnable = true
																				CObj.mat.BumpMapEnable = true
																				CObj.mat.shinestrength = 0.0
																				CObj.mat.selfIllum = 0.0
																			)
				
																		if classof CObj.mat == multiMaterial do
																			(
																				ns  = CObj.mat.numsubs
																				for j = 1 to ns do
																					(
																						CObj.mat[j].DiffuseMapEnable = true
																						CObj.mat[j].BumpMapEnable = true
																						CObj.mat[j].shinestrength = 0.0
																						CObj.mat[j].selfIllum = 0.0
																					)
																			)
				
																		CNamePrefix = ""
																		CNameSuffix = ""
				
																		DetailReductionLevel = 5
																		for j = 1 to CObj.name.count do
																			if CObj.name[j] == "~" do
																				MagicIndex = j
																		if MagicIndex != undefined then
																			(
																				CNamePrefix = substring CObj.name 1 (MagicIndex - 1)
																				CNameSuffix = ((substring CObj.name (MagicIndex + 1) 3) + (DetailReductionLevel as string))
																			) else (
																				for j = 1 to CObj.name.count do
																					if CObj.name[j] == "-" do
																						MagicIndex = j
				
																				if magicIndex == undefined do
																					MagicIndex = CObj.name.count
				
																				CNamePrefix = substring CObj.name 1 (MagicIndex - 1)
																				CNameSuffix = ((substring CObj.name (MagicIndex + 1) 3) + (DetailReductionLevel as string))
																			) 
				
																		-- ************************************************************
																		-- ** Process the bitmaps *************************************
																		-- ************************************************************
																		-- process the bitmaps for size/scale
																		if debug do format "Processing Bitmaps...\n"
																		BitmapInfo = ThresholdBitmap OpacityMap (OpacityThresh.value as integer)
																		if debug do
																			(
																				format "%\n" BitmapInfo
																				display OpacityMap
																			)
																		-- calculate the width and height scalar values for the geometry based on the new cropped areas
																		WidthScalar = BitmapInfo[3].x as float / OpacityMap.width
																		HeightScalar = BitmapInfo[3].y as float / OpacityMap.height
				
																		-- crop the bitmaps
																		-- OpacityMap = CropBitmap BitmapInfo[1] BitmapInfo[2] BitmapInfo[3]
																		-- TMap = CropBitmap TMap BitmapInfo[2] BitmapInfo[3]
				
																		OpacityMap = CropBitmap BitmapInfo[1] BitmapInfo[2] [(BitmapInfo[3].x), BitmapInfo[3].y]
																		TMap = CropBitmap TMap BitmapInfo[2] [(BitmapInfo[3].x), BitmapInfo[3].y]
				
																		NewBitMapHeight = BitmapInfo[3].y
																		NewBitMapWidth = BitmapInfo[3].x
																		
																		-- Extra checking for illegal map sizes (> 256, < 8)
																		if NewBitMapHeight > 256 do NewBitMapHeight = 256
																		if NewBitMapWidth > 256 do NewBitMapWidth = 256
																		if NewBitMapHeight < 8 do NewBitMapHeight = 8
																		if NewBitMapWidth < 8 do NewBitMapWidth = 8
				
																		if RescaleBitmaps.checked do
																			(
																				reCalc = true
																				if (i == 2 and BranchOverRideBitmapSize.checked) do
																						(
																							NewBitMapWidth = BranchWidthOveride.value
																							NewBitMapHeight = BranchHeightOveride.value
																							reCalc = false
																						)

																				if (OverRideBitmapSize.checked and (i == 1) and (CObj == CTrunk)) do
																					(
																						NewBitMapWidth = TrunkWidthOveride.value
																						NewBitMapHeight = TrunkHeightOveride.value
																						reCalc = false
																					)

																				if reCalc == true do
																					(
																						-- calculate the new bitmap dimensions to the nearest power of two size
																						NewBitMapWidth = NearPowOfTwo NewBitMapWidth sizeMaps.state
																						NewBitMapHeight = NearPowOfTwo NewBitMapHeight sizeMaps.state
																						NSize = (PowOfLimit NewBitMapWidth NewBitMapHeight)
																						NewBitMapWidth = NSize.x as integer
																						NewBitMapHeight = NSize.y as integer
																					)
																			)
				
																		if debug do format "Initial BitmapSize: %\t%\nNew Bitmap size: %\t%\n" BitmapInfo[3].x BitmapInfo[3].y NewBitMapWidth NewBitMapHeight

																		FinalOpName = (WorkingFolder + CNamePrefix + "5~" + CNameSuffix + "o8.bmp")
																		FinalTexName = (WorkingFolder + CNamePrefix + "5~" + CNameSuffix + "t2.bmp")
																		InTexName = (WorkingFolder + "t5.bmp")
																		OutTexName = (WorkingFolder + "t_5.bmp")
																		InOpName = (WorkingFolder + "o5.bmp")
																		OutOpName = (WorkingFolder + "o_5.bmp")
				
																		-- set the filenames...
																		NewOp = bitmap NewBitMapWidth NewBitMapHeight
																		NewTex = bitmap NewBitMapWidth NewBitMapHeight

																		if debug do format "Bitmap Size: % %\n" NewBitMapWidth NewBitMapHeight														

																		try (
																			copy OpacityMap NewOp
																			copy TMap NewTex
																		) catch (
																			MessageBox "Something fucked up.\nGo get Kyle."
																		)
		
																		-- Start the Graham's Scan
																		-- Identify all the edge pixels
																		status1.text = "Identifying Edges...."
																		p = IdentifyEdges NewOp YIncrement.value
			
																		-- add some random jitter to the array of points so the angle array will sort properly
																		aj = AngleJitter.value
																		for j = 1 to p.count do
																			(
																				if p[j].x == 0 do (p[j].x += (random 0 aj))
																				if p[j].y == 0 do (p[j].y += (random 0 aj))
																				if p[j].x == NewOp.width do (p[j].x += (random -aj 0))
																				if p[j].y == NewOp.height do (p[j].y += (random -aj 0))
																				if p[j].x < NewOp.width and p[j].x > 0 do (p[j].x += (random -aj aj))
																				if p[j].y < NewOp.height and p[j].y > 0 do (p[j].y += (random -aj aj))
																			)
			
																		-- do the Graham Scan
																		status1.text = "Calculating convex hull..."
																		Hullpoints = GrahamScan p
																		-- remove the jitter to the array of points are integers again
																		for j = 1 to Hullpoints.count do
																			(Hullpoints[j] = [(rnd Hullpoints[j].x), (rnd Hullpoints[j].y)])
																		status1.text = "Reducing Points..."
																		HullPoints = pruneClosePoints HullPoints PrunePointsThreshold.value
																		
																		NewOp.filename = InOpName
																		NewTex.filename = InTexName
				
																		-- Delete the files (if they're already there)
																		DeleteFile NewOp.filename
																		DeleteFile NewTex.filename
				
																		-- and save them...
																		save NewTex
																		save NewOp
				
																		-- if debug do format "Starting Alchemy...\n"
																		-- Process Maps via Alchemy
																		doscommand ("alchemy -w -o -b " + InOpName + " " + OutOpName)
																		doscommand ("alchemy -w -o " + InTexName + " " + OutTexName)
				
																		if debug do format "Alchemy Done!...\nProcessing Bitmaps Finished!\n"
				
																		-- Delete files that may already be there
																		deleteFile FinalOpName
																		deleteFile FinalTexName
				
																		-- Copy the processed bitmaps to their final name/location
																		copyFile OutOpName FinalOpName
																		copyFile OutTexName FinalTexName
																		-- ************************************************************
																		-- ** Create the new mesh *************************************
																		-- ************************************************************
																		if debug do format "Building Geometry...\n"
		
																		-- calculate the new uv's for Hullpoints
																		status1.text = "Calculating Texture Vertices..."
																		minX = HullPoints[1].x
																		maxX = HullPoints[1].x
																		minY = HullPoints[1].y
																		maxY = HullPoints[1].y
																		for i = 1 to HullPoints.count do
																			(
																				if HullPoints[i].x > maxX do maxX = HullPoints[i].x
																				if HullPoints[i].x < minX do minX = HullPoints[i].x
																				if HullPoints[i].y > maxY do maxY = HullPoints[i].y
																				if HullPoints[i].y < minY do minY = HullPoints[i].y
																			)
			
																		iWidth = maxX - minX
																		iHeight = maxY - minY
																		iDimension = [iWidth, iHeight]
																		iCorner = [minX, minY]
																		
																		du = 1.0 / iWidth
																		dv = 1.0 / iHeight
																		
																		-- flip all the y points
																		for j = 1 to Hullpoints.count do (Hullpoints[j].y = iHeight - Hullpoints[j].y)
			
																		TVertArray = #()
																		for j = 1 to Hullpoints.count do
																			(
																				Tvert = [(HullPoints[j].x * du), (HullPoints[j].y * dv),1.0]
																				append TVertArray Tvert
																			)
		
				
																		BoxDimension = ((distance NewCamera.pos NewCamera.target.pos) * (tan(fov * 0.5))) * 2
																		PixelSize = (BoxDimension as float / BitMapSizeValue as float)
																		if debug do format "PixelSize:%\n" PixelSize
				
																		-- calculate the width and height offsets for the new geometry
																		w1 = BitmapInfo[2].x
																		w2 = BitmapInfo[1].width - (w1 + BitmapInfo[3].x)
																		AvgFreeWidth = (BitmapInfo[1].width - BitmapInfo[3].x) * 0.5
																		if w2 >= w1 then
																			ObjectOffsetW = (AvgFreeWidth - w1) * PixelSize
																		else
																			ObjectOffsetW = -(AvgFreeWidth - w2) * PixelSize
				
																		h1 = BitmapInfo[2].y
																		h2 = BitmapInfo[1].height - (h1 + BitmapInfo[3].y)
																		AvgFreeHeight = (BitmapInfo[1].height - BitmapInfo[3].y) * 0.5
																		if h2 >= h1 then
																			ObjectOffsetV = (AvgFreeHeight - h1) * PixelSize
																		else
																			ObjectOffsetV = -(AvgFreeHeight - h2) * PixelSize
				
																		if debug do
																			(
																				format "w1:% w2:% h1:% h2:%\n" w1 w2 h1 h2
																				format "Height Offset:%\nWidth Offset:%\nAvg Free: % %\n" ObjectOffsetV ObjectOffsetW AvgFreeWidth AvgFreeHeight
																			)
				
																		NewHeight = BoxDimension as float * HeightScalar
																		NewWidth = BoxDimension as float * WidthScalar
																		b = box()
																		b.width = NewWidth
																		b.height = BoxDimension
																		b.length = NewHeight
																		b.pos = NewCamera.target.pos
																		b.dir = -NewCamera.dir
																		if debug do
																			format "Camera Dir vector @ %: %\n" t -NewCamera.dir
																		b.mapcoords = true
																		convertToMesh b
																		for j = 1 to 4 do deleteVert b b.numverts
																		if NewCamera.dir.x < 0.0 do
																			in coordsys local rotate b 180 z_axis
																		in coordsys local rotate b 180 z_axis
																		in coordsys local move b [ObjectOffsetW, ObjectOffsetV, 0]
																		-- flip the UV's along U
																		if (getTvert b 4).x == 0.0 do
																		for j = 1 to b.numtverts do
																			(
																				CTvert = (getTvert b j)
																				if CTvert.x == 1.0 then
																					CTvert.x = 0.0
																				else
																					if CTvert.x == 0.0 do (CTvert.x = 1.0)
																				setTvert b j CTvert
																			)
																		update b
																		-- Calculate the new points in 3space via the uv coordinates
																		P0 = getVert b 2
																		P1 = getVert b 1
																		P2 = getVert b 4
																		P3 = getVert b 3
			
																		fVertArray = #()
			
																		for j = 1 to TVertArray.count do
																			(
																				Px = (P0.x + (TVertArray[j].x * (P1.x - P0.x)) + (TVertArray[j].y * (P2.x - P0.x)))
																				Py = (P0.y + (TVertArray[j].x * (P1.y - P0.y)) + (TVertArray[j].y * (P2.y - P0.y)))
																				Pz = (P0.z + (TVertArray[j].x * (P1.z - P0.z)) + (TVertArray[j].y * (P2.z - P0.z)))
																				append fVertArray [Px,Py,Pz]
																			)
																		iFaceArray = #([1,2,3])
																		for i = 3 to (Hullpoints.count - 1) do (append iFaceArray [1,i,i+1])
																		-- construct the mesh
																		delete b
																		b = mesh vertices:fVertArray faces:iFaceArray
																		if b.numverts > 16 do (format "Warning: more than 16 verts detected in convex hull object... please check and regenerate if needed...\n")
																		-- apply the texture coordinates
																		addmodifier b (UVWMap())
																		convertToMesh b
																		for i = 1 to b.numfaces do
																			(
																				iFace = getFace b i
																				iTFace = getTVFace b i
																				setTvert b iTFace.x (TVertArray[iFace.x])
																				setTvert b iTFace.y (TVertArray[iFace.y])
																				setTvert b iTFace.z (TVertArray[iFace.z])
																			)
																		-- Set the Edge visibility
																		for i = 1 to b.numfaces do
																			(
																				if i == 1 do setEdgeVis b i 3 off
																				if i == b.numfaces do setEdgeVis b i 1 off
																				if i > 1 and i < b.numfaces do
																					(
																						SetEdgeVis b i 1 off
																						SetEdgeVis b i 3 off
																					)
																			)
																		update b

																		df = #()
																		for i = 1 to b.numfaces do
																			(
																				Cface = getface b i
																				n = 4
																				NewX = rndP3 (getvert b Cface.x) n
																				NewY = rndP3 (getvert b Cface.y) n
																				NewZ = rndP3 (getvert b Cface.z) n
																				if (Thresh NewX NewY 0.001) do 
																					if findItem df i == 0 do (append df i)
															
																				if (Thresh NewX NewZ 0.001) do
																					if findItem df i == 0 do (append df i)
															
																				if (Thresh NewZ NewY 0.001) do
																					if findItem df i == 0 do (append df i)
			
																				-- check for a degenerate normal
																				if (GetFaceNormal b i) == [0,0,0] do	
																					if findItem df i == 0 do (append df i)
																			)
			
																		if df.count > 0 do 
																			(
																				format "found degenerate polygons... deleting them\n"
																				for i = df.count to 1 by -1 do
																					deleteFace b df[i]
																				update b
																			)
																		
																		-- Check for Stray Vertices
																		StrayVertArray = (for i = 1 to b.numverts collect i)
																		UsedVerts = #()
																		NewVertSelection = #()
															
																		for i = 1 to b.numfaces do
																			(
																				CFace = (getface b i)
																				if (findItem UsedVerts CFace.x) == 0 do (append UsedVerts CFace.x)
																				if (findItem UsedVerts CFace.y) == 0 do (append UsedVerts CFace.y)
																				if (findItem UsedVerts CFace.z) == 0 do (append UsedVerts CFace.z)
																			)
															
																		for i = 1 to StrayVertArray.count do
																			if (findItem UsedVerts i) == 0 do (append NewVertSelection i)
			
																		if NewVertSelection.count > 0 then
																			(
																				format "Stray vertices were found... deleting them\n"
																				for i = NewVertSelection.count to 1 by -1 do
																					deleteVert b NewVertSelection[i]
																				update b
																			)

																		if debug do format "Geometry done!\n"
				
																		-- ************************************************************
																		-- ** Generate a new material
																		-- ************************************************************
																		-- make sure all face IDs are set to 1
																		if debug do
																			format "Building new material...\n"
																		for j = 1 to b.numfaces do setFaceMatID b j 1
																		NMat = standardMaterial()
																		-- give it a random color
																		NMat.diffuse = random (color 20 20 20) (color 220 220 220)
																		NMat.diffuseMap = bitmaptexture()
																		NMat.OpacityMap = bitmaptexture()
																		-- specify the bitmap names
																		NMat.diffuseMap.filename = FinalTexName
																		NMat.OpacityMap.filename = FinalOpName
																		NMat.shinestrength = 0.0
																		NMat.name = substring OutTexName 1 (OutTexName.count - 3)
																		NMat.selfIllum = 100
																		b.mat = NMat
																		if debug do format "Finished building new material!\n"
				
																		b.wirecolor = scaleColor cw 0.3
																		b.name = ("$" + (substring CObj.name 1 (CObj.name.count - 4)) + "5-00")
																		b.pivot = CObj.pivot
																		if debug != true do
																			(
																				delete NewCamera
																				NewCamera = undefined
																			)
																		CObj.mat = OriginalMat
																	)
															) else (
																if renderSeperate.checked do MessageBox "No Objects Defined!"
															)
														gc() -- collect some garbage
													)
											)
											-- Lonnie, comment the next line out "--"
											-- if SingleObject == true do delete CTrunk
											delete CTrunk
											if CBranches != undefined do delete CBranches
										-- unhide everything at the end...
										for o in objects do unhide o
										CTrunk = CBranches = undefined
									) else (
										MessageBox "You must initialize the objects first"
									)
							)
					)

				on DRRollout open do
					(
						MagicIndex = undefined
						offsetHeight = false
						HeightOffset = 0.0
					)
			)

		-- ******************************************************************************************************************************
		-- * Utility Open
		-- ******************************************************************************************************************************
		on DetailReduction open do
			(
				if flip == undefined do
					global flip = false
				if (getfiles "C:\DR.txt").count == 0 then
					(
						if (WorkingFolder = (GetSavePath caption:"Select the location of a temporary folder to use for keeping texturemaps")) != undefined do
							(
								f = createfile "C:\DR.txt"
								openfile "DR.txt"
								WorkingFolder = (WorkingFolder + "\\")
								format ("Working Location: " + WorkingFolder) to:f
								close f
								format "Working Folder initialized to: %\n" WorkingFolder
							)
					) else (
						f = openfile "C:\DR.txt"
						CLine = readline f
						for i = 1 to CLine.count do
							(
								if cLine[i] == ":" do
									(
										WorkingFolder = (substring CLine (i + 2) (CLine.count - i - 1))
										if substring WorkingFolder WorkingFolder.count 1 != "\\" do
											WorkingFolder = WorkingFolder + "\\"
										format "Working Location initialized to: %\n" WorkingFolder
										Exit
									)
							)
						close f
					)

				Flt = NewRolloutFloater "Detail  Reduction v 1.5.2" 400 745 800 200
				addrollout DRRollout Flt
				gc()
			)

		on DetailReduction close do
			closeRolloutFloater Flt

		label DRLabel1 "Detail Reduction"
		label DRLabel2 "v 1.5.2, 7.14.98"
	)