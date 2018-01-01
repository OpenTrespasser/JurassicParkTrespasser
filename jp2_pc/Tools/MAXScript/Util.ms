utility Trespasser_Utilities "Trespasser Utilities"
(

local surface;
label version "v1.0"

group "Object Placement Tools"
(
button Rotate_selected "Random Rotate Selected" width:140
button Drop_pivot "Drop Pivot Point" width:140
)

on Rotate_selected pressed do
(undo on (for o in objects do 
(if o.isselected do (rotate o (random -180.0 180.0 ) z_axis))))

on Drop_pivot pressed do 
	(undo on (
		for o in objects do 
			(
			if o.isselected do 
				(
				o.pivot = ((o.max + o.min) /2)
				drop_amount = ((o.max.z-o.min.z) /2)
				o.pivot = [o.pivot.x, o.pivot.y, (o.pivot.z - drop_amount)]
				))))
		
)
































































































