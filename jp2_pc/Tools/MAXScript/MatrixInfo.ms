--**********************************************************************************************
-- *
-- * Copyright © DreamWorks Interactive, 1997
-- *
-- * Contents:
-- *		Implementation od ObjInfo.ms
-- * Bugs:
-- *
-- * To do:
-- *
-- * Notes:
-- * 		Writes out matrix information about a scene to a text file

-- **********************************************************************************************

Utility MatrixInfo "Output Obj Matrix Info"
(
label v "v1.0"
group "Get Matrix Information"
(
pickbutton GetObj "Pick Object" width:135
button DoScene "Output Whole Scene" width:135
)
on DoScene pressed do
(
save_filename = getsavefilename caption:"Specify Scene Matrix Dump File"
f = createfile save_filename
for o in objects do
(
format "\n"
format "****************************************************************************\n" to:f
format "Object Data for %\n" o.name to:f
format "****************************************************************************\n" to:f
format "Object Pivot:\t\t\t\t%\n" o.pivot to:f
format "Object Geometrical Center:\t\t%\n" o.center to:f
format "Pivot/Center Offset:\t\t\t%\n" (o.pivot - o.center) to:f
format "Object Position:\t\t\t\t%\n" o.pos to:f
format "Object Extents:\t\t\t\t%\n" (o.max - o.min) to:f
format "\n" to:f
format "Main Node Transformation Matrix:\n" to:f
format "\t\t\t\t\t%\n" o.transform.row1 to:f
format "\t\t\t\t\t%\n" o.transform.row2 to:f
format "\t\t\t\t\t%\n" o.transform.row3 to:f
format "\t\t\t\t\t%\n" o.transform.row4 to:f
format "\n" to:f
format "Translation:\t\t\t\t%\n" o.transform.translation to:f
format "Rotation Part:\t\t\t\t%\n" o.transform.rotationpart to:f
format "Translation Part:\t\t\t%\n" o.transform.translationpart to:f
format "Scale\Rotation Part:\t\t\t%\n" o.transform.scalerotationpart to:f
format "Scale Part:\t\t\t\t%\n" o.transform.scalepart to:f
format "Determinant Sign:\t\t\t%\n" o.transform.determinantsign to:f
format "Z Axis direction Vector:\t\t\t%\n" o.dir to:f
format "Object Offset Position:\t\t\t%\n" o.objectoffsetpos to:f
format "Object Offset Rotation:\t\t\t%\n" o.objectoffsetrot to:f
-- (not yet implemented) format "Object Offset Scale: %\n" o.objectoffsetscale to:f
format "\n" to:f
format "Object Transformation Matrix:\n" to:f
format "\t\t\t\t\t%\n" o.objecttransform.row1 to:f
format "\t\t\t\t\t%\n" o.objecttransform.row2 to:f
format "\t\t\t\t\t%\n" o.objecttransform.row3 to:f
format "\t\t\t\t\t%\n" o.objecttransform.row4 to:f
format "\n" to:f
format "Translation:\t\t\t\t%\n" o.objecttransform.translation to:f
format "Rotation Part:\t\t\t\t%\n" o.objecttransform.rotationpart to:f
format "Translation Part:\t\t\t%\n" o.objecttransform.translationpart to:f
format "Scale/Rotation Part:\t\t\t%\n" o.objecttransform.scalerotationpart to:f
format "Scale Part:\t\t\t\t%\n" o.objecttransform.scalepart to:f
format "Determinant Sign:\t\t\t%\n" o.objecttransform.determinantsign to:f
)
close f
)

on GetObj picked obj do
(
o = obj
save_filename = getsavefilename caption:"Specify Data Dump File"
f = createfile save_filename
format "****************************************************************************\n" to:f
format "Object Data for %\n" o.name to:f
format "****************************************************************************\n" to:f
format "Object Pivot:\t\t\t\t%\n" o.pivot to:f
format "Object Geometrical Center:\t\t%\n" o.center to:f
format "Pivot/Center Offset:\t\t\t%\n" (o.pivot - o.center) to:f
format "Object Position:\t\t\t\t%\n" o.pos to:f
format "Object Extents:\t\t\t\t%\n" (o.max - o.min) to:f
format "\n" to:f
format "Main Node Transformation Matrix:\n" to:f
format "\t\t\t\t\t%\n" o.transform.row1 to:f
format "\t\t\t\t\t%\n" o.transform.row2 to:f
format "\t\t\t\t\t%\n" o.transform.row3 to:f
format "\t\t\t\t\t%\n" o.transform.row4 to:f
format "\n" to:f
format "Translation:\t\t\t\t%\n" o.transform.translation to:f
format "Rotation Part:\t\t\t\t%\n" o.transform.rotationpart to:f
format "Translation Part:\t\t\t%\n" o.transform.translationpart to:f
format "Scale\Rotation Part:\t\t\t%\n" o.transform.scalerotationpart to:f
format "Scale Part:\t\t\t\t%\n" o.transform.scalepart to:f
format "Determinant Sign:\t\t\t%\n" o.transform.determinantsign to:f
format "Z Axis direction Vector:\t\t\t%\n" o.dir to:f
format "Object Offset Position:\t\t\t%\n" o.objectoffsetpos to:f
format "Object Offset Rotation:\t\t\t%\n" o.objectoffsetrot to:f
-- (not yet implemented) format "Object Offset Scale: %\n" o.objectoffsetscale to:f
format "\n" to:f
format "Object Transformation Matrix:\n" to:f
format "\t\t\t\t\t%\n" o.objecttransform.row1 to:f
format "\t\t\t\t\t%\n" o.objecttransform.row2 to:f
format "\t\t\t\t\t%\n" o.objecttransform.row3 to:f
format "\t\t\t\t\t%\n" o.objecttransform.row4 to:f
format "\n" to:f
format "Translation:\t\t\t\t%\n" o.objecttransform.translation to:f
format "Rotation Part:\t\t\t\t%\n" o.objecttransform.rotationpart to:f
format "Translation Part:\t\t\t%\n" o.objecttransform.translationpart to:f
format "Scale/Rotation Part:\t\t\t%\n" o.objecttransform.scalerotationpart to:f
format "Scale Part:\t\t\t\t%\n" o.objecttransform.scalepart to:f
format "Determinant Sign:\t\t\t%\n" o.objecttransform.determinantsign to:f
close f
))
