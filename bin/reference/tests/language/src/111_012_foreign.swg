/**
Swag can interop with external "modules" (dlls under windows), which contain exported C functions.

Put a special attribute `Swag.Foreign` before the function prototype, and specify the module name where the function is located.

The module name can be a swag compiled module, or an external system module (where the location depends on the OS).
*/

/**
In the case below, the function is located in `kernel32.dll` (under windows)
*/
#[Swag.Foreign("kernel32")]
func ExitProcess(uExitCode: u32);

// Like for other attributes, you can use a block.
#[Swag.Foreign("kernel32")]
{
    func Sleep(dwMilliseconds: u32);
}

/**
Note that in the case of an external module, you will have to declare somewhere the imported library too.

`#foreignlib` is here to force a link to the given library (when generating executables).
*/
#foreignlib "kernel32"
