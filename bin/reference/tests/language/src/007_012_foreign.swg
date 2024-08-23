/**
# Interoperability with External Modules
Swag provides the ability to interoperate with external modules, such as Dynamic Link Libraries (DLLs) on Windows, 
which export C functions. This interoperability allows Swag code to invoke functions from these external libraries, 
enabling seamless integration with system-level APIs and third-party libraries. This feature is particularly 
powerful for extending Swag applications with capabilities provided by external systems.
*/

/**
# Declaring External Functions
To use functions from external modules, you declare them in your Swag code with the `Swag.Foreign` attribute. 
This attribute specifies the external module where the function is located. The module name can refer to either 
a Swag-compiled module or an external system module, depending on your needs. The exact location of these external 
modules varies by operating system, so it is important to specify the correct module name based on the target platform.
*/

#[Swag.Foreign("kernel32")]
func ExitProcess(uExitCode: u32);                 // Declares the 'ExitProcess' function from 'kernel32.dll'

#[Swag.Foreign("kernel32")]
{
    func Sleep(dwMilliseconds: u32);              // Declares the 'Sleep' function from 'kernel32.dll'
}

/**
# Example: Windows API
In this example, two functions (`ExitProcess` and `Sleep`) are declared as external functions that reside in the 
`kernel32.dll` module, a core system library on Windows. By declaring these functions with `Swag.Foreign`, you 
can directly invoke Windows API functions within your Swag code, allowing for system-level operations such as 
exiting a process or pausing execution for a specified amount of time.
*/

/**
# Linking to External Libraries
When working with external modules, it is essential to ensure that the corresponding library is linked to the 
final executable. This linking process is crucial because it allows the linker to resolve the references to 
the external functions declared in your code.

* Use the `#foreignlib` directive to specify which external library should be linked during the compilation process.
* This directive informs the Swag compiler to include the specified external module during the linking stage, ensuring 
  that all external function calls are correctly resolved at runtime.
*/

// Links the 'kernel32.dll' library to the executable, resolving external function calls.
#foreignlib "kernel32"    
