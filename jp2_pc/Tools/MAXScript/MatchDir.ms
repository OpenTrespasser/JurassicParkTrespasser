Utility MatchDir "Match Dir"
(
pickbutton PickObj "Pick Object"

on pickObj picked obj do
	(
	SourceDir = obj.dir
	for o in selection do
		(
			o.dir = sourceDir
		)
	)
)