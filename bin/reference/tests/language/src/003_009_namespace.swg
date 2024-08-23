/**
# Namespaces in Swag

Namespaces in Swag provide a mechanism to organize and encapsulate symbols such as functions, variables, and types within a specific scope. By grouping related symbols together under a namespace, you can avoid naming conflicts and make your codebase more modular and maintainable. Symbols within a namespace are accessible only through the namespace unless explicitly made available outside of it.
*/

// Define a simple namespace 'A'
namespace A
{
    // Function 'a' is defined within the namespace 'A'.
    func a() => 1
}

/**
# Nested Namespaces

Swag allows you to create nested namespaces, enabling hierarchical organization of symbols. In the following example, namespace `C` is nested inside `B`, which is itself nested inside `A`. This structure facilitates even finer control and organization, particularly useful in large projects.
*/

// Define a nested namespace 'A.B.C'
namespace A.B.C
{
    // Function 'a' is defined within the nested namespace 'A.B.C'.
    func a() => 2
}

#test
{
    // Accessing functions using their fully qualified namespace paths.
    @assert(A.a() == 1)          // Calls the 'a' function from namespace 'A'
    @assert(A.B.C.a() == 2)      // Calls the 'a' function from nested namespace 'A.B.C'
}

/**
# Using `using` with Namespaces

The `using` keyword allows you to import symbols from a namespace into the current scope, eliminating the need to fully qualify those symbols with the namespace name. This makes the code more concise and improves readability, particularly when working with deeply nested namespaces.
*/
using namespace Private
{
    const FileSymbol = 0         // A constant defined within the 'Private' namespace
}

const B = Private.FileSymbol     // Accessing 'FileSymbol' with the full namespace path
const C = FileSymbol             // Direct access to 'FileSymbol' thanks to the 'using' directive

/**
# Private Scopes with `private`

In addition to named namespaces, Swag allows you to define a private scope using the `private` keyword. A private scope creates a unique, unnamed namespace that restricts access to the enclosed symbols to the current file, effectively making them private. This is particularly useful for isolating symbols that should not be accessible outside the file.
*/
private
{
    const OtherSymbol = 0        // A constant defined within an unnamed private scope
}

const D = OtherSymbol            // Direct access to 'OtherSymbol' as it is private to the file

/**
# Exporting Symbols

By default, all symbols defined in a Swag source file are exported and can be accessed by other files within the same module. However, using `private` scopes or explicit namespaces provides a layer of protection against unintentional name conflicts, particularly in larger projects where different files may define symbols with similar or identical names.
*/
