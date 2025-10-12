/**
# Interoperability with External Modules
Swag supports calling external system or third-party library functions,
such as those from Windows DLLs or shared objects on other platforms.
This allows Swag applications to interact directly with system APIs
or external C libraries for extended functionality.
*/

/**
# Declaring External Functions
Use the '#[Swag.Foreign("<module>")]' attribute to declare functions
that exist in external modules (DLLs or shared libraries).
The specified module name must match the external library name.
*/

// Declare functions from the Windows 'kernel32.dll' library
#[Swag.Foreign("kernel32")]
func ExitProcess(uExitCode: u32);

#[Swag.Foreign("kernel32")]
{
    func Sleep(dwMilliseconds: u32);
}

/**
# Example: Windows API Integration
These declarations enable Swag code to directly call Windows API functions
like 'Sleep' or 'ExitProcess' from 'kernel32.dll'.
For instance:
- 'Sleep(1000)' pauses execution for 1 second.
- 'ExitProcess(0)' terminates the running process cleanly.
*/

/**
# Linking to External Libraries
To ensure external calls resolve correctly, use the '#foreignlib' directive.
This instructs the compiler and linker to include the specified module
when building the executable or shared library.
*/

// Link the 'kernel32.dll' library for Windows builds
#foreignlib("kernel32")
