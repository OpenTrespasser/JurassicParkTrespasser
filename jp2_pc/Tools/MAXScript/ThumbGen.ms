Utility ThumbGenerator "Thumbnails"
(

button generateThumbnails "Generate Thumbnails" width:140

on generateThumbnails pressed do
(
ThumbExitCode = #good

-- setup save paths

Props_Thumbnail_dir = "\\\JPII_PC\Trespass\Art\\Thumbnails\\"
Structure_Thumbnail_dir = "\\\JPII_PC\Trespass\Art\\Thumbnails\\"
Terrain_Thumbnail_dir = "\\\JPII_PC\Trespass\Art\\Thumbnails\\"
Animal_Thumbnail_dir = "\\\JPII_PC\Trespass\Art\\Thumbnails\\"
Vegetation_Thumbnail_dir = "\\\JPII_PC\Trespass\Art\\Thumbnails\\"

for o in objects do hide o -- hide everything

ObjNameList = for o in objects collect o.name
ObjList = for o in objects collect o
TOC = ObjNameList.count
UniqueObjNameList =#()
UniqueObjList =#()
for i = 1 to TOC do
	(
		CObjName = ObjNameList[i]
		CObj = ObjList[i]
		-- find the dash
		DashPos = -1
		for i = 1 to CObjName.count do
			(
				if substring CObjName i 1 == "-" do DashPos = i
			)
		-- if dash exists grab the substring
		if DashPos != -1 then
			(
				CObjNamePrefix = substring CObjName 1 (DashPos - 1)
				if finditem UniqueObjNameList CObjNamePrefix == 0 do 
					(
						Append UniqueObjNameList CObjNamePrefix
						append UniqueObjList CObj
					)
			) else (
				-- if not, then stick it into the Unique Array
				Append UniqueObjNameList CObjName
				append UniqueObjList CObj
				messagebox "appending"
			)
)

		ProgressStart "Generating Thumbnails...."
		progscale = 100.0 / UniqueObjList.count
		for i = 1 to UniqueObjList.count do
			(
				o = UniqueObjList[i]
				ObjName = o.name
				if (Thumb_Worthy o) == true do
					(
					GoodObj = false
					if ObjName[1] == "V" do (New_Thumbs_location = Vegetation_Thumbnail_dir; goodObj = true)
					if ObjName[1] == "v" do (New_Thumbs_location = Vegetation_Thumbnail_dir; goodObj = true)
					if ObjName[1] == "T" do (New_Thumbs_location = Terrain_Thumbnail_dir; goodObj = true)
					if ObjName[1] == "t" do (New_Thumbs_location = Terrain_Thumbnail_dir; goodObj = true)
					if ObjName[1] == "A" do (New_Thumbs_location = Animal_Thumbnail_dir; goodObj = true)
					if ObjName[1] == "a" do (New_Thumbs_location = Animal_Thumbnail_dir; goodObj = true)
					if ObjName[1] == "P" do (New_Thumbs_location = Props_Thumbnail_dir; goodObj = true)
					if ObjName[1] == "p" do (New_Thumbs_location = Props_Thumbnail_dir; goodObj = true)
					if ObjName[1] == "S" do (New_Thumbs_location = Structure_Thumbnail_dir; goodObj = true)
					if ObjName[1] == "s" do (New_Thumbs_location = Structure_Thumbnail_dir; goodObj = true)
					if goodObj == true then
						( 
							unhide o
							theta = 46.826
							d = distance o.max o.min
							x = (d / (2 * sin(theta / 2))) * 1.0
							NewCamera = targetCamera()
							NewCamera.fov = 60
							NewCamera.target = targetObject()
							SourceCenter = o.center
							NewCamera.target.pos = SourceCenter
							NewCamera.pos = [(SourceCenter.x + 0.5), (SourceCenter.y - .75), (SourceCenter.z + 0.25)]
							NewDir = normalize (NewCamera.pos - NewCamera.target.pos)
							NewCamera.pos = NewCamera.target.pos + NewDir * x
							b = (render camera:NewCamera outputwidth:640 outputheight:480 vfb:off)
							b.filename = (New_Thumbs_location + "\\" + o.name + ".jpg")
							save b
							delete NewCamera.target
							delete NewCamera
							hide o
							if ProgressUpdate (i * progscale) == false then exit
						) else (
							ThumbExitCode = #bad
						)
					)
			)
		for o in objects do unhide o
		ProgressEnd()		
		if ThumbExitCode == #bad do (MessageBox "some objects not thumbnailed")
		messagebox "Thumbnails Complete"
		)
)