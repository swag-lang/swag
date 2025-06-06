/**
# `#ast` Block
The `#ast` block is one of the simplest methods to generate Swag code dynamically at compile time. It allows you to write code that, when executed during compilation, produces a string. This string is then compiled **in place** as if it were written directly in the source code.
*/

/**
# Basic `#ast` Usage
A `#ast` block can be as straightforward as a single expression that returns the string to be compiled. This feature is highly useful for injecting dynamically generated code during compilation.
*/
#test
{
    #ast "var x = 666"
    @assert(x == 666)          // The variable 'x' is generated by the `#ast` block and initialized with the value 666.
}

/**
# `#ast` Block with `return`
The `#ast` block can contain more complex logic, including multiple statements and an explicit `return`. The returned string will be compiled at the location of the `#ast` block.
*/
#test
{
    var cpt = 2
    #ast
    {
        const INC = 5
        return "cpt += " ++ INC // Generates the code 'cpt += 5', incrementing 'cpt' by the value of 'INC'.
    }

    @assert(cpt == 7)          // The variable 'cpt' is incremented by 5 as generated by the `#ast` block.
}

/**
# `#ast` for Structs and Enums
The `#ast` block can be used to dynamically generate the content of complex types like `structs` or `enums`. This approach is particularly valuable for creating code structures based on compile-time conditions or parameters.
*/
#test
{
    struct MyStruct
    {
        #ast
        {
            return "x, y: s32 = 666" // Generates two fields 'x' and 'y', both initialized to 666.
        }
    }

    var v: MyStruct
    @assert(v.x == 666)         // Asserts that the generated field 'x' is correctly initialized to 666.
    @assert(v.y == 666)         // Asserts that the generated field 'y' is correctly initialized to 666.
}

/**
# `#ast` with Generics
The `#ast` block can be effectively used with generics, allowing for flexible and reusable code generation patterns. This can be particularly powerful when combined with static declarations.
*/
#test
{
    struct(T) MyStruct
    {
        #ast
        {
            return "x, y: " ++ #typeof(T).name // Generates fields 'x' and 'y' with the type of the generic parameter 'T'.
        }

        z: string          // Additional static declaration that adds a field 'z' of type string.
    }

    var v: MyStruct'bool
    #assert(#typeof(v.x) == bool)     // Asserts that the generated field 'x' is of type 'bool'.
    #assert(#typeof(v.y) == bool)     // Asserts that the generated field 'y' is of type 'bool'.
    #assert(#typeof(v.z) == string)   // Asserts that the static field 'z' is of type 'string'.

    var v1: MyStruct'f64
    #assert(#typeof(v1.x) == f64)     // Asserts that the generated field 'x' is of type 'f64'.
    #assert(#typeof(v1.y) == f64)     // Asserts that the generated field 'y' is of type 'f64'.
    #assert(#typeof(v1.z) == string)  // Asserts that the static field 'z' is of type 'string'.
}

/**
# Constructing Strings in `#ast`
The `#ast` block requires a *string-like* value to be returned, which can be dynamically constructed. In this example, the string is manually built, although a more sophisticated method like `Core.String` is typically recommended.
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
            // Construct the code to compile in this local array
            var buf: [256] u8
            append(buf, "x: f32 = 1\n")
            append(buf, "y: f32 = 2\n")
            append(buf, "z: f32 = 3\n")

            // Return the constructed code to the compiler
            return cast(string) buf
        }
    }

    var v: Vector3
    @assert(v.x == 1)           // Asserts that the generated field 'x' is initialized to 1.
    @assert(v.y == 2)           // Asserts that the generated field 'y' is initialized to 2.
    @assert(v.z == 3)           // Asserts that the generated field 'z' is initialized to 3.
}

/**
# Real-World Example
The following is a practical example from the `Std.Core` module. It demonstrates how an `#ast` block can generate a structure where all the fields of another structure have their types converted to `bool`.
```swag
struct(T) IsSet
{
    #ast
    {
        // A `StringBuilder` is used to manipulate dynamic strings.
        var str = StrConv.StringBuilder{}

        // We get the type of the generic parameter 'T'
        let typeof = #typeof(T)

        // Then we foreach all the fields, assuming the type is a struct (or this will not compile).
        // For each original field, we create one with the same name, but with a `bool` type.
        foreach f in typeof.fields:
            str.appendFormat("%: bool\n", f.name)

        // Then we return the constructed source code.
        // It will be used by the compiler to generate the content of the `IsSet` struct.
        return str.toString()
    }
}
```
*/

/**
# `#ast` at Global Scope
The `#ast` block can also be employed at the global scope to dynamically generate global variables, constants, or other declarations. This feature allows for a high degree of flexibility in defining global entities.
*/
#ast
{
    const value = 666
    return "const myGeneratedConst = " ++ value // Generates a global constant 'myGeneratedConst' with the value 666.
}

/**
# `#placeholder` Usage
When generating global symbols that may be referenced elsewhere in the code, it is necessary to use `#placeholder`. This directive informs Swag that the symbol will be generated later, preventing compilation errors when the symbol is referenced before it exists.
*/
#placeholder(myGeneratedConst) // Declares that the symbol `myGeneratedConst` will be generated.

/**
Here, thanks to the `#placeholder`, the `#assert` will wait for the symbol `myGeneratedConst` to be replaced with its actual value before performing the assertion.
*/
#assert(myGeneratedConst == 666) // Asserts that the generated constant 'myGeneratedConst' equals 666.
