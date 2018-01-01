Utility Ht "Height"
	(
		pickbutton GetHeight "Get Height"

		on GetHeight picked obj do
			messageBox (((obj.max.z - obj.min.z) as string) + " M")
	)