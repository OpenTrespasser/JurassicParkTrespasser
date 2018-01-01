Utility TestScene "Test Scene"
(

local ObjectsArray;

label version "v1.0"
group "Options"
(
checkbox SelNonObjects "Sel Non-Objects"
checkbox SelectDegenerateFaces "Sel Degenerate Faces"
checkbox SelectOutOfRangeIDs "Sel Out of Range Mat ID's"
)
radiobuttons affectwhat "Process:" labels:#("Entire Scene", "Selected Objects") offset:[-20,0]
button go "Go!" width:140

on go pressed do
(
if affectwhat.state == 1 do
	(	
		ObjectsArray = for o in objects collect o
	)
if affectwhat.state == 2 do
	(
		ObjectsArray = (for obj in selection collect obj)
	)

Objcount = ObjectsArray.count
if SelNonObjects.checked == true do
	(
	max select none
	for o in objects do
		(
			if o.numfaces == 0 do (selectmore o)
		)
	)
	
for i = 1 to ObjCount do
	(
	if SelectDegenerateFaces.checked == true do
		(
			o = ObjectsArray[i]
			ConvertToMesh o
			setfaceselection o #()
			BadFaces = False
			for i = 1 to o.numfaces do
				(
					currentface = getface o i
					NewX = getvert o currentface.x
					NewY = getvert o currentface.y
					NewZ = getvert o currentface.z
					if NewX == NewY do 
						(
							setfaceselection o #(i) keep:true
							BadFaces = true
						)
					if NewX == NewZ do
						(
							setfaceselection o #(i) keep:true
							BadFaces = true
						)

					if NewZ == NewY do
						(
							setfaceselection o #(i) keep:true
							BadFaces = true
						)
				)

			if BadFaces == false do
				(
					MessageBox "No Bad Faces Found!"
				)
			if badfaces == true do
				(
					MessageBox "Bad Faces Found!\n Bad Faces are current Sub-Object Selection"
				)
		)
	if SelectOutOfRangeIDs.checked == true do
		(
			o = ObjectsArray[i]
			ConvertToMesh o
			if classof o.mat == multiMaterial do
				(
					NumMats = o.mat.numsubs
					nf = o.numfaces
					setfaceselection
					BadMatIDs = false
					for i = 1 to nf do
						(
						if (getfacematid o i) > NumMats do
							(
								setfaceselection o #(i) keep:true
								BadMatIDs = true
							)
						)
					if BadMatIDs == true do
						(
							messageBox "Out of Range Material ID's found!\n Faces are set as current face selection"
						)
				)
		)
	)
)

) -- end