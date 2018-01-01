-- returns the closest power of two number
fn NearPowOfTwo num = 
	(
		num2 = num
		counter = 0
		while num >= 1.0 do
			(
				num = (num * 0.5)
				counter = counter + 1
			)
			
		p2 = 2^(counter - 1)
		if (num2 > p2 * 1.5) then
			return (p2 * 2)
		else 
			return p2	
	)

fn getBitmapMemoryUsage bumpState bumpFactor bitmap =
	(
		memoryUsage = bitmap.width * bitmap.height
		if bumpState == 1 do
			memoryUsage = memoryUsage * bumpFactor
		return memoryUsage
	)

fn SelectUndefinedMat = 
	(
		for o in objects do 
			if classOf o.mat == UndefinedClass do
				selectmore o
	)

fn DeleteNonObjects =
	(
		local BadObjects = #()
		for o in objects do
			if o.numfaces == 0 do
				append BadObjects o
		delete BadObjects
	)

fn resettransform obj = 
	(
		local otm = obj.transform
		obj.transform = matrix3 1
		local xf = xform()
		addmodifier obj xf
		xf.gizmo.transform = otm
	)

fn roundTo val n = 
	(
		local mult = 10.0 ^ n
		(floor ((val * mult) + 0.5)) / mult
	)

fn Power_of_two_num num = 
	(
		validNum = false
		if num == 8 do (ValidNum = true)
		if num == 16 do (ValidNum = true)
		if num == 32 do (ValidNum = true)
		if num == 64 do (ValidNum = true)
		if num == 128 do (ValidNum = true)
		if num == 256 do (ValidNum = true)
		return ValidNum
	)

fn Power_of_two_Map BMap = 
	(
		ValidWidth = Power_of_two_num BMap.width
		ValidHeight = Power_of_two_num BMap.height
		if (validWidth == true) and (validHeight == true) then return true else return false
	)
