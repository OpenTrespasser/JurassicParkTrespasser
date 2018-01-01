Utility noshine "Shine on, Shine on"
(
spinner Shininess_Strength "ShineStrength" range:[0,100,0] type:#integer
button un_Shine "Set Selected ShineStrength"

on un_shine pressed do
(
	for o in objects do
	(
		if o.isselected do
		(
			if classof o.mat == standardmaterial do (o.mat.shinestrength = Shininess_Strength.value)
			if classof o.mat == multimaterial do (for i = 1 to o.mat.numsubs do (o.mat[i].shinestrength = Shininess_Strength.value))
		)
	)
)
)