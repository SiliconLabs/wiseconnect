::::::::::::::::::::::::::::::::::::::::::ISP_SCRIPT_START::::::::::::::::::::::::::::::::::::::::::
::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
@echo off
:::Get current project directory and keil installation directory from keil after build arguments::::
@set PROJECT_DIR=%1
@set KEIL_ARM=%2
:::::::::::::::Cut the project name and project directory form obtained arguments:::::::::::::::::::
SETLOCAL ENABLEDELAYEDEXPANSION
@set PROJECT_NAME=%PROJECT_DIR:*Objects\=%
@set PROJECT_PATH=!PROJECT_DIR!
@echo %PROJECT_NAME%>x&FOR %%? IN (x) DO SET /A strlength=%%~z? - 2&del x
set PROJECT_PATH=!PROJECT_PATH:~0,-%strlength%!

move /y %PROJECT_DIR%.hex %PROJECT_DIR%.axf >nul
:::::::::::::::::::::::::::::::::::::::::add fromelf to PATH::::::::::::::::::::::::::::::::::::::::
@set PATH=%KEIL_ARM%\ARMCC\bin;%path%
::::::::::::::::Generate project.bin and project.asm to ISP generation script path::::::::::::::::::
fromelf --bin %PROJECT_DIR%.axf -o %~dp0\%PROJECT_NAME%.bin

fromelf -c -s %PROJECT_DIR%.axf -o %~dp0\%PROJECT_NAME%.asm
::::::::::::::::::::::set current directory to ISP generation script path:::::::::::::::::::::::::::
cd "%~dp0"
:::::::::::::::::::::::::calculating and adding CRC to the project.bin::::::::::::::::::::::::::::::
srec_cat.exe %PROJECT_NAME%.bin -Binary -byte-swap 4 -o demo_image_hex -MEM 32
::for /f "skip=10 delims=*" %%a in (demo_image_hex) do echo ^0x%%a^,>>new.txt
ssed.exe "s/.*/0x&,/" demo_image_hex > new.txt
ssed.exe "1,10d" new.txt > demo_image_hex
::move /y "new.txt" "demo_image_hex" >nul
gcc -DM4_CRC -I ./ ./gen_isp/prepare_image_header.c ./gen_isp/crc_calc.c -o crc_header
"crc_header.exe" > header.h
gcc -I ./ -c ./gen_isp/image_header.c -o header 
objcopy  -O binary header -j .data crc.bin
move /y "crc.bin" "%PROJECT_NAME%.bin" >nul
del header.h
@echo "M4 CRC IMAGE READY"
:::::::::::::::taking the starting address from scatter file generated from keil::::::::::::::::::::
FOR /f "tokens= 2" %%i in ('findstr /b "LR_IROM1" %PROJECT_DIR%.sct') do set address=%%i
@echo Generating .RPS format from address %address% 
:::::::::::::creating the ISP header with starting address magic no and IVT offset::::::::::::::::::
gcc gen_isp/foperation.c -o F_operation
"F_operation.exe" 1 offset.h %address% 0x0
gcc -I ./ gen_isp/boot_desc.c -o desc
objcopy  -O binary desc.exe -j .secondb desc.bin
"F_operation.exe" 0 desc.bin 4032
type desc.bin %PROJECT_NAME%.bin > m4_image.bin
srec_cat.exe m4_image.bin -Binary -byte-swap 4 -o pmemdata_qspi_hex -MEM 32
::for /f "skip=10 delims=*" %%a in (pmemdata_qspi_hex) do echo ^0x%%a^,>>new.txt
::move /y "new.txt" "pmemdata_qspi_hex" >nul
ssed.exe "s/.*/0x&,/" pmemdata_qspi_hex > new.txt
ssed.exe "1,10d" new.txt > pmemdata_qspi_hex
::::::::::::calculating CRC for the whole image (ISP header + project.bin with crc):::::::::::::::::
gcc -I ./ -I./gen_isp/ ./gen_isp/prepare_image_header.c ./gen_isp/crc_calc.c  -o prepare_header
set /a offset = %address% - 4096
set /a head_address = %address% - 134221824
"prepare_header.exe" %head_address% 1 0 9 > header.h
gcc -I ./ -c ./gen_isp/image_header.c -o header 
objcopy  -O binary header -j .data %PROJECT_NAME%_isp.bin
@echo ISP format Generated successfully
srec_cat.exe %PROJECT_NAME%_isp.bin -Binary -offset %offset% -o %PROJECT_NAME%.hex -Intel
:::::::::::::::::copy the generated binaries to respective folders in current project:::::::::::::::
xcopy /s/y/I %PROJECT_NAME%.hex %PROJECT_PATH%..\Objects\
xcopy /s/y/I %PROJECT_NAME%.bin %PROJECT_PATH%..\output\
xcopy /s/y 	 %PROJECT_NAME%.asm %PROJECT_PATH%..\output
xcopy /s/y 	 %PROJECT_NAME%_isp.bin %PROJECT_PATH%..\output
::::::::::::::::::delete the intermediate files generated by the script:::::::::::::::::::::::::::::
del /f %PROJECT_NAME%.hex, %PROJECT_NAME%.bin, %PROJECT_NAME%.asm, %PROJECT_NAME%_isp.bin, desc.exe
del /f header, header.h, desc.bin, m4_image.bin, append_hex, pmemdata_qspi_hex,  prepare_header.exe
del /f pmemdata_append_hex, F_operation.exe, offset.h, demo_image_hex, crc_header.exe, new.txt
::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
:::::::::::::::::::::::::::::::::::::::::::ISP_SCRIPT_END:::::::::::::::::::::::::::::::::::::::::::

