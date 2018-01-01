Utility ObjScale "Object Scale"
	(
		pickButton PickObject "Pick Object"
		
		on PickObject picked obj do
			(
				ObjMatrixScale = obj.scale
				ObjMatrixScaleString = ("X:" + ObjMatrixScale.x as string + "\nY:" + ObjMatrixScale.y as string + "\nZ:" + ObjMatrixScale.z as string + "\n\n")
				ObjScale = obj.max - obj.min
				ObjScaleString = ("X:" + ObjScale.x as string + "\nY:" + ObjScale.y as string + "\nZ:" + ObjScale.z as string + "\n")
				Messagebox ("\"" + obj.name + "\"\n\nMatrix Scale:\n" + ObjMatrixScaleString + "Physical Scale:\n(Axis Aligned)\n" + ObjScaleString + "\n99% Fat Free\nand oh so tasty\n\"Ted Lite\"") title:"Ted's Scale Info"
			)
	
	
	)