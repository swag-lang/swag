#module "tests.compiler.foreign"
namespace tcf
{
    public func toto()
    {

    }

    public func toto1(x: s64)
    {
        @assert(x == 64)
    }

    public func toto2(x, y, z, w: s64)
    {
        @assert(x == 1)
        @assert(y == 2)
        @assert(z == 3)
        @assert(w == 4)
    }

    public func toto3(a, b, c, d, e, f: s8)
    {
        @assert(a == 1)
        @assert(b == 2)
        @assert(c == 3)
        @assert(d == 4)
        @assert(e == 5)
        @assert(f == 6)
    }

    public func toto4()->string
    {
        return "toto"
    }

    public func toto5(x: s64)->s64
    {
        return x
    }

    public func toto6(x: string)->string
    {
        return x
    }

    public func toto7(a, b, c, d: s64)->s64
    {
        return a+b+c+d;
    }

    public func toto8(a, b, c, d, e, f: s32)->s32
    {
        x := a+b+c+d+e+f
        @assert(x == 21)
        return x
    }

    public func toto9(a, b, c, d, e, f: s32)->string
    {
        return "toto"
    }
}