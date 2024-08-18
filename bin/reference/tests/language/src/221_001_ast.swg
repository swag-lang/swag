/**
The most simple way to produce a string which contains the Swag code to compile is with an `#ast` block. An `#ast` block is executed at compile time and the string it returns will be compiled **inplace**.
*/

/**
The `#ast` can be a simple expression with the string to compile.
*/
#test
{
    #ast "var x = 666"
    @assert(x == 666)
}

/**
Or it can be a block, with an explicit `return`
*/
#test
{
    var cpt = 2
    #ast
    {
        const INC = 5
        return "cpt += " ++ INC // Equivalent to 'cpt += 5'
    }

    @assert(cpt == 7)
}

/**
# Struct and enums
`#ast` can for example be used to generate the content of a `struct` or `enum`.
*/
#test
{
    struct MyStruct
    {
        #ast
        {
            return "x, y: s32 = 666"
        }
    }

    var v: MyStruct
    @assert(v.x == 666)
    @assert(v.y == 666)
}

/**
It works with generics too, and can be mixed with static declarations.
*/
#test
{
    struct(T) MyStruct
    {
        #ast
        {
            return "x, y: " ++ @typeof(T).name
        }

        z: string
    }

    var v: MyStruct'bool
    #assert @typeof(v.x) == bool
    #assert @typeof(v.y) == bool
    #assert @typeof(v.z) == string

    var v1: MyStruct'f64
    #assert @typeof(v1.x) == f64
    #assert @typeof(v1.y) == f64
    #assert @typeof(v1.z) == string
}

/**
`#ast` needs to return a *string like* value, which can of course be dynamically constructed. In real life we should use the Core.String struct here. But instead we: it by hand, for the purpose of the example.
*/
#test
{
    #[Swag.Compiler]
    func append(buf: ^u8, val: string)
    {
        var len = 0
        while buf[len]:
            len += 1
        @memcpy(buf + len, @dataof(val), cast(u64) @countof(val) + 1)
    }

    struct Vector3
    {
        #ast
        {
            // We construct the code to compile in this local array
            var buf: [256] u8
            append(buf, "x: f32 = 1\n")
            append(buf, "y: f32 = 2\n")
            append(buf, "z: f32 = 3\n")

            // And returns to the compiler the corresponding *code*
            return cast(string) buf
        }
    }

    var v: Vector3
    @assert(v.x == 1)
    @assert(v.y == 2)
    @assert(v.z == 3)
}

/**
# For example
This is a real life example of an `#ast` usage from the `Std.Core` module. Here we generate a structure which contains all the fields of an original other structure, but where the types are forced to be `bool`.
```swag
struct(T) IsSet
{
    #ast
    {
        // A `StringBuilder` is used to manipulate dynamic strings.
        var str = StrConv.StringBuilder{}

        // We get the type of the generic parameter 'T'
        let typeof = @typeof(T)

        // Then we visit all the fields, assuming the type is a struct (or this will not compile).
        // For each original field, we create one with the same name, but with a `bool` type.
        visit f in typeof.fields
            str.appendFormat("%: bool\n", f.name)

        // Then we return the constructed source code.
        // It will be used by the compiler to generate the content of the `IsSet` struct.
        return str.toString()
    }
}
```
*/

/**
# At global scope
`#ast` can also be called at the global scope.
*/
#ast
{
    const value = 666
    return "const myGeneratedConst = " ++ value
}

/**
But be aware that you must use `#placeholder` in case you are generating global symbols that can be used by something else in the code. This will tell Swag that *this symbol* will exist at some point, so please wait for it to *exist* before complaining.
*/
#placeholder myGeneratedConst // Symbol `myGeneratedConst` will be generated

/**
Here for example, thanks to the `#placeholder`, the `#assert` will wait for the symbol `myGeneratedConst` to be replaced with its real content.
*/
#assert myGeneratedConst == 666
