-- ; <Master object name> <object that can instance> <pos> <pos> <pos> <rot> <rot> <rot> <rot> <scale>
fn WordArray textline = 
(
	local c;
	local i = 1;
	local word = "";
	local word_array = #();
	
	for i = 1 to textline.count do
		(
			c = textline[i];
			if (c == " " or c == "\t" or c == "\n") then
				(
					append word_array word
					word = "";
				)
			else
				word = word + c;
		)
	append word_array word;
)

fn IsMaster CName = 
	(
		local DashPos = -1
		for i = 1 to CName.count do
			if CName[i] == "-" do
				DashPos = i
		if DashPos == -1 then
			return false
		else
			if (substring CName (DashPos) 3) == "-00" then
				return true
			else
				return false
	)


fn FindInProps str = 
	(
		for o in objects where (IsMaster o.name) do
			(
				CBuffer = getUserPropBuffer o
				if CBuffer.count > 0 do
					(
						1 == 1
					)
			)
	)

fn GetObject oName = 
	(
		for o in objects do
			if o.name == oName then
				return o
		return undefined
	)

Utility RemDupPhysicsBoxes "Dup Boxes"
	(
		local debug = false

		label help1 "Loads duplicates.txt,"
		label help2 "written out by GUIApp"
		label help3 "with duplicates compile"
		label help4 "switch turned on"
		button LoadFile "Load File" width:140

		on LoadFile pressed do
			if (TextFile = getOpenFilename caption:"Select Duplicates Text File:") != undefined do
				(
					f = openfile TextFile
					del = #()
					while not eof f do
						(
							CLine = readLine f
							if CLine[1] != ";" do
								LineData = wordArray CLine
							if (o = GetObject LineData[1]) != undefined do
								(
									if debug do
										(
											format "Line Data count %\n" lineData.count
											format "Found object % ... instancing and updating prs\n" o.name
										)
									p = instance o
									p.rotation = quat (LineData[7] as float) (LineData[8] as float) (LineData[9] as float) (LineData[6] as float)
									p.pos = [LineData[3] as float, LineData[4] as float, LineData[5] as float]
									p.scale = [(LineData[10] as float), (LineData[10] as float), (LineData[10] as float)]
									if (o = GetObject LineData[2]) != undefined do
										(
											format "Deleting \"%\"... replaced by new instanced object \"%\"\n" o.name p.name
											append del o
										)
								)
						)
					close f
					delete del
					messagebox (del.count as string + " Fysics boxes instanced...")
				)
	)