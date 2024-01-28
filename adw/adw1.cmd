@echo OFF

if not exist stats.txt echo 0 0 0 > stats.txt

for /f "tokens=1,2,3" %%i in (stats.txt) do (
	set total=%%i
	set fails=%%j
	set inrow=%%k
)

ping 0.0.0.0

if errorlevel 1 (
	set result=FAIL
	set /a fails+=1
	set /a inrow+=1
) else (
	set result=OK
	set inrow=0
)

if %inrow%==2 (
	set inrow=0
	echo HORI!!!
)

set /a total+=1

echo %date% %time% %result% >> log.txt
echo %total% %fails% %inrow% > stats.txt