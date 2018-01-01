Utility FortuneTeller "Fortune Teller"
	(
		local fortunes
		
		button GetFortune "Tell Fortune" width:140
		
		on FortuneTeller open do
			(
				fortunes = #()
				f = openfile "J:\\Design\\Fortunes\\fortunes.txt"
				while not eof f do
					append fortunes (readline f)
				close f
			)

		on Getfortune pressed do
			(
				Index = random 1 fortunes.count
				messagebox fortunes[Index] title:"Fortune Teller"
			
			)
	)