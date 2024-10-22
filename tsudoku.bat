@echo off
echo:
echo Thank you for playing tsudoku!

:loop
:: Prompt the player for a response
echo:
set /p input= Would you like the in-game menu enabled (y/n)? 

:: If yes, play with in-game menu enabled
if %input%==y (goto :enabled)
if %input%==Y (goto :enabled)

:: If no, play with in-game menu disabled
if %input%==n (goto :disabled)
if %input%==N (goto :disabled)

:: If player input is invalid, notify them
goto :try_again

:enabled
wsl.exe '~/.cargo/bin/tsudoku'
goto :fin

:disabled
wsl.exe '~/.cargo/bin/tsudoku -n'
goto :fin

:: Notify the player of invalid input, then loop back up to get input again
:try_again
echo That is invalid input. Please try again...
goto :loop

:fin
echo Thank you for playing tsudoku! This window will now close...
timeout 5 > NUL
