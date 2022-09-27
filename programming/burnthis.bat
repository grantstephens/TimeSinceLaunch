REM Program a TSL unit and then add a record to the airtable units database
REM Args: Label serial number of the unit to be programmed

color

REM Update delayseconds to reflect the number of seconds to add to the start time to account for
REM the delay in getting everything programmed into the EEPROM


set delayseconds=4
set firmwarefile=tsl.hex

set tempunparsedstarttimefile=%tmp%\unparsedstarttime.txt
set tempeepromfile=%tmp%\burniteeprom.txt
set tempdeviceidfile=%tmp%\burnitdeviceid.txt
set tempfirmwarehashfile=%tmp%\burnitfirmwarehash.txt
set tempfirmwarerecordfile=%tmp%\burnitfirmwarerecord.txt

REM clear out the rrormessage variable since we use this to see if everything OK
set "errormessage="

REM We have to use delayed explanation here because, well, this is batch
REM https://stackoverflow.com/questions/9102422/windows-batch-set-inside-if-not-working

	



SETLOCAL

:nextserial

set errormessage=

set serialno=

IF "%~1" == "" (
	REM No serial number passed
	SET /p serialno= Serial Number or blank to quit:
) else (
	set serialno=%1
)


REM Test if serialno was set. Note this must be after the IF or it does not work
if "%serialno%"=="" (
	set errormessage=No serial number specified
	goto end 
)

REM Running programming cycle in black background
REM Will turn red or green when done
color

REM Generate a fingerprint of the firmware we just programming
call md5\md5.bat %firmwarefile% >%tempfirmwarehashfile%
set /p firmwarehash=<%tempfirmwarehashfile%



set /p firmwarerecord=<%tempfirmwarerecordfile%

REM  The returned string has embeded quotes. This escapes them so we can use the string. OMG this is so ugly.
REM https://stackoverflow.com/a/562131/3152071
REM Note that I had to convert the quotes to "Q" to get this to work otherwise the IF was choking on all the quoted quotes.
REM No chance of the Q having a collision in this context. 

set firmwarerecordescaped=%firmwarerecord:"=Q%

REM If firmware hash is found, then the returned string from airtable looks {"records":[{"id":"recG6cEJLSYWvHXHU",
REM If not, then it looks like {"records":[]}
REM Lets capture the device ID from the ATMEGA chip
REM https://electronics.stackexchange.com/questions/414087/how-can-you-read-out-the-serial-number-of-an-xmega-chip-in-a-batch-file-during-p
"C:\Program Files (x86)\Atmel\Studio\7.0\atbackend\atprogram.exe" --tool avrispmk2 --interface pdi --device atxmega128b3 read --prodsignature --offset 0x08 --size 11 --format hex --file %tempdeviceidfile%
if errorlevel 1 (
	set errormessage=Error getting device ID from unit XMEGA 
	goto end
)
set /p deviceid=<%tempdeviceidfile%

REM Next lets write fuses on the XMEGA. We turn program RSTDSBL
"C:\Program Files (x86)\Atmel\Studio\7.0\atbackend\atprogram.exe" --tool avrispmk2 --interface pdi --device atxmega128b3 write --fuses --values FFFFFFFFEEF7
if errorlevel 1 (
	set errormessage=Error programming XMEGA fuses
	goto end
)

REM Next lets program the firmware and fuses into the flash on the XMEGA
"C:\Program Files (x86)\Atmel\Studio\7.0\atbackend\atprogram.exe" --tool avrispmk2 --interface pdi --device atxmega128b3 program --flash --chiperase --format hex --verify --file %firmwarefile% 
if errorlevel 1 (
	set errormessage=Error downloading firmware to flash on XMEGA 
	goto end
)

echo Starting time programming sequence at %time%

REM Next lets generate the eeprom block with the current time as start time
eeprom-utils-bin\tsl-make-block %tempeepromfile% -o %delayseconds% | findstr "BURNTIME:" >%tempunparsedstarttimefile%
set /p starttimeline=<%tempunparsedstarttimefile%
REM The time is after the string "Start time:"
REM This funky syntax does a substring starting at pos 9
set "starttime=%starttimeline:~9%"

REM And now program the eeprom block into the XMEGA
"C:\Program Files (x86)\Atmel\Studio\7.0\atbackend\atprogram.exe" --tool avrispmk2 --interface pdi --device atxmega128b3 erase --eeprom program --eeprom --format bin --verify --file %tempeepromfile% 
if errorlevel 1 (
	set errormessage=Error writing eeprom block to XMEGA 
	goto end
)

echo Ending time programming sequence at %time%


REM Starttime in quotes because it has embeded spaces
call airtable-insert.bat "%starttime%" %serialno% %firwarerecordid% %deviceid% %burnthis_airtable_api_key%

if errorlevel 1 (
	set errormessage=Error adding unit record to airtable database
	goto end
)

echo Airtable insert success 

:end

REM testing
REM copy %tempeepromfile% .

REM Clean up after ourselves 
del %tempeepromfile%
del %tempunparsedstarttimefile%
del %tempdeviceidfile%
del %tempfirmwarehashfile%
del %tempfirmwarerecordfile%


@if "%errormessage%"=="" (
	REM Color white on green
	color 27
	@echo SUCCESS
	REM Loop back for next unit if we started in scanning batch mode
	IF "%~1" == "" goto nextserial
) else (
	REM Color white on red
	color 47
	@echo ============================================================================================================
	@echo ERROR !!!  %errormessage% !!!	
)

@ENDLOCAL
