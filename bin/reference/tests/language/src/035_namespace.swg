/**
You can create a global scope with a namespace. All symbols inside the namespace will be in the corresponding global scope.
*/

namespace A
{
    func a() => 1
}

/**
You can also specify more than one name. Here `C` will be a namespace inside `B` which is itself inside `A`.
*/

namespace A.B.C
{
    func a() => 2
}

#test
{
    @assert(A.a() == 1)
    @assert(A.B.C.a() == 2)
}

/**
You can also put `using` in front of the namespace to be able to access the content without scoping in the **current** file.
*/
using namespace Private
{
    const FileSymbol = 0
}

const B = Private.FileSymbol
const C = FileSymbol // No need to specify 'Private' because of the 'using'

/**
This is equivalent to `private`, but you don't have to specify a name, the compiler will generate it for you.
*/
private {
    const OtherSymbol = 0
}

const D = OtherSymbol

/**
All symbols from a Swag source file are exported to other files of the same module. So using `private` can protect from name conflicts.
*/
