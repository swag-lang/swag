/**
A function declaration usually starts with the `func` keyword followed by the function name.
*/
func toto()
{
}

/**
If the function needs to return a value, you must add `->` followed by the type.
*/
func toto1()->s32
{
    return 0
}

/**
The return type can be deduced in case of a simple expression, by using `=>` instead of `->`.
Here the return type will be deduced to be `s32`.
*/
func sum(x, y: s32) => x + y

/**
A short version exists, in case of a function returning nothing.
*/
func print(val: string) = @print(val)

/**
Parameters are specified after the function name, between parenthesis.
Here we declare two parameters `x` and `y` of type `s32`, and one last parameter of type `f32`.
*/
func sum1(x, y: s32, unused: f32)->s32
{
    return x + y
}

/**
Parameters can have default values.
*/
func sum2(x, y: s32, unused: f32 = 666)->s32
{
    return x + y
}

/**
The type of the parameters can be deduced if you specify a default value.
Here `x` and `y` have the type `f32` because `0.0` is a 32 bits floating point literal.
*/
func sum3(x, y = 0.0)
{
    #assert @typeof(x) == f32
    #assert @typeof(y) == f32
}

enum Values { A; B; }
func toto(x: s32, y = Values.A)
{
    #assert @typeof(y) == Values
}

#test
{
    // Functions can be nested inside other functions. This are not closure but just functions in a sub scope.
    func sub(x, y: s32) => x - y

    // Simple call.
    {
        x := sub(5, 2)
        @assert(x == 3)
    }
}

/**
You can name parameters, and don't have to respect parameters order in that case.
*/
#test
{
    func sub(x, y: s32) => x - y

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

/**
# Multiple return values
*/
#test
{
    // A **tuple** can be used to return multiple values in a function.
    {
        func myFunction() -> {f32, f32}
        {
            return {1.0, 2.0}
        }

        result := myFunction()
        @assert(result.item0 == 1.0)
        @assert(result.item1 == 2.0)

        var (x, y) = myFunction()
        @assert(x == 1.0)
        @assert(y == 2.0)

        (z, w) := myFunction()
        @assert(z == 1.0)
        @assert(w == 2.0)
    }

    // You can name the fields of the function result too
    func returns2() -> {x: s32, y: s32}
    {
        // You can return a tuple literal as long as the types match
        if false
            return {1, 2}

        // Or use the specifal type 'retval' which is an alias to the
        // function return type (but not only, we'll see later)
        using var result: retval
        x = 1
        y = 2
        return result
    }

    // You can access the tuple fields with the names (if specified), or with 'item?'
    result := returns2()
    @assert(result.x == 1)
    @assert(result.item0 == 1)
    @assert(result.y == 2)
    @assert(result.item1 == 2)

    // You can deconstruct the returned tuple
    (x, y) := returns2()
    @assert(x == 1)
    @assert(y == 2)
}