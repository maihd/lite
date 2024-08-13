@echo off

echo > Pulling update from github repo...
git pull https://github.com/maihd/lite master

if not %ErrorLevel%==0 (
    echo Cannot update source from github, please check internet connection and run update.bat again!
)

echo:
call build_clang.bat

:done
echo:
echo > All progression of update done!

if not %ErrorLevel%==0 (
    echo:
    echo > Error occured, please just message print above to troubleshooting!
)
