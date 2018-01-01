Utility JointsFromBones "Joints From Bones"
(
group "Joint Options"
(
edittext Prefix "Joint Prefix: " width:10 align:#left
spinner JointSize "Cube Size: " range:[0.01, 0.5, 0.1] type:#float align:#left
)
Button JfB "Joints From Bones" width: 130

on JfB pressed do
(
	JointPrefix = Prefix.text
	BonesArray = for obj in $Bone* collect obj
	NumBones = BonesArray.count
	for i = 1 to NumBones do
	(
		JointSuffix = substring BonesArray[i].name ((BonesArray[i].name).count - 1) 2
		b = box Width:JointSize.value Length:JointSize.value Height:JointSize.value name:(JointPrefix + JointSuffix)
		b.pivot = b.center
		b.pos = BonesArray[i].pos
	)
)


)