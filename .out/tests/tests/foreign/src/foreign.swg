using swag

public func tcf_tuple()->{s32, s32}
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

public impl tcf_Point
{
    func set(using self)
    {
        x = 444
        y = 555
        z = 666
    }
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
    @assert(v == 333)
    @assert(w == 444)
    @assert(x == 555)
    @assert(y == 666)
    @assert(z == 777)
    fct(v,w,x,y,z)
}

//func tcf_assert1(a,b,c,d,e,f: s32)->string
public func tcf_assert1()
{
    @errormsg("message", #location)
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

public func tcf_join(titi: string, values: s64...)
{
    @assert(titi == "titi")
    loop i: values
    {
        //@print(values[i])
        //@print(" ")
        switch i
        {
            case 0: @assert(values[i] == 1)
            case 1: @assert(values[i] == 2)
            case 2: @assert(values[i] == 3)
            case 3: @assert(values[i] == 4)
            default: @assert(false)
        }
    }
}

public func tcf_join8(titi: string, values: s8...)
{
    @assert(titi == "titi")
    loop i: values
    {
        //@print(values[i])
        //@print(" ")
        switch i
        {
            case 0: @assert(values[i] == 1)
            case 1: @assert(values[i] == 2)
            case 2: @assert(values[i] == 3)
            case 3: @assert(values[i] == 4)
            default: @assert(false)
        }
    }
}

public func tcf_join16(titi: string, values: s16...)
{
    @assert(titi == "titi")
    loop i: values
    {
        //@print(values[i])
        //@print(" ")
        switch i
        {
            case 0: @assert(values[i] == 1)
            case 1: @assert(values[i] == 2)
            case 2: @assert(values[i] == 3)
            case 3: @assert(values[i] == 4)
            default: @assert(false)
        }
    }
}

public func tcf_join32(titi: string, values: s32...)
{
    @assert(titi == "titi")
    loop i: values
    {
        //@print(values[i])
        //@print(" ")
        switch i
        {
            case 0: @assert(values[i] == 1)
            case 1: @assert(values[i] == 2)
            case 2: @assert(values[i] == 3)
            case 3: @assert(values[i] == 4)
            default: @assert(false)
        }
    }
}

private const DaysToMonth365 = @[0'u16, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365]

//#[swag.printbc]
public func tcf_toto20(a: u16)
{
//    @assert(a == 2020)
//    @assert(b == 2)
    res := DaysToMonth365[a]

    //var slice: const [..] u16 = DaysToMonth365
    //res1 := slice[a]
}

struct TT
{
    x: IAllocator
}

func all(itf: IAllocator)
{
    var req: AllocatorRequest
    req.mode = AllocatorMode.Alloc
    req.size = 10

    itf.alloc(&req)
    @assert(req.address != null)
    req.mode = AllocatorMode.Realloc
    itf.alloc(&req)
    req.mode = AllocatorMode.Free
    itf.alloc(&req)
    @assert(req.address == null)
}

public func tcf_alloc(size: u32)
{
    var vv: TT
    if vv.x == null
        vv.x = @getcontext().allocator
    all(vv.x)
}

#[swag.foreign("kernel32")]
{
    func CreateThread(lpThreadAttributes: *void, dwStackSize: u64, lpStartAddress: *void, lpParameter: *void, dwCreationFlags: u32, lpThreadId : *u32)->*void;
    func WaitForSingleObject(hHandle: *void, dwMilliseconds : u32)->u32;
}

#[swag.callback]
public func tcf_entry(param: *void)
{
    ptr := cast(*u64) param
    @assert(ptr[1] == 666)
}

public func tcf_createThread(lambda: func(*void), userParam: uint)
{
    var paramBC: [2] *void
    paramBC[0] = acast lambda
    paramBC[1] = acast userParam

    id := 0'u32
    h := CreateThread(null, 0, @mkcallback(lambda), cast(*void) &paramBC[0], 0, &id)
    const INFINITE = 0xFFFFFFFF
    ew := WaitForSingleObject(h, INFINITE)
}

public func tcf_callret(fct: func()->s64)->s64
{
    return fct()
}

public func tcf_callret1(v: s64, fct: func(s64)->string)->string
{
    return fct(v)
}

public func tcf_callret2(v: f32, fct: func(f32)->string)->string
{
    return fct(v)
}

public func tcf_manualcall(x: s64)->s64
{
    return x
}

//#[swag.printbc]
public func tcf_toto22(v: s8, w: s16, x: s32, y: string, z: s64, fct: func(s8, s16, s32, string, s64))
{
    @assert(v == 33)
    @assert(w == 44)
    @assert(x == 55)
    @assert(y == "666")
    @assert(z == 777)

    fct(v,w,x,y,z)
}

public func tcf_print(params: ...)
{
    visit c: params
    {
        //@print(cast(string) c)
        @assert(cast(string) c == "this is a string")
    }
}