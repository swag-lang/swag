/**
# '#ast' Block

The '#ast' block lets you dynamically generate and inject Swag code at compile time.
It produces a string that the compiler treats as if it were written directly in the
source. This allows for dynamic, context-dependent code generation.
*/

/**
# Basic '#ast' Usage
A '#ast' block can return a simple string expression representing Swag code.
*/
#test
{
    #ast "var x = 666"
    @assert(x == 666)
}

/**
# '#ast' Block with 'return'
A '#ast' block can include logic and must return a string to compile.
*/
#test
{
    var cpt = 2
    #ast
    {
        const INC = 5
        return "cpt += " ++ INC
    }
    @assert(cpt == 7)
}

/**
# '#ast' for Structs and Enums
You can use '#ast' inside struct or enum definitions to generate members dynamically.
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

    let v: MyStruct
    @assert(v.x == 666)
    @assert(v.y == 666)
}

/**
# '#ast' with Generics
'#ast' works with generics for flexible and reusable code generation.
*/
#test
{
    struct(T) MyStruct
    {
        #ast
        {
            return "x, y: " ++ #typeof(T).name
        }

        z:   string
    }

    let v: MyStruct'bool
    #assert(#typeof(v.x) == bool)
    #assert(#typeof(v.y) == bool)
    #assert(#typeof(v.z) == string)

    let v1: MyStruct'f64
    #assert(#typeof(v1.x) == f64)
    #assert(#typeof(v1.y) == f64)
    #assert(#typeof(v1.z) == string)
}

/**
# Constructing Strings in '#ast'
'#ast' must return a string. You can construct it dynamically, e.g. by using a buffer.
*/
#test
{
    #[Swag.Compiler]
    func append(buf: [*] u8, val: string)
    {
        var len = 0
        while buf[len] do
            len += 1
        @memcpy(buf + len, @dataof(val), cast(u64) @countof(val) + 1)
    }

    struct Vector3
    {
        #ast
        {
            var buf: [256] u8
            append(buf, "x: f32 = 1\n")
            append(buf, "y: f32 = 2\n")
            append(buf, "z: f32 = 3\n")
            return cast(string) buf
        }
    }

    let v: Vector3
    @assert(v.x == 1)
    @assert(v.y == 2)
    @assert(v.z == 3)
}

/**
# Real-World Example
Example from Std.Core — dynamically generates a struct with all fields of another
struct replaced by bools.

```swag
struct(T) IsSet
{
    #ast
    {
        var str = StrConv.StringBuilder{}
        let typeof = #typeof(T)
        foreach f in typeof.fields:
            str.appendFormat("%: bool\n", f.name)
        return str.toString()
    }
}
````

*/

/**

# '#ast' at Global Scope

'#ast' can generate global declarations dynamically as well.
*/
#ast
{
    const value = 666
    return "const myGeneratedConst = " ++ value
}

#assert(myGeneratedConst == 666)
