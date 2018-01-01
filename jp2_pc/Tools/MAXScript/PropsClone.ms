Utility PropsClone "Props Clone"
	(
		radiobuttons CopyType labels:#("Instance", "Copy")
		spinner offset "Offset in X:" range:[0,100,10]
		button CopyObjects "Clone Objects" width:120

		on CopyObjects pressed do
			(
				ObjArray = selection as array
				if ObjArray.count >= 1 then
					(
						for o in ObjArray do
							(
								if CopyType.state == 1 then
									d = instance o
								else
									d = copy o
								setUserPropBuffer d (getUserPropBuffer o)
								d.wirecolor = o.wirecolor
								d.pos.x += offset.value
							)
					) else (
						Messagebox "Must have something selected!"
					)
			)
	)
