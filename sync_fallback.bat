@echo off

echo - Syncing .fallback folder...
:: rmdir .fallback\core
:: rmdir .fallback\user
:: rmdir .fallback\plugins

echo:
echo - Copying changed files from data\core
xcopy data\core .fallback\core /d /e /i /f /y

echo:
echo - Copying changed files from data\user
xcopy data\user .fallback\user /d /e /i /f /y

echo:
echo - Copying changed files from data\plugins
xcopy data\plugins .fallback\plugins /d /e /i /f /y

echo:
if not %ErrorLevel%==0 (
    echo - Sync fallback failed!
) else (
    echo - Sync fallback succeed!
)
