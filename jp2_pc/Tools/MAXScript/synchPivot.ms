Utility SynchPivots "Synch Pivots"
	(
		local p3Pivot

		label PivotLabel "Pivot:" align:#left offset:[-7,0]
		pickbutton GetPivot width:120 align:#right offset:[8,-20]
		
		button MatchSelection "Match Selection" width:140
		button MatchPosSelection "Match Position Selection" width:140
		
		on GetPivot picked obj do
			(
				p3Pivot = obj.pivot	
				GetPivot.text = obj.name
			)

		on MatchSelection pressed do
			for obj in selection do
				obj.pivot = p3Pivot

		on MatchPosSelection pressed do
			for obj in selection do
				obj.pos = p3Pivot

	)