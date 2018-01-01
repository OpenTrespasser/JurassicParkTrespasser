Utility DummyMaterial "Dummy Material"
(
	group "Options"
	(
		radiobuttons ChooseColor labels:#("Dummy Map", "Occlusion") offset:[0,0] align:#left
		Checkbox AssignUVs "Assign UVW" checked:true offset:[0,0]
	)

	button AssignMat "Assign Material" width:140

	on AssignMat Pressed do
		(
			MaterialA = StandardMaterial()
			MaterialA.name = "Dummy Material"
			MaterialA.shinestrength = 0
			MaterialA.diffusemap = bitmaptexture()
			if choosecolor.state == 1 do (MaterialA.diffusemap.filename = "\\\Jpii_pc\\Trespass\Art\PDummyMapt2.bmp")
			if choosecolor.state == 2 do (MaterialA.diffusemap.filename = "\\\Jpii_pc\\trespass\Art\Soccludet2.bmp")
			ObjectArray = for obj in selection collect obj
			oc = ObjectArray.count
			for i = 1 to oc do
				(
					o = ObjectArray[i]
					o.mat = MaterialA
					if AssignUVs.checked == true do
						(
							ConvertToMesh o
							addmodifier o (UVWMap())
							o.modifiers[1].maptype = 4
							ConvertToMesh o
						)
				)
			for i = 1 to oc do
				selectmore ObjectArray[i]
		)
)