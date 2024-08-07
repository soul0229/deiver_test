> # ts视频合成.bat
```
@echo off
setlocal enabledelayedexpansion

set /a output=1
set "target_folder=%~dp0"
dir /B /TC /O:D /AD > folders_order.txt

REM 遍历当前文件夹下的所有一级子文件夹
for /F "delims=" %%G in (folders_order.txt) do (
REM for /D %%G in (*) do (
    echo folder1: %%G
	set /a file_count=1000
	REM 遍历文件夹中的每个.ts文件
	cd %target_folder%\%%G
	for /R %%A in (.) do (
		echo folder2: %%A
		for %%F in ("%%A\*.ts") do (
			REM 移动文件到目标文件夹
			move "%%F" "%target_folder%\%%G"
		)
	)
	set /a count=1000
	set /a i=0
	REM 遍历当前文件夹下的所有.ts文件
	for %%F in (*.ts) do (
		echo Processing file: !i!
		REM 重命名文件
		ren "!i!.ts" "(!count!).ts"
		REM 增加计数器的值
		echo count = !count!
		echo i = !i!
		set /a count+=1
		set /a i+=1
	)
	copy /b "*.ts" "%target_folder%\!output!.ts"
	cd ..
	rd /s /q "%target_folder%\%%G"
	set /a output+=1
)

for %%F in ("%target_folder%\*.*") do (
    set "extension=%%~xF"
    if /i "!extension!" neq ".ts" (
        if /i "!extension!" neq ".bat" (
            if /i "!extension!" neq ".txt" (
				del "%%F"
			)
        )
    )
)
echo succeed
endlocal
pause
```
