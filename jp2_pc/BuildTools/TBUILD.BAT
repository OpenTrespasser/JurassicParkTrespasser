@echo off
@if not exist "..\release" mkdir ..\release
@del ..\release
@rem
@attrib -r -h -s ..\source\gblinc\config.hpp
@attrib -r -h -s configs\config_*.hpp
@touch configs\config_p5.hpp
@touch configs\config_p6.hpp
@touch configs\config_k6.hpp
@rem
@rem
@rem ***********************************************************
@rem
@rem   Build versions of Trespass Final for all the platforms
@rem
@rem ***********************************************************
@rem
@rem Build Pentium Version
@rem 
@title Trespass Pentium Version
copy configs\config_p5.hpp ..\source\gblinc\config.hpp
msdev ..\jp2_pc.dsw /make "trespass - win32 final" /rebuild
copy ..\build\final\trespass\trespass.exe ..\release\tpassp5.exe
@rem
@rem Build Pentium Pro Version
@rem 
@title Trespass Pentium-Pro Version
copy configs\config_p6.hpp ..\source\gblinc\config.hpp
msdev ..\jp2_pc.dsw /make "trespass - win32 final p6" /rebuild
copy ..\build\final_p6\trespass\trespass.exe ..\release\tpassp6.exe
@rem
@rem Build K6 2 Version
@rem 
@title Trespass K6-2 Version
copy configs\config_k6.hpp ..\source\gblinc\config.hpp
msdev ..\jp2_pc.dsw /make "trespass - win32 final" /rebuild
copy ..\build\final\trespass\trespass.exe ..\release\tpassk6.exe
@rem
@rem
copy ..\release\tpas*.exe \\jpii_pc\trespas2\drop\smoke\game\setup
@rem
@rem
@rem For AMD K6 debug build
@rem msdev ..\jp2_pc.dsw /make "trespass - win32 debug" /rebuild
@rem copy ..\build\debug\trespass\trespass.exe \\jpii_pc\trespas2\drop\smoke\tools\tpassk6_debug.exe
@rem
@rem ***********************************************************
@rem
@rem   Build versions of GUI App Release for all the platforms
@rem
@rem ***********************************************************
@rem
@rem Build Pentium Version
@rem 
@title GUIApp Pentium Version
copy configs\config_p5.hpp ..\source\gblinc\config.hpp
msdev ..\jp2_pc.dsw /make "GUIApp - win32 release" /rebuild
copy ..\build\release\guiapp\guiapp.exe ..\release\guiapp_p5.exe
@rem
@rem Build Pentium-Pro Version
@rem 
@title GUIApp Pentium-Pro Version
@copy configs\config_p6.hpp ..\source\gblinc\config.hpp
msdev ..\jp2_pc.dsw /make "GUIApp - win32 release" /rebuild
@copy ..\build\release\guiapp\guiapp.exe ..\release\guiapp_p6.exe
@rem
@rem Build K6 2 Version
@rem 
@title GUIApp K6-2 Version
@copy configs\config_k6.hpp ..\source\gblinc\config.hpp
msdev ..\jp2_pc.dsw /make "GUIApp - win32 release" /rebuild
@copy ..\build\release\guiapp\guiapp.exe ..\release\guiapp_k6.exe
@rem
@rem
@rem ***********************************************************
@rem
@rem   Build versions of GUI App Debug for Pentium only
@rem
@rem ***********************************************************
@rem
@rem Build Pentium Version
@rem 
@title Debug GUIApp Pentium Version
copy configs\config_p5.hpp ..\source\gblinc\config.hpp
msdev ..\jp2_pc.dsw /make "GUIApp - win32 debug" /rebuild
copy ..\build\debug\guiapp\guiapp.exe ..\release\debug_guiapp_p5.exe
@rem
copy ..\release\*guiapp*.exe \\jpii_pc\trespas2\drop\smoke\tools
@title Build Complete

