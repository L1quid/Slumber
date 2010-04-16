Building on various platforms:

Windows:
Use the provided VC 8.0 sln file in makefiles/vs80.  This was adapted from the 
stock version provided with jsoncpp to more or less align the compilation flags
(in particular, /MD), and spit out lib names that won't be so confusing.

Mac
python scons.py platform=mac-universal-gcc libs

Linux:
python scons.py platform=linux-gcc libs

