Utility UnitConversion "Units Conversion"
(
label v "v1.0"

spinner MetersSpinner "M:" range:[0,100000,0] type:#float offset:[-25,0]
spinner FeetSpinner "Ft:" range:[0,100000,0] type:#float offset:[-25,0]
spinner InchesSpinner "In:" range:[0,100000,0] type:#float offset:[-25,0]
label foo ""

on MetersSpinner changed state do
(
FeetSpinner.value = MetersSpinner.value * 3.28100
InchesSpinner.value = MetersSpinner.value * 39.3700
-- cmspinner.value = MetersSpinner.value * 100.00
)

on FeetSpinner changed state do
(
MetersSpinner.value = FeetSpinner.value * 0.304800
InchesSpinner.value = FeetSpinner.value / 0.083300
-- cmSpinner.value = FeetSpinner.value * 0.032800
)

on InchesSpinner changed state do
(
MetersSpinner.value = InchesSpinner.value * 0.025400
FeetSpinner.value = InchesSpinner.value * 0.083300
-- cmSpinner.value = InchesSpinner.value * 2.54
)

)