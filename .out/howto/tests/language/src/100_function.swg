// A function starts with the 'func' keyword
func toto()
{
}

// To return a value, you must add -> followed by the type
func toto1()->s32
{
    return 0
}


// The return type can be deduced in case of a simple expression, with '=>' instead of '->'
func sum2(x, y: s32) => x + y

// A short version exists, in case of a function returning nothing
func print(val: string) = @print(val)

// Parameters
func sum1(x, y: s32, unused: f32)->s32
{
    return x + y
}

// Parameters can have default values
func sum2(x, y: s32, unused: f32 = 666)->s32
{
    return x + y
}