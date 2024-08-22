/**
# Interoperability with External Modules
Swag can interoperate with external "modules" (such as DLLs on Windows), which contain exported C functions. This allows Swag code to call functions from these external libraries, providing powerful integration with system-level APIs and third-party libraries.
*/

/**
# Declaring External Functions
To declare an external function, use the `Swag.Foreign` attribute before the function prototype. This attribute specifies the module where the function is located.

* The module name can refer to a Swag-compiled module or an external system module.
* The location of external system modules depends on the operating system.
*/

/**
# Example: Windows API
In the example below, the functions `ExitProcess` and `Sleep` are declared as external functions located in the `kernel32.dll` module on Windows.
*/

#[Swag.Foreign("kernel32")]
func ExitProcess(uExitCode: u32); // Declares the 'ExitProcess' function from 'kernel32.dll'

#[Swag.Foreign("kernel32")]
{
    func Sleep(dwMilliseconds: u32); // Declares the 'Sleep' function from 'kernel32.dll'
}

/**
# Linking to External Libraries
When working with external modules, you must also ensure that the library is linked to the executable. This is necessary for the linker to resolve the references to the external functions.

* Use `#foreignlib` to force a link to the specified library when generating executables.
* This directive tells the compiler to include the specified external module during the linking stage.
*/

#foreignlib "kernel32" // Links the 'kernel32.dll' library to the executable, resolving external function calls.
