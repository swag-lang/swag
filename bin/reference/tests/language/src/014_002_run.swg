/**
# Force Compile-Time Execution with `#run`

The `#run` directive allows you to invoke a function at compile time, even if that function
is not marked with the `#[Swag.ConstExpr]` attribute. This powerful feature enables
compile-time execution of any function, regardless of its original design or intent,
whether it comes from external modules, system libraries, or is defined within your code.
*/

// The function 'isThisRelease' is not marked with 'Swag.ConstExpr', which means
// it is not specifically prepared for compile-time evaluation.
func isThisRelease() => true

// However, by using the `#run` directive, we force the function to be executed
// at compile time. In this example, since 'isThisRelease()' returns `true`, the
// condition `isThisRelease() == false` evaluates to `false`, and the code block
// is excluded from the compilation process.
#if #run isThisRelease() == false
{
    #error "this should not be called!"
}

/**
This capability allows you to execute any function at compile time, whether it
is a system function, a function from an external module, or a user-defined function.
*/

// The function 'sum' is a regular function that sums a variable number of integers.
// It is not explicitly marked for compile-time evaluation, as it lacks the
// 'Swag.ConstExpr' attribute.
func sum(values: s32...) -> s32
{
    var result = 0's32
    foreach v in values:
        result += v
    return result
}

// Despite the absence of 'Swag.ConstExpr', we can still execute 'sum' at compile time
// using the `#run` directive. The expression `#run sum(1, 2, 3, 4) + 10` is evaluated
// during compilation, and the result is assigned to 'SumValue'.
const SumValue = #run sum(1, 2, 3, 4) + 10
#assert(SumValue == 20)

/**
# `#run` Block

The `#run` directive can also be used in a block format. When placed inside a block,
`#run` enables you to execute complex logic or initialize global variables at compile time.
Multiple `#run` blocks can exist in a program, but the execution order is undefined,
so care must be taken when relying on the order of these blocks.
*/

/**
An example of using `#run` to precompute global values at compile time.
*/

// A global array 'G' that we intend to initialize using a more complex logic.
var G: [5] f32 = undefined

/**
The `#run` block below initializes the global array `G` with the values `[1, 2, 4, 8, 16]`
at compile time, ensuring that the array is fully prepared before runtime.
*/
#run
{
    var value = 1'f32
    for i in @countof(G)
    {
        G[i] = value
        value *= 2
    }
}

/**
`#test` blocks are executed after `#run` blocks, even when run at compile time
(during testing). This allows you to validate the correctness of compile-time
calculations, as demonstrated below by verifying the contents of `G`.
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
The flexibility of Swag allows it to function as a scripting language.
In fact, if your project only contains `#run` blocks, you are effectively
writing a script that runs during compilation.
*/

/**
# `#run` Expression

The `#run` directive can also be used as an expression block. The return type
of the block is inferred from the `return` statement within it.
*/
#test
{
    const Value = #run
    {
        var result: f32
        for 10:
            result += 1
        return result // The inferred type of 'Value' will be 'f32'.
    }
    #assert(Value == 10.0)
}

/**
This technique can also be utilized to initialize a static array.
*/
#test
{
    const N = 4
    const PowerOfTwo: [N] s32 = #run
    {
        var arr: [N] s32
        for i in arr:
            arr[i] = 1 << cast(u32) i
        return arr
    }

    #assert(PowerOfTwo[0] == 1)
    #assert(PowerOfTwo[1] == 2)
    #assert(PowerOfTwo[2] == 4)
    #assert(PowerOfTwo[3] == 8)
}

/**
String initialization is another use case for `#run` blocks. The block
below demonstrates how to construct a string at compile time. This is safe
because the `#run` block creates a copy of the string, ensuring it persists
beyond the block's execution.
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
`#run` blocks can also initialize plain old data (POD) structs. If necessary,
you can enforce POD status on a struct by tagging it with `#[Swag.ConstExpr]`.
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
    #assert(White.r == 255 and White.g == 255 and White.b == 255)
}

/**
> NOTE:
> It is possible to convert a complex struct (e.g., one that uses the heap) into
> a static array, provided the struct implements the `opCount` and `opSlice` methods.
> In this case, the resulting type will be a static array. The compiler will invoke
> `opCount` to determine the array size and `opSlice` to initialize its content.
> If the struct also implements `opDrop`, it will be called after the array
> conversion is complete.
*/
