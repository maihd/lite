@echo off

set TARGET=__edit_with_lite.reg

:: Find lite.exe absolute path, and normalize to seperate with '\\' instead of
:: which is required by .reg file
for /f "delims=" %%A in ('
    PowerShell -NoP "$(Get-Location).Path.Replace('\','\\')"
') do set LITE_EXE=%%A\lite.exe
echo - Absolute path of lite is %LITE_EXE%

:: Generate .reg file
echo - Generating .reg file...
(
    echo Windows Registry Editor Version 5.00

    echo:
    echo [HKEY_CLASSES_ROOT\*\Shell\Lite]
    echo @="Edit with Lite"
    echo "Icon"="\"%LITE_EXE%\""

    echo:
    echo [HKEY_CLASSES_ROOT\*\Shell\Lite\command]
    echo @="\"%LITE_EXE%\" \"%1\""

    echo:
    echo [HKEY_CLASSES_ROOT\Directory\Background\shell\Lite]
    echo @="Edit with Lite"
    echo "Icon"="\"%LITE_EXE%\""

    echo:
    echo [HKEY_CLASSES_ROOT\Directory\Background\shell\Lite\command]
    echo @="\"%LITE_EXE%\" \"%V\""
) > %TARGET%

:: The magic occur here
echo - Requesting add registry to system...
call %TARGET%

:: All done, .reg is no need anymore, delete it!
echo - Removing redundant .reg file...
del %TARGET%

echo - Edit with Lite activation succeed!
