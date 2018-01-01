To install the DWI performance driver simply run install.bat and 
then re-boot (you will not be able to use the driver until you have).

NOTE: Make sure you are in that directory and the directory is local.

If this fails you will have to do it by hand in the following steps:

1. Copy perf_dwi.sys to winnt\system32\drivers
2. Modify your registry by typing: Regini.exe perf.ini
   DO NOT ATTEMPT TO MODIFY THE REGISTRY BY HAND.
3. re boot your machine.

The driver is started at boot after boot and system devices have
been started. The driver is completely safe as it will be dormant
until started by an application. While dormant the driver consumes
zero processing time and only 200 bytes of memory.