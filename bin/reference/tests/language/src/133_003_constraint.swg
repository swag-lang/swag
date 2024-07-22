/**
Swag provides also a simple way of checking generic parameters, without the need of `#validif`.
A type constraint can be added with 'where' when declaring a generic type. If a function or a struct is instantiated with a type that does not conform to the constraint, then an error will be raised.
*/

#test
{
    // The type constraint is a compile time function (with #[Swag.ConstExpr]) that should return a bool.
    #[Swag.ConstExpr]
    func isS32(t: typeinfo) => t == s32

    // Here we check that the function generic type is 's32' by calling 'isS32'.
    func(var T where isS32(T)) sum(x: T...)->T
    {
        var total = 0'T
        visit it: x do
            total += it
        return total
    }

    // This is ok.
    let res1 = sum's32(1, 2)
    @assert(res1 == 3)

    // But the following would generate an error because the type is 'f32'.
    // let res1 = sum'f32(1, 2)
}

/**
The type constraint can be any compile time expression, as long as the resulting type is `bool`.
So you could do something like this.
*/
#test
{
    #[Swag.ConstExpr]
    func isS32(t: typeinfo) => t == s32
    #[Swag.ConstExpr]
    func isBool(t: typeinfo) => t == bool

    // Here we check that the function generic type is 's32' or 'bool'.
    func(var T where isS32(T) or isBool(T)) myFunc(x: T) => x

    // This is ok.
    @assert(myFunc(5's32) == 5)
    @assert(myFunc(true) == true)
    @assert(myFunc(false) == false)
}

/**
Works also for structs.
*/
#test
{
    #[Swag.ConstExpr]
    func isFloat(t: typeinfo) => t == f32 or t == f64

    struct(var T where isFloat(T)) Point
    {
        x, y: T
    }

    var pt:  Point'f32
    var pt1: Point'f64

    // This will generate a type constraint error.
    //var pt: Point's32
}
