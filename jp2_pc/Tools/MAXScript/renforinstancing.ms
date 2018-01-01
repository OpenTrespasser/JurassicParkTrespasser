Utility RenObjectsForInstancing "Rename for Instancing"
(
label version "v1.0"
label label1 "1. Select source object." align: #left
label label2 "2. Select objects for renaming." align: #left
label label3 "3. Click Rename Selected" align: #left
label labelblank0 ""
label label4 "NOTE:"
label label4a "Source Object SHOULD NOT"
label label5 "contain the -00 extension!"
label labelblank ""
pickbutton PickObject "Pick Source Object" width:135
Label Source_name "-- none --"
label label6 ""
button RenameSelected "Rename Selected" width:135
button RenameSelected2 "Rename Selected2" width:135


on PickObject Picked obj do
	(
		Source_name.text = obj.name
	)

on renameselected pressed do 
	(
	i = 0;
	
		for o in objects do
			(
			if o.isselected do
				(
				i = (i +1)
				if i >= 1 and i <= 9 do
				(object_count = ("0" + (i as string)))
				if i >= 10 do
				(object_count = (i as string))
				o.name = (Source_name.text + "-" + object_count)
				)
			)
	)

on renameselected2 pressed do 
	(
	i = 0;
	
		for o in objects do
			(
			if o.isselected do
				(
				i = (i +1)
				if i >= 1 and i <= 9 do
				(object_count = ("0" + (i as string)))
				if i >= 10 do
				(object_count = (i as string))
				o.name = (Source_name.text + object_count)
				)
			)
	)

)
