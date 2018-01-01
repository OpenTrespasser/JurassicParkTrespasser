filein "\\\JPII_PC\\Trespass\Art\\Tools\Scripts\HardReset.ms"

fn Volume2 obj =
(
	local v3Min = GetVert obj 1
	local v3Max = GetVert obj 1
	local MyVert = v3Max
	
-- returns the volume of the axis aligned bounding box around obj
	for i = 1 to obj.numverts do
	(
		MyVert = GetVert obj i
		if (v3Min.x > MyVert.x) do
			v3Min.x = MyVert.x
		if (v3Min.y > MyVert.y) do
			v3Min.y = MyVert.y
		if (v3Min.z > MyVert.z) do
			v3Min.z = MyVert.z
		
		if (v3Max.x < MyVert.x) do
			v3Max.x = MyVert.x
		if (v3Max.y < MyVert.y) do
			v3Max.y = MyVert.y
		if (v3Max.z < MyVert.z) do
			v3Max.z = MyVert.z
	)

--	extents = obj.max - obj.min;
	local extents = v3Max - v3Min
	return (extents.x * 1000) * (extents.y * 1000) * (extents.z * 1000);
)

fn Random11 = 
(
	return Random -1.0 1.0;
)

fn RandomQuaternion = 
(
	return (quat (random 0 180) [Random11(),Random11(),Random11()]);
)

fn RandomRotate obj = 
(
-- Randomly rotates obj, and returns the rotation.
	local quaternion = randomQuaternion();
	obj.rotation = quaternion;
	return quaternion;
)

fn BoxMeVol obj count = 
(
	obj = convertToMesh (copy obj)

	local rot = obj.rotation;
	local vsmall = volume2 obj;
	local qsmall = rot;

	-- temp variables
	local q = rot;
	local v = vsmall;

	for i = 1 to (count as integer) do
		(
			q = RandomRotate obj;
			v = volume2 obj;
			if v < vsmall do
			(
				vsmall = v;
				qsmall = q;
			)
		)
	delete obj
	return vsmall
)

Utility MeasurementTools "Measurement Tools"
(

local CObj, CObjVolume

group "Unit Conversion"
(
	spinner MetersSpinner "M:" range:[0,100000,0] type:#float offset:[-25,0]
	spinner FeetSpinner "Ft:" range:[0,100000,0] type:#float offset:[-25,0]
	spinner InchesSpinner "In:" range:[0,100000,0] type:#float offset:[-25,0]
	label foo ""
	spinner Pounds "lb:" range:[0,100000,0] type:#float offset:[-25,0]
	spinner Kilos "Kg: " range:[0,100000,0] type:#float offset:[-25,0]

)

group "Pixel per Unit Ratio"
(
	spinner NumPixels "Pixels:" range:[2,4096,64] type:#integer fieldwidth:45 align:#right offset:[-20,0]
	checkbutton PixelsChkBtn align:#right offset:[0,-23]
	spinner Meters "Meters:" range:[0,512,1] type:#float fieldwidth:45 align:#right offset:[-20,0]
	checkbutton MetersChkBtn align:#right offset:[0,-23]
	spinner PpU "Pixels per M:" range:[0,32768,64] type:#integer fieldwidth:45 align:#right offset:[-20,0]
	checkbutton PpUChkBtn align:#right offset:[0,-23] checked:true
)

group "RGB space conversion"
(
	colorpicker clr "Color:"
	label label0 "==> 255" align:#left offset:[10,0]
	label label1 "==> 1.0" align:#right offset:[-10,-18]
	spinner r "R:" range:[0,255,128] align:#left fieldwidth:45 type:#integer
	spinner r1 "R:" range:[0,1,.5] align:#right fieldwidth:45 offset:[0,-20] type:#float
	spinner g "G:" range:[0,255,128] align:#left fieldwidth:45 type:#integer
	spinner g1 "G:" range:[0,1,.5] align:#right fieldwidth:45 offset:[0,-20] type:#float
	spinner b "B:" range:[0,255,128] align:#left fieldwidth:45 type:#integer
	spinner b1 "B:" range:[0,1,.5] align:#right fieldwidth:45 offset:[0,-20] type:#float
	label blank1
)


group "Mass"
	(
		spinner Mass "Mass (kg): " range:[0, 999999, 0] fieldwidth:45 offset:[-20,0]
		checkbutton UseMass offset:[0,-23] align:#right
		spinner Density "D (g/cm3): " range:[0, 10, .5] fieldwidth:45 offset:[-20,0]
		checkbutton UseDensity offset:[0,-23] align:#right checked:true
		radiobuttons VolMethod labels:#("Axis Aligned", "Precise")
		spinner Precision "Precision:" range:[1,10000,1000] type:#integer enabled:false
		label PickObjLabel "Object: " align:#left
		pickbutton ChooseObject offset:[0,-20] align:#right width:100
	)

on Pounds changed state do
	(	
		Kilos.value = Pounds.value * 0.454
	)

on Kilos changed state do
	(	
		Pounds.value = Kilos.value / 0.454
	)

on ChooseObject picked obj do
	(
		CObj = obj
		ChooseObject.text = CObj.name
		if VolMethod.state == 1 then
			(
				CObjScale = CObj.max - CObj.min
				CObjScale = [(CObjScale.x * 1000), (CObjScale.y * 1000), (CObjScale.z * 1000)]
				CObjVolume = (CObjScale.x * CObjScale.y * CObjScale.z)
			) else (
				CObjVolume = BoxMeVol CObj Precision.value
			)

		if UseMass.checked then
			Mass.value = (Density.value * CObjVolume) / 1000000.0 as float
		else
			Density.value = ((Mass.value * 1000000.0 as float) / CObjVolume)
	)

	on VolMethod changed state do
		if VolMethod.state == 1 then
			Precision.enabled = false
		else
			Precision.enabled = true

	on Mass changed state do
		if CObjVolume != undefined do
			if UseMass.checked then
				Mass.value = (Density.value * CObjVolume) / 1000000.0 as float
			else
				Density.value = ((Mass.value * 1000000.0 as float) / CObjVolume)

	on Density changed state do
		if CObjVolume != undefined do
			if UseMass.checked then
				Mass.value = (Density.value * CObjVolume) / 1000000.0 as float
			else
				Density.value = ((Mass.value * 1000000.0 as float) / CObjVolume)
	
	on UseMass changed state do
		(
			if UseMass.checked then
				UseDensity.checked = false
			else
				UseDensity.checked = true
		)

	on UseDensity changed state do
		(
			if UseDensity.checked then
				UseMass.checked = false
			else
				UseMass.checked = true
		)
	
	on PixelsChkBtn changed state do
		(
			if PixelsChkBtn.checked then
				MetersChkBtn.checked = PpUChkBtn.checked = false
		)

	on MetersChkBtn changed state do
		(
			if MetersChkBtn.checked then
				PixelsChkBtn.checked = PpUChkBtn.checked = false
		)


	on PpUChkBtn changed state do
		(
			if PpUChkBtn.checked then
				PixelsChkBtn.checked = MetersChkBtn.checked = false
		)

	on NumPixels changed state do
		(
			if PixelsChkBtn.checked do
				NumPixels.value = (PpU.value as float * Meters.value as float)
			if MetersChkBtn.checked do
				Meters.value = (NumPixels.value as float / PpU.value as float)
			if PpUChkBtn.checked do
				PpU.value = (NumPixels.value as float / Meters.value as float)
		)
	
	on Meters changed state do
		(
			if PixelsChkBtn.checked do
				NumPixels.value = (PpU.value as float * Meters.value as float)
			if MetersChkBtn.checked do
				Meters.value = (NumPixels.value as float / PpU.value as float)
			if PpUChkBtn.checked do
				PpU.value = (NumPixels.value as float / Meters.value as float)
		)
	
	on PpU changed state do
		(
			if PixelsChkBtn.checked do
				NumPixels.value = (PpU.value as float * Meters.value as float)
			if MetersChkBtn.checked do
				Meters.value = (NumPixels.value as float / PpU.value as float)
			if PpUChkBtn.checked do
				PpU.value = (NumPixels.value as float / Meters.value as float)
		)



	on MetersSpinner changed state do
		(
			FeetSpinner.value = MetersSpinner.value * 3.28100
			InchesSpinner.value = MetersSpinner.value * 39.3700
		)
	
	on FeetSpinner changed state do
		(
			MetersSpinner.value = FeetSpinner.value * 0.304800
			InchesSpinner.value = FeetSpinner.value / 0.083300
		)
	
	on InchesSpinner changed state do
		(
			MetersSpinner.value = InchesSpinner.value * 0.025400
			FeetSpinner.value = InchesSpinner.value * 0.083300
		)


	on r changed state do
		(
		NewColor = [r.value, g.value, b.value] as color
		clr.color = NewColor
		r1.value = (NewColor.r / 255)
		g1.value = (NewColor.g / 255)
		b1.value = (NewColor.b / 255)
		)
	
	on g changed state do
		(
		NewColor = [r.value, g.value, b.value] as color
		clr.color = NewColor
		r1.value = (NewColor.r / 255)
		g1.value = (NewColor.g / 255)
		b1.value = (NewColor.b / 255)
		)
	
	on b changed state do
		(
		NewColor = [r.value, g.value, b.value] as color
		clr.color = NewColor
		r1.value = (NewColor.r / 255)
		g1.value = (NewColor.g / 255)
		b1.value = (NewColor.b / 255)
		)
	
	on r1 changed state do
	(
		NewColor = [r1.value, g1.value, b1.value]
		NewColor.x = (NewColor.x * 255) as integer
		NewColor.y = (NewColor.y * 255) as integer
		NewColor.z = (NewColor.z * 255) as integer
		clr.color = NewColor as color
		r.value = NewColor.x
		g.value = NewColor.y
		b.value = NewColor.z
	)
	
	on g1 changed state do
	(
		NewColor = [r1.value, g1.value, b1.value]
		NewColor.x = (NewColor.x * 255) as integer
		NewColor.y = (NewColor.y * 255) as integer
		NewColor.z = (NewColor.z * 255) as integer
		clr.color = NewColor as color
		r.value = NewColor.x
		g.value = NewColor.y
		b.value = NewColor.z
	)
	
	on b1 changed state do
	(
		NewColor = [r1.value, g1.value, b1.value]
		NewColor.x = (NewColor.x * 255) as integer
		NewColor.y = (NewColor.y * 255) as integer
		NewColor.z = (NewColor.z * 255) as integer
		clr.color = NewColor as color
		r.value = NewColor.x
		g.value = NewColor.y
		b.value = NewColor.z
	)


)