filein "\\\JPII_PC\\Trespass\Art\\Tools\Scripts\functions.ms"
Utility ScriptsControl "__Script Control"
	(
		ListBox Scripts height:5 items:#(	"Anim Mapper",\
																			"AssetDebugger",\
																			"Asset Group Checker",\
																			"Batch Debugger",\
																			"Bitmap Tools",\
																			"Bounding Box",\
																			"Bounding Objects",\
																			"Bumpmap Manager",\
																			"Combine",\
																			"Cuber",\
																			"Dummy Material",\
																			"Find in Materials",\
																			"Find Intersections",\
																			"Heap Manager",\
																			"Import Export",\
																			"Load Groff Text",\
																			"Match Dir",\
																			"Measurement Tools",\
																			"Mesh Tools",\
																			"Morph",\
																			"Munch Scene",\
																			"Naming Tools",\
																			"Obj Props Selection",\
																			"Obj Props Editing",\
																			"Object Properties",\
																			"Paint Trees",\
																			"Placement Manager",\
																			"Pointer",\
																			"Replace",\
																			"Scene Release Tools",\
																			"Scene Utils",\
																			"Selection Tools",\
																			"Slope",\
																			"Submit Assets",\
																			"Terrain Exporter",\
																			"Thumbnails",\
																			"Tree Placer",\
																			"Trigger Tools",\
																			"UV Tools",\
																			"VPlace",\
																			"ZRelax")

		button Execute "Execute" width:140
		
		on Execute pressed do
			(
				if Scripts.selection == 1 do
					filein "\\\JPII_PC\\Trespass\Art\\Tools\Scripts\AnimMapper.ms"
				if Scripts.selection == 2 do
					filein "\\\JPII_PC\\Trespass\Art\\Tools\Scripts\AssetDebugger.ms"
				if Scripts.selection == 3 do
					filein "\\\JPII_PC\\Trespass\Art\\Tools\Scripts\Asset_Checking.ms"
				if Scripts.selection == 4 do
					filein "\\\JPII_PC\\Trespass\Art\\Tools\Scripts\BatchDebugger.ms"
				if Scripts.selection == 5 do
					filein "\\\JPII_PC\\Trespass\Art\\Tools\Scripts\BitmapTools.ms"
				if Scripts.selection == 6 do
					filein "\\\JPII_PC\\Trespass\Art\\Tools\Scripts\box.ms"
				if Scripts.selection == 7 do
					filein "\\\JPII_PC\\Trespass\Art\\Tools\Scripts\BObjects.ms"
				if Scripts.selection == 8 do
					filein "\\\JPII_PC\\Trespass\Art\\Tools\Scripts\BumpMapManager.ms"
				if Scripts.selection == 9 do
					filein "\\\JPII_PC\\Trespass\Art\\Tools\Scripts\Combine.ms"
				if Scripts.selection == 10 do
					filein "\\\JPII_PC\\Trespass\Art\\Tools\Scripts\Cuber.ms"
				if Scripts.selection == 11 do
					filein "\\\JPII_PC\\Trespass\Art\\Tools\Scripts\DummyMat.ms"
				if Scripts.selection == 12 do
					filein "\\\JPII_PC\\Trespass\Art\\Tools\Scripts\findInMat.ms"
				if Scripts.selection == 13 do
					filein "\\\JPII_PC\\Trespass\Art\\Tools\Scripts\FindBBoxIntersections.ms"
				if Scripts.selection == 14 do
					filein "\\\JPII_PC\\Trespass\Art\\Tools\Scripts\HeapManager.ms"
				if Scripts.selection == 15 do
					filein "\\\JPII_PC\\Trespass\Art\\Tools\Scripts\imp_exp.ms"
				if Scripts.selection == 16 do
					filein "\\\JPII_PC\\Trespass\Art\\Tools\Scripts\LoadGroffText.ms"
				if Scripts.selection == 17 do
					filein "\\\JPII_PC\\Trespass\Art\\Tools\Scripts\MatchDir.ms"
				if Scripts.selection == 18 do
					filein "\\\JPII_PC\\Trespass\Art\\Tools\Scripts\MeasureTools.ms"
				if Scripts.selection == 19 do
					filein "\\\JPII_PC\\Trespass\Art\\Tools\Scripts\MeshTools.ms"
				if Scripts.selection == 20 do
					filein "\\\JPII_PC\\Trespass\Art\\Tools\Scripts\Morph.ms"
				if Scripts.selection == 21 do
					filein "\\\JPII_PC\\Trespass\Art\\Tools\Scripts\MunchScene.ms"
				if Scripts.selection == 22 do
					filein "\\\JPII_PC\\Trespass\Art\\Tools\Scripts\\NamingTools.ms"
				if Scripts.selection == 23 do
					filein "\\\JPII_PC\\Trespass\Art\\Tools\Scripts\ObjPropsSelect.ms"
				if Scripts.selection == 24 do
					filein "\\\JPII_PC\\Trespass\Art\\Tools\Scripts\ObjPropsEdit.ms"
				if Scripts.selection == 25 do
					filein "\\\JPII_PC\\Trespass\Art\\Tools\Scripts\objprops.ms"
				if Scripts.selection == 26 do
					filein "\\\JPII_PC\\Trespass\Art\\Tools\Scripts\pt.ms"
				if Scripts.selection == 27 do
					filein "\\\JPII_PC\\Trespass\Art\\Tools\Scripts\PlacementManager.ms"
				if Scripts.selection == 28 do
					filein "\\\JPII_PC\\Trespass\Art\\Tools\Scripts\Pointer.ms"
				if Scripts.selection == 29 do
					filein "\\\JPII_PC\\Trespass\Art\\Tools\Scripts\\replace.ms"
				if Scripts.selection == 30 do
					filein "\\\JPII_PC\\Trespass\Art\\Tools\Scripts\sceneReleaseTools.ms"
				if Scripts.selection == 31 do
					filein "\\\JPII_PC\\Trespass\Art\\Tools\Scripts\SceneUtils.ms"
				if Scripts.selection == 32 do
					filein "\\\JPII_PC\\Trespass\Art\\Tools\Scripts\SelectionTools.ms"
				if Scripts.selection == 33 do
					filein "\\\JPII_PC\\Trespass\Art\\Tools\Scripts\slope.ms"
				if Scripts.selection == 34 do
					filein "\\\JPII_PC\\Trespass\Art\\Tools\Scripts\Asset_Submission.ms"
				if Scripts.selection == 35 do
					filein "\\\JPII_PC\\Trespass\Art\\Tools\Scripts\TerrainExporter.ms"
				if Scripts.selection == 36 do
					filein "\\\JPII_PC\\Trespass\Art\\Tools\Scripts\ThumbGen.ms"
				if Scripts.selection == 37 do
					filein "\\\JPII_PC\\Trespass\Art\\Tools\Scripts\TreeDebrisPlacer.ms"
				if Scripts.selection == 38 do
					filein "\\\JPII_PC\\Trespass\Art\\Tools\Scripts\\TriggerTools.ms"
				if Scripts.selection == 39 do
					filein "\\\JPII_PC\\Trespass\Art\\Tools\Scripts\UVTools.ms"
				if Scripts.selection == 40 do
					filein "\\\JPII_PC\\Trespass\Art\\Tools\Scripts\VPlace.ms"
				if Scripts.selection == 41 do
					filein "\\\JPII_PC\\Trespass\Art\\Tools\Scripts\Zrelax.ms"
			)
	)
