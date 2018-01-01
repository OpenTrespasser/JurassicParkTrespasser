-- Combine.ms

fn FindPoint3Item Point3Item Point3Array =
(
local returnIndex = -1
while returnIndex == -1 do
	(
	for i = 1 to Point3Array.count do 
		(
		ArrayItem = Point3Array[i]
		if Point3Item == ArrayItem do (returnIndex = i)
		if returnIndex != -1 then Exit
		)
	if returnIndex == -1 do (returnIndex = 0)
	)
return returnIndex
)

fn VertsToFaces obj =
(
o = obj
VertSel = getvertselection o
AllFaces = (for i = 1 to o.numfaces collect (getface o i))
AFc = AllFaces.count
setfaceselection o #()
for j = 1 to AFc do
(
	Cnt = 0
	if (findItem VertSel (AllFaces[j]).x) != 0 do (Cnt = Cnt + 1)
	if (findItem VertSel (AllFaces[j]).y) != 0 do (Cnt = Cnt + 1)
	if (findItem VertSel (AllFaces[j]).z) != 0 do 
		(
		Cnt = Cnt + 1
		if Cnt == 3 do (setfaceselection o #(j) keep:true)
		)
	)
update o
)

fn VertsToFacesCrossing obj =
(
o = obj
VertSel = getvertselection o
AllFaces = (for i = 1 to o.numfaces collect (getface o i))
VSc = VertSel.count
AFc = AllFaces.count
setfaceselection o #()
	for i = 1 to VSc do
	(
		for j = 1 to AFc do
		(
		if VertSel[i] == (Allfaces[j]).x do (setfaceselection o #(j) keep:true)
		if VertSel[i] == (Allfaces[j]).y do (setfaceselection o #(j) keep:true)
		if VertSel[i] == (Allfaces[j]).z do (setfaceselection o #(j) keep:true)
))
update o
)


fn detach_selected_faces m =
(
	ofs = getFaceSelection m
	ofvs = for f in ofs collect getFace m f
	vmap = #()
	i = 0
	for f in ofvs do
	(if vmap[f.x] == undefined do vmap[f.x] = (i += 1)
	if vmap[f.y] == undefined do vmap[f.y] = (i += 1)
	if vmap[f.z] == undefined do vmap[f.z] = (i += 1))
	nv = #()
	for i in 1 to vmap.count do
	if vmap[i] != undefined do nv[vmap[i]] = getVert m i
	nf = for f in ofvs collect [vmap[f.x], vmap[f.y], vmap[f.z]]
   NewMesh = mesh name:(m.name + "-ToCombine") vertices:nv faces:nf 
   NewMesh.pivot = NewMesh.center
)

Utility CombineMesh "Combine Mesh"
(

local PrimObjVertArray, PrimObj, PrimNV, SecndNV, SecndObj, SecndObjVertArray, Cnt, 
PrimaryObjIndexArray, SecondaryObjIndexArray, CVertex, returnIndex, Pcount;

label ver "v0.5"
group "Combine Objects Setup"
(
pickbutton GetPrimaryObject "Pick New Object" width:135
label PrimObjLbl "-- none specified --"
label PrimObjLbl2 ""
pickbutton GetSecondaryObject "Pick Original Object" width:135
label SecndObjLbl "-- none specified --"
label SecndObjLbl2 ""
)

group "Combine Options"
(
)

group "Debug"
(
-- label Debug1 ""
-- label Debug2 ""
-- label debug3 ""
)
Button combineObjects "Combine Objects" width:135

on GetPrimaryObject picked obj do
(
PrimObj = obj
PrimNV = PrimObj.numverts
PrimObjVertArray = (for i = 1 to PrimNV collect (getvert PrimObj i))
PrimObjLbl.text = PrimObj.name
PrimObjLbl2.text = (PrimNV as string + " Vertices")
)

on GetSecondaryObject picked obj do
(
SecndObj = obj
SecndNV = SecndObj.numverts
SecndObjVertArray = (for i = 1 to SecndNV collect (getvert SecndObj i))
SecndObjLbl.text = SecndObj.name
SecndObjLbl2.text = (SecndNV as string + " Vertices")
)

On combineObjects pressed do
(
SecondaryObjIndexArray = #() -- initialize Array
PrimaryObjIndexArray = #() -- initialize Array
Cnt = 0
Pcount = 0
for i = 1 to SecndNV do
	(
	SecndPoint3 = SecndObjVertArray[i]
	rtrnIndex = FindPoint3Item SecndPoint3 PrimObjVertArray
	if rtrnIndex != 0 do
		(
		Cnt = Cnt + 1
		SecondaryObjIndexArray[Cnt] = rtrnIndex
		)
	)

for i = 1 to PrimNV do
	(
	PrimPoint3 = PrimObjVertArray[i]
	rtrnIndex = FindPoint3Item PrimPoint3 SecndObjVertArray
	if rtrnIndex == 0 do
		(
		Pcount = Pcount + 1
		PrimaryObjIndexArray[Pcount] = i
		)
	)
SetVertselection SecndObj SecondaryObjIndexArray
SetVertselection PrimObj PrimaryObjIndexArray
VertsToFaces SecndObj
VertsToFacesCrossing PrimObj
Update SecndObj
Update PrimObj
detach_selected_faces SecndObj
detach_selected_faces PrimObj
)
)

