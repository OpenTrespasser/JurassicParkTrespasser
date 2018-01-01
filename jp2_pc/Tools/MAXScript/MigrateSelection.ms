-- MigrateSelection.ms
-- by Kyle McKisic
-- kmckisic@dreamworks.com

Utility MigrateSelection "Migrate Selection"
(
group "Migration Options"
(
checkbox ClearSel "Keep initial selection"
radiobuttons fromSel "From:" labels:#("Vertices", "Faces") columns:1 offset:[-40,0] default:1
radiobuttons toSel "To:" labels:#("Vertices", "Faces") columns:1 offset:[40,-50] default:2
label methodlabel "Method:" align:#left
CheckButton MethodWindow "Window" offset:[-37,0] width:70
CheckButton MethodCrossing "Crossing" offset:[37,-26] width:70 default:checked
pickButton PickObject "Pick Object" width:140
)

group "Other Selection Tools"
(
label affects "Affects:" align:#left
radiobuttons SelToolsType labels:#("Verts", "Faces")
pickbutton InvertSelection "Invert Selection" width:140
)

on InvertSelection picked obj do
(
o = obj
if SelToolsType.state == 1 do
	(
		VertselArray = getvertselection o
		if VertSelArray.count == 0 then 
			(
			MessageBox "No Vertices Selected!"
			) else (
			nv = o.numverts
			setvertselection o #()
			for i = 1 to nv do 
				(
				if finditem vertSelArray i == 0 do setvertselection o #(i) keep:true
				)
			)
	)

if SelToolsType.state == 2 do
	(
		FaceSelArray = getFaceSelection o
		if FaceSelArray.count == 0 then 
			(
			MessageBox "No Faces Selected!"
			) else (
			nf = o.numfaces
			setFaceSelection o #()
			for i = 1 to nf do 
				(
				if finditem FaceSelArray i == 0 do setFaceSelection o #(i) keep:true
				)
			)
	)
)


on MigrateSelection open do 
	(
	MethodCrossing.checked = true
	)


on MethodWindow changed state do
	(
	if MethodWindow.state == on then 
		(MethodCrossing.state = off)
		else
		(MethodCrossing.state = on)
	)

on MethodCrossing changed state do
	(
	if MethodCrossing.state == on then 
		(MethodWindow.state = off)
		else 
		(MethodWindow.state = on)
	)


on fromSel changed state do 
(
if fromSel.state == 1 do 
	(
	toSel.state = 2
	MethodWindow.enabled = true
	MethodCrossing.enabled = true
	MethodCrossing.checked = true
	)
if fromSel.state == 2 do 
	(
	toSel.state = 1
	MethodWindow.checked = false
	MethodWindow.enabled = false
	MethodCrossing.checked = false
	MethodCrossing.enabled = false
	)
)

on ToSel changed state do 
(
if ToSel.state == 1 do 
	(
	FromSel.state = 2
	MethodWindow.checked = false
	MethodWindow.enabled = false
	MethodCrossing.checked = false
	MethodCrossing.enabled = false
	)
if ToSel.state == 2 do 
	(
	FromSel.state = 1
	MethodWindow.enabled = true
	MethodCrossing.enabled = true
	MethodCrossing.checked = true
	)
)


On PickObject picked obj do
(
	o = obj
	undo on (
	
	if FromSel.state == 1 do
		(
			if MethodCrossing.checked == true then 
				(
					VertSel = getvertselection o
					AllFaces = (for i = 1 to o.numfaces collect (getface o i))
					VSc = VertSel.count
					AFc = AllFaces.count
					if ClearSel.checked == false do (setfaceselection o #())
					for i = 1 to VSc do
						(
							for j = 1 to AFc do
								(
									if VertSel[i] == (Allfaces[j]).x do (setfaceselection o #(j) keep:true)
									if VertSel[i] == (Allfaces[j]).y do (setfaceselection o #(j) keep:true)
									if VertSel[i] == (Allfaces[j]).z do (setfaceselection o #(j) keep:true)
								)
						)
					update o
				) 
					else 
				(
					VertSel = getvertselection o
					AllFaces = (for i = 1 to o.numfaces collect (getface o i))
					AFc = AllFaces.count
					if ClearSel.checked == false do (setfaceselection o #())
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
)
	
	if FromSel.state == 2 do
		(
			faceSel = getfaceselection o
			NewVertSel = (for i = 1 to faceSel.count collect (getface o faceSel[i]))
			NVSc = NewVertSel.count
			if ClearSel.checked == false do (setvertselection o #())
			for i = 1 to NVSc do 
				(
					setvertselection o #((NewVertSel[i]).x) keep:true
					setvertselection o #((NewVertSel[i]).y) keep:true
					setvertselection o #((NewVertSel[i]).z) keep:true
				)
			update o
		)
))
)