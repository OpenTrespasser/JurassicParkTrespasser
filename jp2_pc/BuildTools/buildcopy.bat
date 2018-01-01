@rem -------------------------------------------
@rem
@rem   Ensure that the smoke directory exists
@rem
@rem -------------------------------------------
@rem
@if not exist "\\jpii_pc\trespas2\drop\smoke" mkdir \\jpii_pc\trespas2\drop\smoke
@rem
@rem
@echo -------------------------------------------
@rem
@echo          Copy the template directories
@rem 
@echo -------------------------------------------
@rem
@title Copying Template Directories
@xcopy \\jpii_pc\trespas2\drop\template \\jpii_pc\trespas2\drop\smoke /E /h /r /d
@rem
@copy \\DWI-ENT-3\SourceSafe\shadow\JP2_PC\readme.txt \\jpii_pc\trespas2\drop\smoke\game
@attrib -r \\jpii_pc\trespas2\drop\smoke\game\readme.txt
@rem 
@rem @copy \\DWI-ENT-3\SourceSafe\shadow\JP2_PC\tools\*.exe \\jpii_pc\trespas2\drop\smoke\game\setup
@rem attrib -r \\jpii_pc\trespas2\drop\smoke\game\setup\*.exe
@rem
@rem
@echo ------------------------------------------
@echo            Copy the TPA Files
@echo ------------------------------------------
@title Copying TPA Files
@xcopy \\trespasser\siteb\audiopackfiles\*.tpa \\jpii_pc\trespas2\drop\smoke\game\data /e /h /r /d
@rem
@rem
@call copyscenes
@rem
@title Finished Copy

