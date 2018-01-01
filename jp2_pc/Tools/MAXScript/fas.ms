fn FloatAsString f = 
(
CString = f as string
Convert = true
for i = 1 to CString.count do
	if CString[i] == "." do
		Convert = false
if Convert == true do
	CString = (CString + ".0")
return CString
)