/**
In Swag, all source files use the `.swg` extension, except for simple scripts, which use the `.swgs` extension. All files must be encoded in **UTF-8**.

Swag does not support compiling individual source files (with the exception of `.swgs` script files). Instead, source code is organized within a **workspace** that contains one or more **modules**.

For instance, `Std` is a workspace that includes all the Swag standard modules.

A module can be either a `dll` (on Windows) or an executable, and a workspace may encompass multiple modules. Therefore, a workspace typically contains both the modules you develop (such as your main executable) and all your dependencies (including any external modules you use).

In most cases, the entire workspace is compiled together.
*/
