/**
# 'Namespaces in Swag'

Namespaces in Swag provide a structured way to organize symbols — functions,
variables, and types — within a specific scope. Grouping related symbols under
a namespace helps prevent naming conflicts and makes the code more modular
and maintainable.

Symbols in a namespace are accessible only through that namespace unless they
are explicitly imported or exposed.
*/

// Define a simple namespace 'A'
namespace A
{
    // Function 'a' is defined within the namespace 'A'.
    func a() => 1
}

/**
# 'Nested Namespaces'

Swag supports nested namespaces, allowing hierarchical organization of symbols.
This enables fine-grained structuring of code, which is especially helpful in
large projects. In the example below, 'C' is nested inside 'B', which is nested
inside 'A'.
*/

// Define a nested namespace 'A.B.C'
namespace A.B.C
{
    // Function 'a' is defined within the nested namespace 'A.B.C'.
    func a() => 2
}

#test
{
    // Access functions using their fully qualified namespace paths.
    @assert(A.a() == 1) // Calls 'a' from namespace 'A'
    @assert(A.B.C.a() == 2) // Calls 'a' from nested namespace 'A.B.C'
}

/**
# 'Using' with Namespaces

The 'using' keyword imports symbols from a namespace into the current scope.
This eliminates the need to fully qualify symbols, improving readability,
especially when dealing with deeply nested namespaces.
*/

using namespace Private
{
    const FileSymbol = 0 // Constant defined within namespace 'Private'
}

const B = Private.FileSymbol     // Access via fully qualified name
const C = FileSymbol             // Direct access via 'using' directive

/**
# 'Private' Scopes

In addition to named namespaces, Swag provides 'private' scopes. A private
scope creates a unique, unnamed namespace restricted to the current file.
Symbols defined in such a scope are inaccessible outside it, making this
useful for isolating internal details.
*/

private
{
    const OtherSymbol = 0 // Constant defined in a file-local private scope
}

const D = OtherSymbol // Accessible within this file only

/**
# 'Exporting Symbols'

By default, all symbols in a Swag file are exported to other files within
the same module. Using explicit namespaces or private scopes provides
protection against accidental symbol conflicts across files.
*/
