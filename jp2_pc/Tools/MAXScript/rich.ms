if heapsize < 67108864 do heapsize = 67108864
fileIn "\\\JPII_PC\\Trespass\Art\\Tools\Scripts\functions.ms"
Utility ScriptsControl "__Script Control"
	(
		ListBox Scripts height:10 items:#(	"AI Commands",\
																			"Anim Mapper",\
																			"AssetDebugger",\
																			"Asset Group Checker",\
																			"Audio Coordinator",\
																			"Batch Debugger",\
																			"Batch Replace",\
																			"BG Trees Manager",\
																			"Bitmap Tools",\
																			"Bounding Box",\
																			"Bounding Objects",\
																			"Bumpmap Manager",\
																			"Copy Transform",\
																			"Dummy Material",\
																			"Equalizer",\
																			"Find in Materials",\
																			"Find Intersections",\
																			"Fortune Teller",\
																			"Heap Manager",\
																			"HiRez - LoRez",\
																			"Import Export",\
																			"Load Groff Text",\
																			"Magic 8 Ball",\
																			"Match Dir",\
																			"Measurement Tools",\
																			"Mesh Conform",\
																			"Mesh Tools",\
																			"Morph",\
																			"Munch Scene",\
																			"Naming Tools",\
																			"Object Info",\
																			"Obj Merge Assist",\
																			"Obj Props Selection",\
																			"Obj Props Editing",\
																			"Object Properties",\
																			"Paint Trees",\
																			"Placement Manager",\
																			"Pointer",\
																			"Quantization Mgr",\
																			"Replace",\
																			"Scene Release Tools",\
																			"Scene Utils",\
																			"Selection Tools",\
																			"Skeleton Props",\
																			"Slope",\
																			"Streaking Faces",\
																			"Submit Assets",\
																			"Sound Mat Update",\
																			"Tableaux Manager",\
																			"Terrain Exporter",\
																			"Thumbnails",\
																			"Tree Placer",\
																			"Trigger Tools",\
																			"UV Tools",\
																			"VPlace",\
																			"ZRelax")

		button Execute "Execute" width:140
		
		group "Auto Load Script Groups"
			(
				button AssetDebugging "Debugging" width:70 offset:[-37,0]
				button AreaReleasing "Releasing" width:70 offset:[37,-26]
				button AreaEditing "Editing" width:70 offset:[-37,0]
				button TerrainEditing "Terrain" width:70 offset:[37,-26]
				button DetailReduction "Detail" width:70 offset:[-37,0]
				button FunScripts "Fun Scripts" width:70 offset:[37,-26]
			)

		on FunScripts pressed do
			(
				MessageBox "Trespasser: No Fun"
			)

		on AssetDebugging pressed do
			(
				fileIn "\\\JPII_PC\\Trespass\Art\\Tools\Scripts\HeapManager.ms"
				fileIn "\\\JPII_PC\\Trespass\Art\\Tools\Scripts\AssetDebugger.ms"
				fileIn "\\\JPII_PC\\Trespass\Art\\Tools\Scripts\BatchDebugger.ms"
				fileIn "\\\JPII_PC\\Trespass\Art\\Tools\Scripts\BitmapTools.ms"
				fileIn "\\\JPII_PC\\Trespass\Art\\Tools\Scripts\BumpMapManager.ms"
				fileIn "\\\JPII_PC\\Trespass\Art\\Tools\Scripts\findInMat.ms"
				fileIn "\\\JPII_PC\\Trespass\Art\\Tools\Scripts\MeshTools.ms"
			)

		on AreaReleasing pressed do
			(
				fileIn "\\\JPII_PC\\Trespass\Art\\Tools\Scripts\HeapManager.ms"
				fileIn "\\\JPII_PC\\Trespass\Art\\Tools\Scripts\BatchDebugger.ms"
				fileIn "\\\JPII_PC\\Trespass\Art\\Tools\Scripts\BumpMapManager.ms"
				fileIn "\\\JPII_PC\\Trespass\Art\\Tools\Scripts\DummyMat.ms"
				fileIn "\\\JPII_PC\\Trespass\Art\\Tools\Scripts\MunchScene.ms"
				fileIn "\\\JPII_PC\\Trespass\Art\\Tools\Scripts\\NamingTools.ms"
				fileIn "\\\JPII_PC\\Trespass\Art\\Tools\Scripts\sceneReleaseTools.ms"
				fileIn "\\\JPII_PC\\Trespass\Art\\Tools\Scripts\TerrainExporter.ms"
			)

		on AreaEditing pressed do
			(
				fileIn "\\\JPII_PC\\Trespass\Art\\Tools\Scripts\HeapManager.ms"
				fileIn "\\\JPII_PC\\Trespass\Art\\Tools\Scripts\BumpMapManager.ms"
				fileIn "\\\JPII_PC\\Trespass\Art\\Tools\Scripts\box.ms"
				fileIn "\\\JPII_PC\\Trespass\Art\\Tools\Scripts\\NamingTools.ms"
				fileIn "\\\JPII_PC\\Trespass\Art\\Tools\Scripts\objprops.ms"
				fileIn "\\\JPII_PC\\Trespass\Art\\Tools\Scripts\LoadGroffText.ms"
				fileIn "\\\JPII_PC\\Trespass\Art\\Tools\Scripts\MeasureTools.ms"
				fileIn "\\\JPII_PC\\Trespass\Art\\Tools\Scripts\PlacementManager.ms"
				fileIn "\\\JPII_PC\\Trespass\Art\\Tools\Scripts\\replace.ms"
				fileIn "\\\JPII_PC\\Trespass\Art\\Tools\Scripts\SelectionTools.ms"
				fileIn "\\\JPII_PC\\Trespass\Art\\Tools\Scripts\TreeDebrisPlacer.ms"
				fileIn "\\\JPII_PC\\Trespass\Art\\Tools\Scripts\\TriggerTools.ms"
			)

		on TerrainEditing pressed do
			(
				fileIn "\\\JPII_PC\\Trespass\Art\\Tools\Scripts\HeapManager.ms"
				fileIn "\\\JPII_PC\\Trespass\Art\\Tools\Scripts\imp_exp.ms"
				fileIn "\\\JPII_PC\\Trespass\Art\\Tools\Scripts\MeasureTools.ms"
				fileIn "\\\JPII_PC\\Trespass\Art\\Tools\Scripts\\NamingTools.ms"
				fileIn "\\\JPII_PC\\Trespass\Art\\Tools\Scripts\objprops.ms"
				fileIn "\\\JPII_PC\\Trespass\Art\\Tools\Scripts\SceneUtils.ms"
				fileIn "\\\JPII_PC\\Trespass\Art\\Tools\Scripts\slope.ms"
				fileIn "\\\JPII_PC\\Trespass\Art\\Tools\Scripts\SelectionTools.ms"
				fileIn "\\\JPII_PC\\Trespass\Art\\Tools\Scripts\TrnTexUVs.ms"
				fileIn "\\\JPII_PC\\Trespass\Art\\Tools\Scripts\TerrainExporter.ms"
				fileIn "\\\JPII_PC\\Trespass\Art\\Tools\Scripts\VPlace.ms"
				fileIn "\\\JPII_PC\\Trespass\Art\\Tools\Scripts\Zrelax.ms"
	
			)

		on Detailreduction pressed do
			(
				fileIn "\\\JPII_PC\\Trespass\Art\\Tools\Scripts\objprops.ms"
				fileIn "\\\JPII_PC\\Trespass\Art\\Tools\Scripts\imp_exp.ms"
				fileIn "\\\JPII_PC\\Trespass\Art\\Tools\Scripts\Assetdebugger.ms"
				fileIn "\\\JPII_PC\\Trespass\Art\\Tools\Scripts\meshtools.ms"
				fileIn "\\\JPII_PC\\Trespass\Art\\Tools\Scripts\bitmaptools.ms"
				fileIn "\\\JPII_PC\\Trespass\Art\\Tools\Scripts\synchpivot.ms"
                fileIn "\\\JPII_PC\\Trespass\Art\\Tools\Scripts\height.ms"	
     	)

		on Execute pressed do
			(
				if Scripts.selection == 1 do fileIn "\\\JPII_PC\\Trespass\Art\\Tools\Scripts\AICommands.ms"
				if Scripts.selection == 2 do fileIn "\\\JPII_PC\\Trespass\Art\\Tools\Scripts\AnimMapper.ms"
				if Scripts.selection == 3 do fileIn "\\\JPII_PC\\Trespass\Art\\Tools\Scripts\AssetDebugger.ms"
				if Scripts.selection == 4 do fileIn "\\\JPII_PC\\Trespass\Art\\Tools\Scripts\Asset_Checking.ms"
				if Scripts.selection == 5 do fileIn "\\\JPII_PC\\Trespass\Art\\Tools\Scripts\AudioCoordinator.ms"
				if Scripts.selection == 6 do fileIn "\\\JPII_PC\\Trespass\Art\\Tools\Scripts\BatchDebugger.ms"
				if Scripts.selection == 7 do fileIn "\\\JPII_PC\\Trespass\Art\\Tools\Scripts\BatchReplace.ms"
				if Scripts.selection == 8 do fileIn "\\\JPII_PC\\Trespass\Art\\Tools\Scripts\BTrees.ms"
				if Scripts.selection == 9 do fileIn "\\\JPII_PC\\Trespass\Art\\Tools\Scripts\BitmapTools.ms"
				if Scripts.selection == 10 do fileIn "\\\JPII_PC\\Trespass\Art\\Tools\Scripts\box.ms"
				if Scripts.selection == 11 do fileIn "\\\JPII_PC\\Trespass\Art\\Tools\Scripts\BObjects.ms"
				if Scripts.selection == 12 do fileIn "\\\JPII_PC\\Trespass\Art\\Tools\Scripts\BumpMapManager.ms"
				if Scripts.selection == 13 do fileIn "\\\JPII_PC\\Trespass\Art\\Tools\Scripts\CopyTransform.ms"
				if Scripts.selection == 14 do fileIn "\\\JPII_PC\\Trespass\Art\\Tools\Scripts\DummyMat.ms"
				if Scripts.selection == 15 do fileIn "\\\JPII_PC\\Trespass\Art\\Tools\Scripts\Equalizer.ms"
				if Scripts.selection == 16 do fileIn "\\\JPII_PC\\Trespass\Art\\Tools\Scripts\findInMat.ms"
				if Scripts.selection == 17 do fileIn "\\\JPII_PC\\Trespass\Art\\Tools\Scripts\FindBBoxIntersections.ms"
				if Scripts.selection == 18 do fileIn "\\\JPII_PC\\Trespass\Art\\Tools\Scripts\FortuneTeller.ms"
				if Scripts.selection == 19 do fileIn "\\\JPII_PC\\Trespass\Art\\Tools\Scripts\HeapManager.ms"
				if Scripts.selection == 20 do fileIn "\\\JPII_PC\\Trespass\Art\\Tools\Scripts\HiRezLoRez.ms"
				if Scripts.selection == 21 do fileIn "\\\JPII_PC\\Trespass\Art\\Tools\Scripts\imp_exp.ms"
				if Scripts.selection == 22 do fileIn "\\\JPII_PC\\Trespass\Art\\Tools\Scripts\LoadGroffText.ms"
				if Scripts.selection == 23 do fileIn "\\\JPII_PC\\Trespass\Art\\Tools\Scripts\Magic8Ball.ms"
				if Scripts.selection == 24 do fileIn "\\\JPII_PC\\Trespass\Art\\Tools\Scripts\MatchDir.ms"
				if Scripts.selection == 25 do fileIn "\\\JPII_PC\\Trespass\Art\\Tools\Scripts\MeasureTools.ms"
				if Scripts.selection == 26 do fileIn "\\\JPII_PC\\Trespass\Art\\Tools\Scripts\MeshConform.ms"
				if Scripts.selection == 27 do fileIn "\\\JPII_PC\\Trespass\Art\\Tools\Scripts\MeshTools.ms"
				if Scripts.selection == 28 do fileIn "\\\JPII_PC\\Trespass\Art\\Tools\Scripts\Morph.ms"
				if Scripts.selection == 29 do fileIn "\\\JPII_PC\\Trespass\Art\\Tools\Scripts\MunchScene.ms"
				if Scripts.selection == 30 do	fileIn "\\\JPII_PC\\Trespass\Art\\Tools\Scripts\\NamingTools.ms"
				if Scripts.selection == 31 do	fileIn "\\\JPII_PC\\Trespass\Art\\Tools\Scripts\ObjInfo.ms"
				if Scripts.selection == 32 do	fileIn "\\\JPII_PC\\Trespass\Art\\Tools\Scripts\ObjMergeAssist.ms"
				if Scripts.selection == 33 do	fileIn "\\\JPII_PC\\Trespass\Art\\Tools\Scripts\ObjPropsSelect.ms"
				if Scripts.selection == 34 do	fileIn "\\\JPII_PC\\Trespass\Art\\Tools\Scripts\ObjPropsEdit.ms"
				if Scripts.selection == 35 do fileIn "\\\JPII_PC\\Trespass\Art\\Tools\Scripts\objprops.ms"
				if Scripts.selection == 36 do fileIn "\\\JPII_PC\\Trespass\Art\\Tools\Scripts\pt.ms"
				if Scripts.selection == 37 do	fileIn "\\\JPII_PC\\Trespass\Art\\Tools\Scripts\PlacementManager.ms"
				if Scripts.selection == 38 do	fileIn "\\\JPII_PC\\Trespass\Art\\Tools\Scripts\Pointer.ms"
				if Scripts.selection == 39 do	fileIn "\\\JPII_PC\\Trespass\Art\\Tools\Scripts\QuantManager.ms"
				if Scripts.selection == 40 do fileIn "\\\JPII_PC\\Trespass\Art\\Tools\Scripts\\replace.ms"
				if Scripts.selection == 41 do fileIn "\\\JPII_PC\\Trespass\Art\\Tools\Scripts\sceneReleaseTools.ms"
				if Scripts.selection == 42 do fileIn "\\\JPII_PC\\Trespass\Art\\Tools\Scripts\SceneUtils.ms"
				if Scripts.selection == 43 do	fileIn "\\\JPII_PC\\Trespass\Art\\Tools\Scripts\SelectionTools.ms"
				if Scripts.selection == 44 do	fileIn "\\\JPII_PC\\Trespass\Art\\Tools\Scripts\SkelProps.ms"
				if Scripts.selection == 45 do	fileIn "\\\JPII_PC\\Trespass\Art\\Tools\Scripts\slope.ms"
				if Scripts.selection == 46 do	fileIn "\\\JPII_PC\\Trespass\Art\\Tools\Scripts\StreakFaces.ms"
				if Scripts.selection == 47 do fileIn "\\\JPII_PC\\Trespass\Art\\Tools\Scripts\Asset_Submission.ms"
				if Scripts.selection == 48 do fileIn "\\\JPII_PC\\Trespass\Art\\Tools\Scripts\SndMatUpdate.ms"
				if Scripts.selection == 49 do fileIn "\\\JPII_PC\\Trespass\Art\\Tools\Scripts\TableauxManager.ms"
				if Scripts.selection == 50 do fileIn "\\\JPII_PC\\Trespass\Art\\Tools\Scripts\TerrainExporter.ms"
				if Scripts.selection == 51 do fileIn "\\\JPII_PC\\Trespass\Art\\Tools\Scripts\ThumbGen.ms"
				if Scripts.selection == 52 do fileIn "\\\JPII_PC\\Trespass\Art\\Tools\Scripts\TreeDebrisPlacer.ms"
				if Scripts.selection == 53 do	fileIn "\\\JPII_PC\\Trespass\Art\\Tools\Scripts\\TriggerTools.ms"
				if Scripts.selection == 54 do	fileIn "\\\JPII_PC\\Trespass\Art\\Tools\Scripts\UVTools.ms"
				if Scripts.selection == 55 do	fileIn "\\\JPII_PC\\Trespass\Art\\Tools\Scripts\VPlace.ms"
				if Scripts.selection == 56 do	fileIn "\\\JPII_PC\\Trespass\Art\\Tools\Scripts\Zrelax.ms"
			)
	)
