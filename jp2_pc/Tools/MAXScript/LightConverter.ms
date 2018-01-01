Utility LightConverter "Light Converter"
(

button ProcessScene "Process Lights" width:140
label blank0
group "Results"
(
label results1 align:#left
label results2 align:#left
)

on ProcessScene pressed do
	(
		MAX select None
		for o in objects do
			(
				if classof o == omnilight do (selectmore o)
				if classof o == TargetSpot do (selectmore o)
				if classof o == FreeSpot do (selectmore o)
			)	
		ValidLightArray = for obj in selection collect obj
		LightCount = ValidLightArray.count
		PointCount = 0
		DirectionalCount = 0
		for i = 1 to LightCount do
			(
			CLight = ValidLightArray[i]
			if classof CLight == omnilight then
				(
					s = sphere name:("PLight_" + CLight.name)
					s.pos = CLight.pos
					s.segs = 8
					Intensity = CLight.Multiplier
					if Intensity > 1.0 do (Intensity = 1.0)
					s.radius = Intensity
					PointCount = PointCount + 1
				) else (
					s = sphere name:("DLight_" + CLight.name)
					s.pos = CLight.pos
					s.segs = 8
					Intensity = CLight.Multiplier
					if Intensity > 1.0 do (Intensity = 1.0)
					s.radius = Intensity
					s.dir = -CLight.dir
					DirectionalCount = DirectionalCount + 1
				)
			)
results1.text = (PointCount as string + " Point Lights")
results2.text = (DirectionalCount as string + " Directional Lights")
	)
)