--  A script to import GUIApp text files.

fn GetGUIAppScale obj =
(
	local rot = obj.rotation;
	obj.rotation = (quat 0 Z_axis);
	
	local dim = obj.max - obj.min;
	local GUIscale = dim.x;
	if (dim.y > GUIScale) do
		GUIscale = dim.y;
	if (dim.z > GUIScale) do
		GUIscale = dim.z;
	
	obj.rotation = rot;
	
	return GUIscale * 0.5;
)

fn UpdateObject obj word_array = 
(
	local len, angle, x, y, z, w;
	len = word_Array.count;
--	print word_array;

	-- Read from right to left.
	-- Scale is the last number
	local rescale = GetGUIAppScale obj;
	rescale = word_array[len] as float / rescale;
	obj.scale = obj.scale * rescale;
	-- ignore scale
	len = len - 1;
		
	-- Rotation is the next four
	w = word_array[len-3] as float;
	x = word_array[len-2] as float;
	y = word_array[len-1] as float;
	z = word_array[len-0] as float;
	obj.rotation = (quat x y z w);
	len = len - 4;

	-- Position is the next three.
	x = word_array[len-2] as float;
	y = word_array[len-1] as float;
	z = word_array[len-0] as float;
	obj.pos = [x, y, z]
	len = len - 3;
)



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
		(
			word = word + c;
		)
	)
	append word_array word;
)

fn load_placement_file filename = 
(
	-- loads the file into an array of arrays.  Each array in the 
	--	containing array is a line of text as a WordArray.
	
	local FileArray = #();
	
	local f, l;
	f = openfile filename; 
	if f != undefined then
	(
		while not eof f do
		(
			l = readline f;
			append FileArray (WordArray l);
		)
		close f;	
	)
	else
	(
		print "File not found";
		print filename
	)
	filearray
)

fn find_array_index obj filearray = 
(
	-- object name should not have spaces!
	if (wordArray obj.name).count != 1 then
	(
		print "Object name cannot have spaces!";
		print obj.name;
	)
	else
	(
		-- Find object in array.
		arraylength = filearray.count;
		
		for i = 1 to arraylength do
		(
			linelength = filearray[i].count;
			if (obj.name == filearray[i][linelength - 8]) do
			(
				-- found it!  Now update it.
				return i;
			)
		)
	)
	
	return undefined;
)

fn update_obj_from_array obj filearray =
(
	local i;
	i = find_array_index obj filearray;

	if i != undefined then
	(
		updateObject obj filearray[i]
	)
	else
	(
		print "Object not found: " + obj.name;
	)
)