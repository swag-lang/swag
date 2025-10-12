/**
# Source Files and Workspaces

In Swag, all source files must use the '.swg' extension, except for simple script
files, which use the '.swgs' extension. All files must be encoded in **UTF-8**
to ensure proper handling of text and symbols.

Swag does not compile individual source files (except for '.swgs' scripts).
Instead, source code is organized within a **workspace** that contains one or
more **modules**. This modular approach promotes clean structure and efficient
management of complex projects.

For example, 'Std' is a workspace that includes all the standard modules
provided by Swag.

A module can either produce a 'dll' (on Windows) or an executable. A single
workspace may include multiple modules, typically comprising both the modules
you develop (such as your main executable) and any external dependencies.

Generally, the entire workspace is compiled together, ensuring that all modules
and dependencies are properly built, up to date, and fully integrated.
*/
