/**
All source files in Swag have the **.swg** extension, except if you write a simple script with the **.swgs** extension.
They must be encoded in **UTF8**.

In Swag you cannot compile a single file (with the exception of **.swgs** script files). The source code is organized in a **workspace** which contains one or multiple **modules**.

For example, the Swag standard library is a collection of modules in the *Std* workspace.

A module is a dll (windows) or an executable, and a workspace can include many of them.
So a workspace will contain the modules you write (like your main executable) but also all your dependencies (some external modules you use).
Typically, the entire workspace is compiled.
*/