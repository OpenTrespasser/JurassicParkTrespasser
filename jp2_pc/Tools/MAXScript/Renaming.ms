-- **********************************************************************************************
-- *
-- * Copyright © DreamWorks Interactive, 1997
-- *
-- * Contents:
-- *		Implementation of Renaming.ms 
-- *
-- * Bugs:
-- *
-- * To do:
-- *
-- * Notes:
-- *
-- **********************************************************************************************

utility AssetRenaming "Renaming"
(

local
slash_count = 0,
diffuse_mapname_length = 0,
mat_name_length = 0,
opacity_mapname_length = 0,
bump_mapname_length = 0,
i = 0,
j = 0,
Sel_Object_count = 0,
diffuse_tag = "t2",
opacity_tag = "o8",
bump_tag = "b8",
bmpFileTag = ".bmp";

label version "v0.7"

group "Rename from Maps"
	(
	checkbox Ren_Obj_chkbox "Rename Objects" 
	checkbox Ren_Multi_chkbox "Rename Multi/Subs"
	button Rename_Selected_btn "Rename Selected" width:135
	)

on Rename_Selected_btn pressed do
	( undo on (
	Sel_Object_count = 0;
	for o in objects do 
		(
		if o.isselected then
			(
			Sel_Object_count = (Sel_Object_Count +1);
			if classof o.mat == multimaterial do 
				(
				for i = 1 to o.mat.numsubs do
					(
		if ClassOf o.mat[i].diffusemap == bitmaptexture do
			(
			diffuse_map_name = (FileNameFromPath o.mat[i].diffusemap.filename);
			mat_name_length = (diffuse_map_name.count - 6);
			single_mat_name = (substring diffuse_map_name 1 mat_name_length);
			o.mat[i].name = single_mat_name;
			o.mat[i].diffusemap.name = diffuse_map_name;
			);

		if ClassOf o.mat[i].opacitymap == bitmaptexture do (o.mat[i].opacitymap.name = (FileNameFromPath o.mat[i].opacitymap.filename))
		if ClassOf o.mat[i].bumpmap == bitmaptexture then (o.mat[i].bumpmap.name = (FileNameFromPath o.mat[i].bumpmap.filename))

-- hack for object name

diffuse_map_name = (FileNameFromPath o.mat[1].diffusemap.filename);
mat_name_length = (diffuse_map_name.count - 6);
single_mat_name = (substring diffuse_map_name 1 mat_name_length);
Object_name_tag = "-00";
Object_name = (substring single_mat_name 1 (single_mat_name.count - 2));

if Sel_Object_Count <= 9 do (NameCount = ("0" + Sel_Object_Count as string))
if Sel_Object_Count >= 10 do (NameCount = Sel_Object_Count as string)

if Ren_Obj_chkbox.checked do (o.name = (Object_name + NameCount + Object_name_tag));
if Ren_Multi_chkbox.checked do (o.mat.name = (Object_name + NameCount));

));
if classof o.mat == standardmaterial do
(
	diffuse_map_name = (FileNameFromPath o.mat.diffusemap.filename);
	mat_name_length = (diffuse_map_name.count - 6);
	single_mat_name = (substring diffuse_map_name 1 mat_name_length);
	o.mat.name = single_mat_name;
	o.mat.diffusemap.name = diffuse_map_name;
	o.name = (o.mat.name + "-00");
))))))
