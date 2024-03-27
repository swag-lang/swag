/**
# Force compile-time call
`#run` can be used to call a function that is not marked with `#[Swag.ConstExpr]`.
*/

// This time 'isThisRelease' is not marked with 'Swag.ConstExpr'
func isThisRelease() => true

// But this call is still valid because we force the compile time execution with '#run'
#if #run isThisRelease() == false
{
    #error "this should not be called !"
}

/**
So that means that you can call everything compile-time, even a function from an external module, a system function etc.
*/

// This function was not 'prepared' for compile-time evaluation, because there's no
// specific attribute
func sum(values: s32...)->s32
{
    var result = 0's32
    visit v: values do
        result += v
    return result
}

// But you can call it compile-time with '#run'
const SumValue = #run sum(1, 2, 3, 4) + 10
#assert SumValue == 20

/**
# #run block
`#run` is also a special function that will be called by the compiler. You can have as many '#run' block as you want, but be aware that the execution order in that case is random.
*/

/**
It can be used to precompute some global values for example.
*/

// A global variable we would like to initialize in a 'complexe' way.
var G: [5] f32 = undefined

/**
Initialize `G` with `[1,2,4,8,16]` at compile time.
*/
#run
{
    var value = 1'f32
    loop i: @countof(G)
    {
        G[i] = value
        value *= 2
    }
}

/**
`#test` blocks are executed after `#run`, even at compile time (during testing).
So we can test the values of `G` here.
*/
#test
{
    @assert(G[0] == 1)
    @assert(G[1] == 2)
    @assert(G[2] == 4)
    @assert(G[3] == 8)
    @assert(G[4] == 16)
}

/**
This is where we can see that Swag can be used as a scripting language, because if you have a project with just some '#run' blocks, you have in fact a... script.
*/

/**
# #run expression
`#run` can also be used as an expression block.
The return type is deduced from the `return` statement.
*/
#test
{
    const Value = #run
    {
        var result: f32
        loop 10 do
            result += 1
        return result // 'Value' will be of type 'f32'
    }
    #assert Value == 10.0
}

/**
Can also be used to initialize a static array.
*/
#test
{
    const N           = 4
    const PowerOfTwo: [N] s32 = #run
    {
        var arr: [N] s32
        loop i: arr do
            arr[i] = 1 << cast(u32) i
        return arr
    }

    #assert PowerOfTwo[0] == 1
    #assert PowerOfTwo[1] == 2
    #assert PowerOfTwo[2] == 4
    #assert PowerOfTwo[3] == 8
}

/**
Can also be used to initialize a string.

This is legit to return a string that is constructed on the stack, because the '#run' block will make a copy. Remember that a string is a pointer to the bytes (in UTF8 format) and a length.
*/
#test
{
    const MyString: string = #run
    {
        var str: [3] u8
        str[0] = `a`
        str[1] = `b`
        str[2] = str[1] + 1
        return cast(string) str
    }
    #assert MyString == "abc"
}

/**
Can also be used to initialize a plain old data struct. Note that you can also force the struct to be considered as a POD by tagging it with '#[Swag.ConstExpr]'.
*/
#test
{
    struct RGB
    {
        r, g, b: u8
    }
    const White: RGB = #run
    {
        var rgb: RGB = undefined
        rgb.r = 255
        rgb.g = rgb.r
        rgb.b = rgb.r
        return rgb
    }
    #assert White.r == 255 and White.g == 255 and White.b == 255
}
