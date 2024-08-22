/**
Namespaces in Swag allow you to create a global scope for symbols, helping organize your code and avoid naming conflicts. A namespace groups functions, variables, and other declarations under a specific name, making them accessible only through that name unless otherwise specified.
*/

namespace A
{
    // This function is inside the namespace 'A'.
    func a() => 1
}

/**
You can nest namespaces to create hierarchical structures. In this example, `C` is a namespace inside `B`, which is itself inside `A`. This allows for even more granular organization of symbols.
*/

namespace A.B.C
{
    // This function is inside the nested namespace 'A.B.C'.
    func a() => 2
}

#test
{
    // Accessing functions using their full namespace paths.
    @assert(A.a() == 1)          // Calls 'a' from namespace 'A'
    @assert(A.B.C.a() == 2)      // Calls 'a' from nested namespace 'A.B.C'
}

/**
The `using` keyword can be applied to namespaces, allowing you to access symbols without needing to fully qualify them in the current file. This can make code more concise and easier to read by removing the need for repeated namespace references.
*/
using namespace Private
{
    const FileSymbol = 0         // A constant defined inside the 'Private' namespace
}

const B = Private.FileSymbol     // Accessing 'FileSymbol' using the full namespace
const C = FileSymbol             // Direct access to 'FileSymbol' thanks to 'using'

/**
You can also create a private scope using `private` instead of a named namespace. In this case, the compiler generates a unique name for the scope, effectively making all enclosed symbols private to the file.
*/
private
{
    const OtherSymbol = 0        // A constant defined in an unnamed private scope
}

const D = OtherSymbol            // Direct access to 'OtherSymbol' since it is private to the file

/**
By default, all symbols from a Swag source file are exported to other files within the same module. However, using `private` or namespaces helps protect against name conflicts, especially in larger projects where multiple files may define similar or identical symbols.
*/
