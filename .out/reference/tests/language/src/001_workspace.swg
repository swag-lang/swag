/*

The compile unit of swag is a 'workspace' which contains a variable number of 'modules'.
A module will compile to a static library (not done!), a dynamic library or an executable.

To create a fresh new workspace, type 'swag new -w:<workspacename>' in a Console. This will also create a simple module.

A workspace contains a predefined number of sub folders :
modules/        contains all the modules (sub folders) of that workspace
output/         contains the result of the build
tests/          (optionnal) contains a list of test modules
examples/       (optionnal) contains a list of 'examples' modules
dependencies/   (optionnal) contains a list of external modules needed to compile the workspace

A module is organized also in a predefined way :
modulename/     the folder name of the module is used to create output files
src/            contains the source code
public/         a generated folder that will contain all the exports needed by other modules to use that one

A module contains, at its root, a file named "module.swg". This is a special file that can be used to configure the module. It is mandatory.

To compile a workspace, type 'swag build -w:<workspacename>'. You can omit the workspace name if you call swag from the workspace folder. This will compile 'modules/' and 'examples/'.

To also compile and run the '/tests' folders, type 'swag test -w:<workspacename>'.

*/