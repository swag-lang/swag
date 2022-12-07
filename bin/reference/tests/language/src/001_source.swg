/**
All swag source files have the '.swg' extension.
They must be encoded in UTF8.

In Swag you cannot compile a single file (with the exception of '.swgs' files which are single script files)
A source tree is organized in a workspace which contains modules.

For example the Swag standard library is in fact a collection of modules in the 'Std' workspace.

A module is a dll (windows) or an executable.

A workspace can contain as many executables as you want, and as many modules as you want.
You will typically compile the full workspace.
*/