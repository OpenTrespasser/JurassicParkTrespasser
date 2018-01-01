Utility RGBTools "RGB Tools"
(

colorpicker clr "Color:"
group ""
(
label label0 "==> 255" align:#left offset:[10,0]
label label1 "==> 1.0" align:#right offset:[-10,-18]
spinner r "R:" range:[0,255,128] align:#left fieldwidth:45 type:#integer
spinner r1 "R:" range:[0,1,.5] align:#right fieldwidth:45 offset:[0,-20] type:#float
spinner g "G:" range:[0,255,128] align:#left fieldwidth:45 type:#integer
spinner g1 "G:" range:[0,1,.5] align:#right fieldwidth:45 offset:[0,-20] type:#float
spinner b "B:" range:[0,255,128] align:#left fieldwidth:45 type:#integer
spinner b1 "B:" range:[0,1,.5] align:#right fieldwidth:45 offset:[0,-20] type:#float
label blank
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