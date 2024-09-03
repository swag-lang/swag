/**
In Swag, all source files are required to use the `.swg` extension, with the exception of simple script files, which use the `.swgs` extension. All files must be encoded in **UTF-8** to ensure proper handling of text and symbols.

Swag does not support the compilation of individual source files (except for `.swgs` script files). Instead, source code is structured within a **workspace** that comprises one or more **modules**. This approach promotes organized and modular development, allowing for efficient management of complex projects.

For example, `Std` is a workspace that includes all the standard modules provided by Swag.

A module can either be a `dll` (on Windows) or an executable. A single workspace may contain multiple modules, meaning that a workspace usually includes the modules you develop (such as your main executable) as well as all your dependencies, including any external modules.

Typically, the entire workspace is compiled together, ensuring that all modules and their dependencies are up to date and correctly integrated.
*/
