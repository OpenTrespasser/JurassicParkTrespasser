Utility PpURatio "Pixel/Unit Ratio"
(
spinner NumPixels "Pixels:" range:[2,4096,64] type:#integer fieldwidth:45
spinner Meters "Meters:" range:[0,512,1] type:#float fieldwidth:45
label blank0
spinner PpU "Pixels per M:" range:[0,32768,64] type:#integer fieldwidth:45
radiobuttons affectwhat labels:#("Pixels", "Meters")
label blank

on NumPixels changed state do
	(
		PpU.value = (NumPixels.value as float / Meters.value)
	)
on Meters changed state do
	(
		PpU.value = (NumPixels.value as float / Meters.value)
	)

on PpU changed state do
	(
	
	if affectwhat.state == 1 do
		(
			NumPixels.value = (PpU.value * Meters.value)
		)
	if affectwhat.state == 2 do
		(
			Meters.value = (NumPixels.value as float / PpU.value)
		)
	)

)