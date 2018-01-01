Utility UVManager "UV Manager"
(
pickbutton gatherInfo "Gather UV Information"
checkbox SelFaces "Selected Faces"

on gatherInfo picked obj do
	(
		CObj = obj
		nf = CObj.numfaces
		format "%\n" CObj.name
		for i = 1 to nf do
			(
				format "Face Index: %\n" i
				Cface = (getface CObj i)
				format "\tVert: % %\n" CFace.x (gettvert CObj CFace.x)
				format "\tVert: % %\n" CFace.y (gettvert CObj CFace.y)
				format "\tVert: % %\n" CFace.z (gettvert CObj CFace.z)
			)
	)
)