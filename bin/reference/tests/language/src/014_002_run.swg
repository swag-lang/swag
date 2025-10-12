/**
# 'Force Compile-Time Execution with #run'

The '#run' directive allows any function to execute at compile time, even if
it’s not marked with '#[Swag.ConstExpr]'. This means you can trigger compile-time
execution of regular, external, or system functions as part of your program.
*/

// Regular runtime function
func isThisRelease() => true

// Forcing compile-time evaluation with '#run'
#if #run isThisRelease() == false
{
    #error("this should not be called!")
}

/**
Any function — including system or user-defined ones — can be executed
at compile time using '#run'.
*/

// Example: sum without 'ConstExpr'
func sum(values: s32...)->s32
{
    var result = 0's32
    foreach v in values do
        result += v
    return result
}

// Force execution at compile time
const SumValue = #run sum(1, 2, 3, 4) + 10
#assert(SumValue == 20)

/**
# '#run' Block

'#run' blocks execute arbitrary logic at compile time. They are useful for
initializing globals or precomputing data before runtime.

Execution order between '#run' blocks is undefined, so avoid relying on order.
*/

// Global array initialized at compile time
var G: [5] f32 = undefined

#run
{
    var value = 1'f32
    for i in @countof(G)
    {
        G[i] = value
        value *= 2
    }
}

// Validate precomputed results
#test
{
    @assert(G[0] == 1)
    @assert(G[1] == 2)
    @assert(G[2] == 4)
    @assert(G[3] == 8)
    @assert(G[4] == 16)
}

/**
Swag can act like a scripting language: if a project only contains '#run'
blocks, it behaves like a compile-time script.
*/

/**
# '#run' Expression

'#run' can also be used as an expression block. Its return type is inferred
from the 'return' statement inside the block.
*/
#test
{
    const Value = #run
    {
        var result: f32
        for 10 do
            result += 1
        return result
    }
    #assert(Value == 10.0)
}

/**
Example: initializing a static array at compile time.
*/
#test
{
    const N           = 4
    const PowerOfTwo: [N] s32 = #run
    {
        var arr: [N] s32
        for i in arr do
            arr[i] = 1 << cast(u32) i
        return arr
    }

    #assert(PowerOfTwo[0] == 1)
    #assert(PowerOfTwo[1] == 2)
    #assert(PowerOfTwo[2] == 4)
    #assert(PowerOfTwo[3] == 8)
}

/**
Example: compile-time string construction.
*/
#test
{
    const MyString: string = #run
    {
        var str: [3] u8
        str[0] = 'a'
        str[1] = 'b'
        str[2] = str[1] + 1
        return cast(string) str
    }
    #assert(MyString == "abc")
}

/**
Example: initializing a struct in a '#run' block.
*/
#test
{
    struct RGB { r, g, b: u8 }

    const White: RGB = #run
    {
        var rgb: RGB = undefined
        rgb.r = 255
        rgb.g = rgb.r
        rgb.b = rgb.r
        return rgb
    }

    @assert(White.r == 255 and White.g == 255 and White.b == 255)
}

/**
> NOTE:
> Complex structs that implement 'opCount' and 'opSlice' can be converted to
> static arrays at compile time. The compiler calls 'opCount' for array size,
> 'opSlice' for initialization, and 'opDrop' after conversion if present.
*/
