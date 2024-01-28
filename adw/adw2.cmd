reg query HKCU\Software\ADW

if errorlevel 1 (
	reg add HKCU\Software\ADW /v counter /d 0
) else (
	for /f "skip=1 tokens=3" %%i in ('reg query HKCU\Software\ADW /v counter') do (
		set \a a=%%i+1
		reg add HKCU\Software\ADW /v counter /d %a% /f
	)
)
