fn GetIncludes obj =
	(
		-- #include "[Object.name]"
		local IncludeObjects = #()
		local StartPos, EndPos
		CBuffer = GetUserPropBuffer obj
		if (StartPos = findString CBuffer "#include <") != undefined do
			(
				EndPos = 0
				for i = (StartPos + 10) to Cbuffer.count do
					if CBuffer[i] == ">" do
						(
							EndPos = i
							Exit
						)
				if EndPos > 0 do
					(
						local CName = (substring CBuffer (StartPos + 10) (EndPos - (StartPos + 10)))
						for o in objects do
							if o.name == CName do
								(
									append IncludeObjects  o
									Exit
								)
					)
			)
		return IncludeObjects
	)

fn KeyName CName = 
	(
		local DashPos = -1
		for i = 1 to CName.count do
			if CName[i] == "-" do
				DashPos = i
		if DashPos == -1 then
			return CName
		else
			return (substring CName 1 (DashPos - 1))
	)

fn MungePropsBuffer o = 
	(
		CBuffer = getUserPropBuffer o
		if Cbuffer.count > 500 then
			(
				ObjIndex = 1
				while (getUserPropBuffer o).count > 500 do
					(
						CBuffer = getUserPropBuffer o
						SplitIndex = 0
						for i = 400 to Cbuffer.count do
							if (substring CBuffer i 3) == "\r\r\n" do
								(
									SplitIndex = (i + 3)
									Exit
								)
						Buffer1 = substring CBuffer 1 (SplitIndex - 1)
						Buffer2 = substring CBuffer SplitIndex (Cbuffer.count - SplitIndex + 1)
						h = hedra()
						h.radius = 0.5
						h.name = ((Keyname o.name) + (ObjIndex as string) + ".oal")
						setUserPropBuffer o (Buffer1 + "#include<" + h.name + ">\r\r\n")
						setUserPropBuffer h ("//OAL bject used by \"" + o.name + "\"\r\r\n" + Buffer2)
						ObjIndex += 1
					)
				return true
			) else (
				return undefined
			)
	)