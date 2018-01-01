@title Copying %1
@for %%i in (\\trespasser\siteb\island\%1\*.grf) do @call gawk -f grf_press.awk %%i grp \\jpii_pc\trespas2\drop\smoke\game\data "/d"
@xcopy "\\trespasser\siteb\island\%1\*.scn" \\jpii_pc\trespas2\drop\smoke\game\data /d
@xcopy "\\trespasser\siteb\island\%1\*.pid" \\jpii_pc\trespas2\drop\smoke\game\data /d
@for %%i in (\\trespasser\siteb\island\%1\*.swp) do @call gawk -f mypress.awk %%i spz
@xcopy \\trespasser\siteb\island\%1\*.spz \\jpii_pc\trespas2\drop\smoke\game\data /d
@xcopy "\\trespasser\siteb\island\%1\*.wtd" \\jpii_pc\trespas2\drop\smoke\game\data /d

