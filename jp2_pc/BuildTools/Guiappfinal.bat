@echo off
@rem
@attrib -r -h -s ..\source\gblinc\config.hpp
@attrib -r -h -s configs\config_*.hpp
@touch configs\config_p5.hpp
@touch configs\config_p6.hpp
@touch configs\config_k6.hpp
@rem
@rem ***********************************************************
@rem
@rem   Build version of GUI App Final for all Pentium
@rem
@rem ***********************************************************
@rem
@rem Build Pentium Version
@rem
:BuildOnly
@title GUIApp Final Pentium Version
copy configs\config_p5.hpp ..\source\gblinc\config.hpp
msdev ..\jp2_pc.dsw /make "GUIApp - win32 final" /rebuild
copy ..\build\final\guiapp\guiapp.exe \\jpii_pc\trespas2\drop\smoke\tools\final_guiapp_p5.exe
@echo The build has been placed on smoke\tools\final_guiapp_p5.exe
