REM in order to have 'vsce' to be installed, you need to:
REM - go to https://nodejs.org/en to install Hode.js
REM - open a command line, and type 'npm install -g vsce'
xcopy ..\\web\\imgs\\swag_icon.png images\\ /Y
xcopy ..\\web\\imgs\\syntax.png images\\ /Y
vsce package