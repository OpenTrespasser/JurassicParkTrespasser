Utility MorphUtil "Morph"
(
local SourceObj, DestObj
group ""
	(
		label MorphTOLabel "Morph To:" align:#left
		pickbutton GetSourceObject width:70 align:#right offset:[0,-20]
		label MorphFromLabel "Morph From:" align:#left
		pickbutton GetDestinationObject width:70 offset:[0,-20] align:#right
	)
button MorphObjects "Morph" width:140

on GetSourceObject picked obj do
	(
		SourceObj = obj
		GetSourceObject.text = SourceObj.name
	)

on GetDestinationObject picked obj do
	(
		DestObj = obj
		GetdestinationObject.text = DestObj.name
	)

on MorphObjects pressed do
	(
		if DestObj != undefined and SourceObj != undefined do
			(
				undo on
					(
						DestObjectPos = DestObj.pos
						DestObj.pos = SourceObj.pos
						Snv = SourceObj.numverts
						SourceVerts = for i = 1 to Snv collect (getvert SourceObj i)
						for i = 1 to Snv do
							Setvert DestObj i SourceVerts[i]
						DestObj.pos = DestObjectPos
						update DestObj
					)
			)
	)

)