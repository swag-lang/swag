#module "tests.compiler.foreign"

public func tcf_tuple()->struct{s32, s32}
{
    return @{0, 0}
}

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
    @assert(a == 1)
    @assert(f == 6)
    x := a+b+c+d+e+f
    @assert(x == 21)
    return x
}

public func tcf_toto9(a, b, c, d, e, f: s32)->string
{
    @assert(a == 1)
    @assert(f == 6)
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
    @assert(x == 666)
    return @{10, 20}
}

public func tcf_toto12(a, b, c, d, e, f: s32)->tcf_Point
{
    @assert(a+b+c+d+e+f == 21)
    return @{10, 20}
}

public func tcf_toto13(xx: s32, x: string)->string
{
    @assert(xx == 1)
    return x
}

public func tcf_toto14(xx, yy: s32, x: string)->string
{
    @assert(xx == 1)
    @assert(yy == 2)
    return x
}

public func tcf_toto15(xx, yy, zz: s32, x: string)->string
{
    @assert(xx == 1)
    @assert(yy == 2)
    @assert(zz == 3)
    return x
}

public func tcf_toto16(x: s32, y: f64)
{
    @assert(x == 456)
    @assert(y == 3.14)
    //@print(cast(s64) x)
    //@print(cast(f64) y)
}

public func tcf_toto17(x: s64, fct: func(s64))
{
    @assert(x == 555)
    fct(x)
}

public func tcf_toto18(x, y: s64, fct: func(s64, s64))
{
    /*@print("toto18: ")
    @print(cast(s64) x)
    @print(" ")
    @print(cast(s64) y)
    @print("\n")*/

    @assert(x == 555)
    @assert(y == 666)
    fct(x,y)
}

public func tcf_toto19(v, w, x, y, z: s64, fct: func(s64, s64, s64, s64, s64))
{
    /*@print("toto19: ")
    @print(cast(s64) v)
    @print(" ")
    @print(cast(s64) w)
    @print(" ")
    @print(cast(s64) x)
    @print(" ")
    @print(cast(s64) y)
    @print(" ")
    @print(cast(s64) z)
    @print("\n")*/

    @assert(v == 333)
    @assert(w == 444)
    @assert(x == 555)
    @assert(y == 666)
    @assert(z == 777)
    fct(v,w,x,y,z)
}

//func tcf_assert1(a,b,c,d,e,f: s32)->string
func tcf_assert1(x, y, z: s32)
{
    @error(#location, "message")
}

public func tcf_assert(values: ...)
{
    //tcf_assert1(0,0,0)
}

public namespace tcf
{
    public struct AA
    {
    }

    public interface ITcf
    {
        call: func(self)->u32
    }

    public impl ITcf for AA
    {
        func call(self)->u32
        {
            return 666
        }
    }
}
