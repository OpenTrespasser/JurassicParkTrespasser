fn DetachSelectedFaces m =
(
	ofs = getFaceSelection m					-- original selected faces
	ofvs = for f in ofs collect getFace m f  	-- original face verts
	-- build an old-to-new vertex map
	vmap = #()
	i = 0
	for f in ofvs do
		(
			if vmap[f.x] == undefined do vmap[f.x] = (i += 1)
			if vmap[f.y] == undefined do vmap[f.y] = (i += 1)
			if vmap[f.z] == undefined do vmap[f.z] = (i += 1)
		)
	-- build the new vertex coord array
	nv = #()
	for i in 1 to vmap.count do
		if vmap[i] != undefined do nv[vmap[i]] = getVert m i
	-- build the new face array
	nf = for f in ofvs collect [vmap[f.x], vmap[f.y], vmap[f.z]]
	moo = mesh vertices:nv faces:nf
	addmodifier moo (UVWMap())
	convertToMesh moo
	return moo
)

Utility BObjects "Bounding Objects"
(
	group "Options"
		(
			radiobuttons ObjectType "Bounding Object Type:" labels:#("Box", "Sphere", "Cylinder") columns:1 offset:[-40,0]
			spinner SphereSegs "Segs:" range:[4,200,16] type:#Integer enabled:false fieldwidth:35 offset:[0,-25]
			checkbox dropPivot "Drop Pivot"
			radioButtons DeriveFrom "Bound From:" labels:#("Entire Object", "Face Selection") columns:1 offset:[-20,10]
			checkbox NameAsPhysics "Name as Phys SubModel"
		)
	
	pickbutton PickObj "Pick Object" width:140

	-- make sure the Segs spinner is disabled when the user has Box type selected

	on ObjectType changed state do
		if state == 1 then SphereSegs.enabled = false else SphereSegs.enabled = true

	on PickObj picked obj do
		(
			sourceObj = obj
	
			-- check to see if we're using a face selection
			if DeriveFrom.state == 2 do 
				(
					sourceObj = (DetachSelectedFaces obj)
					if NameAsPhysics.checked then
						sourceObj.name = Obj.name
					else
						sourceObj.name = (Obj.name + "_SubMesh")
				)
			-- we must have an editable mesh!
			sourcePivot = sourceObj.pivot
			sourceObj.pivot = sourceObj.center
		
			-- if we're creating a box...
			if ObjectType.state == 1 do 
				(
					b = box height:((sourceObj.max).z - (SourceObj.min).z) width:((sourceObj.max).x - (SourceObj.min).x) length:((sourceObj.max).y - (SourceObj.min).y)
					if NameAsPhysics.checked then
						b.name = ("$F" + SourceObj.name)
					else
						b.name = (SourceObj.name + "_Bounding Box")
					b.pivot = b.center
					b.pos = sourceObj.pos
					b.mapcoords = true
					if dropPivot.checked do
						(
							b.pivot = b.center
							drop_amount = ((b.max.z-b.min.z) /2)
							b.pivot = [b.pivot.x, b.pivot.y, (b.pivot.z - drop_amount)]
						)
				)
	
			-- if we're creating a sphere
			if ObjectType.state == 2 do 
				(
					nv = sourceObj.numverts
					SourceCenter = SourceObj.center
					LDistance = distance SourceCenter (getvert SourceObj 1)
	
					-- find the largest distance from the mesh center to any vertex in the source mesh
					for i = 1 to nv do
						(
							NewDistance = distance SourceCenter (getvert SourceObj i)
							if NewDistance > LDistance do (Ldistance = NewDistance)
						)
					s = sphere radius:Ldistance pos:SourceCenter
					s.name = (SourceObj.name + "_Bounding Sphere")
					s.segs = SphereSegs.value
					s.mapcoords = true
				)
	
			if ObjectType.state == 3 do 
				(
					ObjCenter = sourceObj.center
					nv = sourceObj.numverts
					Verts = for i = 1 to nv collect (getvert sourceObj i)
					-- first we find the vertex distance furthest from the center of the object
					FarVert = 0.0
					VertexDistances = #()
					-- we put all the vertex distances into an array, and find the one furthest from the center of the mesh
					for i = 1 to nv do
						(
							append VertexDistances (distance [ObjCenter.x, ObjCenter.y, Verts[i].z] Verts[i])
							if VertexDistances[i] > FarVert do FarVert = VertexDistances[i]
						)
					c = cylinder()
					c.radius = FarVert
					c.height = (sourceObj.max - sourceObj.min).z
					c.pivot = c.center
					c.sides = SphereSegs.value
					c.mapcoords = true
					c.pos = sourceObj.center
				)
	
			-- put the pivot point back where it was
			sourceObj.pivot = sourcePivot
			if DeriveFrom.state == 2 do delete sourceObj
		)
)