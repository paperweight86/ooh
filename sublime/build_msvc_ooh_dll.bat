@echo off

set vc_ver=14.0
set vc_dir=C:\Program Files (x86)\Microsoft Visual Studio %vc_ver%\VC
pushd %vc_dir%
call vcvarsall.bat x64
popd

set out_dir=%1
set file_dir=%2
set file_name=%3
set inc_dirs=%4 /I%5

echo %*

if not exist %out_dir% (
  echo "out dir is missing %out_dir%"
  goto error
)
if not exist %file_dir% (
  echo "file dirctory is missing %file_dir%"
  goto error
)
rem if not exist %file_dir%\%file_name% (
rem   echo "out file is missing %file_dir%\%file_name%"
rem   goto error
rem )

rem /WX
set cl_flags=/GS /W3 /Zc:wchar_t /ZI -Gm- -GR- /Od /sdl /Zc:inline /fp:precise /I%inc_dirs% /Tp%file_dir%\%file_name%.ooh
set link_flags=/link /incremental:no /opt:ref /pdb:%out_dir%\%file_name%_%random%.pdb /OUT:%out_dir%\%file_name%.dll /DLL user32.lib
del %out_dir%\%file_name%_*.pdb
set cl_cmd_all=%cl_flags% %link_flags%
echo %cl_cmd_all%
cl.exe %cl_cmd_all%
del %out_dir%\%file_name%.lib
del %out_dir%\%file_name%.exp
del %out_dir%\%file_name%.obj
goto end

:error
echo error not found %ferror_text%
set errorlevel=255

:end

popd
