-- A function which takes a vwector and returns a valid
-- rotation with it's y axis pointing down the vector.
fn y_point dir =
	(
		y = normalize dir
		
		if (y.z == 0)then
			if (y.y == 0)then
				return "bogus";
		
		--	z = [0,0,1]
		x = [1,0,0]
		--	z = y * (rotateXMatrix 90) 
		--	z = y * (rotateYMatrix 90) 
		z = normalize (cross x y)
		x = - normalize (cross z y)
		m =	matrix3 x y z [0,0,0]
		inverse m
	)

fn point_obj obj dir = 
	(
		local pos = obj.position
		obj.position = [0,0,0]
		ret = y_point dir
		if (ret == "bogus") do
			(
				obj.position = pos
				return "BOGUS"
			)
	
		obj.rotation = ret as quat
		obj.position = pos

		-- make sure that Z points up
--		if ForceZup.checked do
			if obj.dir.z < 0.0 do in coordsys local rotate obj 180 y_axis
	)


fn Repoint source target =
(
--  Forces source to point to target with its Y axis.
	local v3_to

	if (target == source) then return "Target == Source"

	if (target == undefined) then return "target undefined"
	if (source == undefined) then return "source undefined"
	
	v3_to = target.position - source.position
	
	if (v3_to == undefined) then
		exit
	
	point_obj source v3_to
)

fn CenteredRepoint source target =
(
	if (target == undefined) then return "target undefined"
	if (source == undefined) then return "source undefined"

	source.pivot = source.center
	target.pivot = target.center

	Repoint source target
)

utility Pointer "Pointer"
(

	pickbutton Activate "Point Selected"
	checkbox ForceZup "Force Z axis up"
	button flipJoints "Flip Joints"

	on Activate picked obj do
		(
			CenteredRepoint selection[1] obj
			select obj
		)

	on flipJoints pressed do
		in coordsys local rotate selection 180 y_axis
)