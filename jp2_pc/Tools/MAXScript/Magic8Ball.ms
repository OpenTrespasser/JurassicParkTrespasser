Utility Magic8Ball "Magic 8 Ball"
	(
		local replies = #("K:\\Art\\Users\\Kyle\\8Ball\\reply1.bmp",\
											"K:\\Art\\Users\\Kyle\\8Ball\\reply2.bmp",\
											"K:\\Art\\Users\\Kyle\\8Ball\\reply3.bmp",\
											"K:\\Art\\Users\\Kyle\\8Ball\\reply4.bmp",\
											"K:\\Art\\Users\\Kyle\\8Ball\\reply5.bmp",\
											"K:\\Art\\Users\\Kyle\\8Ball\\reply6.bmp",\
											"K:\\Art\\Users\\Kyle\\8Ball\\reply7.bmp",\
											"K:\\Art\\Users\\Kyle\\8Ball\\reply8.bmp",\
											"K:\\Art\\Users\\Kyle\\8Ball\\reply9.bmp",\
											"K:\\Art\\Users\\Kyle\\8Ball\\reply10.bmp",\
											"K:\\Art\\Users\\Kyle\\8Ball\\reply11.bmp",\
											"K:\\Art\\Users\\Kyle\\8Ball\\reply12.bmp",\
											"K:\\Art\\Users\\Kyle\\8Ball\\reply13.bmp",\
											"K:\\Art\\Users\\Kyle\\8Ball\\reply14.bmp",\
											"K:\\Art\\Users\\Kyle\\8Ball\\reply15.bmp",\
											"K:\\Art\\Users\\Kyle\\8Ball\\reply16.bmp",\
											"K:\\Art\\Users\\Kyle\\8Ball\\reply17.bmp",\
											"K:\\Art\\Users\\Kyle\\8Ball\\reply18.bmp",\
											"K:\\Art\\Users\\Kyle\\8Ball\\reply19.bmp",\
											"K:\\Art\\Users\\Kyle\\8Ball\\reply20.bmp")

		bitmap Ball filename:"K:\\Art\\Users\\Kyle\\8Ball\\8Ball.bmp" offset:[-20,0]
		label label2
		label label3
		label label4 "Ask a yes or no question"
		
		button AskQuestion "Answer Me!" width:140

		bitmap reply filename:"K:\\Art\\Users\\Kyle\\8Ball\\blank.bmp" offset:[0,0] width:120 height:120
		label label5
		
		on AskQuestion pressed do
			(
				Index = random 1 20
				reply.filename = replies[Index]
			)
	)