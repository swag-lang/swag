#module "tests.compiler.foreign"

public func tcf_toto()
{

}

public func tcf_toto1(x: s64)
{
    @assert(x == 64)
}

public func tcf_toto2(x, y, z, w: s64)
{
    @assert(x == 1)
    @assert(y == 2)
    @assert(z == 3)
    @assert(w == 4)
}

public func tcf_toto3(a, b, c, d, e, f: s8)
{
    @assert(a == 1)
    @assert(b == 2)
    @assert(c == 3)
    @assert(d == 4)
    @assert(e == 5)
    @assert(f == 6)
}

//#[swag.printbc]
public func tcf_toto4()->string
{
    return "toto"
}

public func tcf_toto5(x: s64)->s64
{
    return x
}

public func tcf_toto6(x: string)->string
{
    return x
}

public func tcf_toto7(a, b, c, d: s64)->s64
{
    return a+b+c+d;
}

public func tcf_toto8(a, b, c, d, e, f: s32)->s32
{
    x := a+b+c+d+e+f
    @assert(x == 21)
    return x
}

public func tcf_toto9(a, b, c, d, e, f: s32)->string
{
    return "toto"
}

public struct tcf_Point
{
    x, y, z: s32
}

//#[swag.printbc]
public func tcf_toto10()->tcf_Point
{
    return @{10, 20}
}

public func tcf_toto11(x: s32)->tcf_Point
{
    return @{10, 20}
}