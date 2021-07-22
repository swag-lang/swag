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
func sum(x, y: s32) => x + y

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

// Type of parameters can be deduced in case of a default value
func sum3(x, y = 0.0)
{
    #assert @typeof(x) == f32
}

enum Values { A; B; }
func toto(x: s32, y = Values.A)
{
    #assert @typeof(y) == Values
}

#test
{
    // Functions can be nested inside other functions.
    // This is not closure but just functions in a sub scope.
    func sub(x, y: s32) => x - y

    // Simple call
    {
        x := sub(5, 2)
        @assert(x == 3)
    }

    // You can name parameters, and don't have to respect parameters order
    // in that case
    {
        x1 := sub(x: 5, y: 2)
        @assert(x1 == 3)
        x2 := sub(y: 5, x: 2)
        @assert(x2 == -3)
    }

    {
        func returnMe(x, y: s32 = 0) => x + y * 2
        @assert(returnMe(x: 10) == 10)
        @assert(returnMe(y: 10) == 20)
    }
}