public struct A38 { a, r, g, b: u8 }

public func toto38(a, b, c, d: s64, x: A38)
{
    @assert(x.a == 1)
    @assert(x.r == 2)
    @assert(x.g == 3)
    @assert(x.b == 4)
    //@print("toto38: ", x.a, " ", x.r, " ", x.g, " ", x.b, "\n")
}

public func toto38A(a: A38, b, c, d: s64, x: A38)
{
    @assert(a.a == 1)
    @assert(a.r == 2)
    @assert(a.g == 3)
    @assert(a.b == 4)

    @assert(x.a == 1)
    @assert(x.r == 2)
    @assert(x.g == 3)
    @assert(x.b == 4)

    //@print("toto38A a: ", a.a, " ", a.r, " ", a.g, " ", a.b, "\n")
    //@print("toto38A x: ", x.a, " ", x.r, " ", x.g, " ", x.b, "\n")
}
