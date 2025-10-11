using Swag

public func tcf_tuple()->{ a, b: s32 }
{
    return {0, 0}
}

public func tcf_toto() {}

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

//#[Swag.PrintBc]
public func tcf_toto4()->string
{
    return "toto"
}

public func tcf_toto5(x: s64)->s64
{
    return x
}

public func tcf_toto55(x: f64)->f64
{
    return x
}

public func tcf_toto6(x: string)->string
{
    return x
}

public func tcf_toto7(a, b, c, d: s64)->s64
{
    return a + b + c + d
}

public func tcf_toto8(a, b, c, d, e, f: s32)->s32
{
    @assert(a == 1)
    @assert(f == 6)
    var x = a + b + c + d + e + f
    @assert(x == 21)
    return x
}

public func tcf_toto9(a, b, c, d, e, f: s32)->string
{
    @assert(a == 1)
    @assert(f == 6)
    return "toto"
}

#[Swag.ExportType("methods")]
public struct tcf_Point { x, y, z: s32 }

public impl tcf_Point
{
    func set(me)
    {
        me.x = 444
        me.y = 555
        me.z = 666
    }
}

//#[Swag.PrintBc]
public func tcf_toto10()->tcf_Point
{
    return {10, 20}
}

public func tcf_toto11(x: s32)->tcf_Point
{
    @assert(x == 666)
    return {10, 20}
}

public func tcf_toto12(a, b, c, d, e, f: s32)->tcf_Point
{
    @assert(a + b + c + d + e + f == 21)
    return {10, 20}
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
    fct(x, y)
}

public func tcf_toto19(v, w, x, y, z: s64, fct: func(s64, s64, s64, s64, s64))
{
    @assert(v == 333)
    @assert(w == 444)
    @assert(x == 555)
    @assert(y == 666)
    @assert(z == 777)
    fct(v, w, x, y, z)
}

//func tcf_assert1(a,b,c,d,e,f: s32)->string
public func tcf_assert1()
{
    @compilererror("message", #curlocation)
}

public func tcf_assert(values: ...)
{
    //tcf_assert1(0,0,0)
}

public namespace tcf
{
    public struct AA {}

    public interface ITcf
    {
        func call(me)->u32;
    }

    public impl ITcf for AA
    {
        func impl call(me)->u32
        {
            return 666
        }
    }
}

public func tcf_join(titi: string, values: s64...)
{
    @assert(titi == "titi")
    for i in values
    {
        //@print(values[i])
        //@print(" ")
        switch i
        {
        case 0:
            @assert(values[i] == 1)
        case 1:
            @assert(values[i] == 2)
        case 2:
            @assert(values[i] == 3)
        case 3:
            @assert(values[i] == 4)
        default:
            @assert(false)
        }
    }
}

public func tcf_join8(titi: string, values: s8...)
{
    @assert(titi == "titi")
    for i in values
    {
        //@print(values[i])
        //@print(" ")
        switch i
        {
        case 0:
            @assert(values[i] == 1)
        case 1:
            @assert(values[i] == 2)
        case 2:
            @assert(values[i] == 3)
        case 3:
            @assert(values[i] == 4)
        default:
            @assert(false)
        }
    }
}

public func tcf_join16(titi: string, values: s16...)
{
    @assert(titi == "titi")
    for i in values
    {
        //@print(values[i])
        //@print(" ")
        switch i
        {
        case 0:
            @assert(values[i] == 1)
        case 1:
            @assert(values[i] == 2)
        case 2:
            @assert(values[i] == 3)
        case 3:
            @assert(values[i] == 4)
        default:
            @assert(false)
        }
    }
}

public func tcf_join32(titi: string, values: s32...)
{
    @assert(titi == "titi")
    for i in values
    {
        //@print(values[i])
        //@print(" ")
        switch i
        {
        case 0:
            @assert(values[i] == 1)
        case 1:
            @assert(values[i] == 2)
        case 2:
            @assert(values[i] == 3)
        case 3:
            @assert(values[i] == 4)
        default:
            @assert(false)
        }
    }
}

const DaysToMonth365 = [0'u16, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365]

//#[Swag.PrintBc]
public func tcf_toto20(a: u16)
{
    //    @assert(a == 2020)
    //    @assert(b == 2)
    var res = DaysToMonth365[a]

    //var slice: const [..] u16 = DaysToMonth365
    //var res1 = slice[a]
}

struct TT
{
    x:   IAllocator
}

func all(itf: IAllocator)
{
    var req: AllocatorRequest
    req.mode = AllocatorMode.Alloc
    req.size = 10

    itf.req(&req)
    @assert(req.address != null)
    req.mode = AllocatorMode.Realloc
    itf.req(&req)
    req.mode = AllocatorMode.Free
    itf.req(&req)
    @assert(req.address == null)
}

public func tcf_alloc(size: u32)
{
    var vv: TT
    if vv.x == null:
        vv.x = @getcontext().allocator
    all(vv.x)
}

public alias ThreadProc = func(*void)->u32

#[Swag.Foreign("kernel32")]
{
    func CreateThread(lpThreadAttributes: #null *void, dwStackSize: u64, lpStartAddress: ThreadProc, lpParameter: *void, dwCreationFlags: u32, lpThreadId: *u32)->*void;
    func WaitForSingleObject(hHandle: *void, dwMilliseconds: u32)->u32;
}

public func tcf_entry(param: [*] void)->u32
{
    var ptr = cast([*] u64) param
    @assert(ptr[1] == 666)
    return 0
}

public func tcf_createThread(lambda: ThreadProc, userParam: u64)
{
    var paramBC: [2] *void
    paramBC[0] = cast() lambda
    paramBC[1] = cast() userParam

    var id         = 0'u32
    var h          = CreateThread(null, 0, @mkcallback(lambda), cast(*void) &paramBC[0], 0, &id)
    const INFINITE = 0xFFFFFFFF
    var ew         = WaitForSingleObject(h, INFINITE)
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

//#[Swag.PrintBc]
public func tcf_toto22(v: s8, w: s16, x: s32, y: string, z: s64, fct: func(s8, s16, s32, string, s64))
{
    @assert(v == 33)
    @assert(w == 44)
    @assert(x == 55)
    @assert(y == "666")
    @assert(z == 777)

    fct(v, w, x, y, z)
}

public func tcf_print(params: ...)
{
    foreach c in params
    {
        //@print(cast(string) c)
        @assert(cast(string) c == "this is a string")
    }
}

//#[Swag.PrintBc]
public func tcf_double()->f64
{
    return 3.14
}

public func tcf_float()->f32
{
    return 3.14
}

public func tcf_toto23(x: string, fct: func(string))
{
    fct(x)
}

public struct T3209
{
    m11:     s32
    m12:     s32
}

public impl T3209
{
    const Identity: T3209{1, 10}
}

public struct AA24 { x, y: s32 }

public func tcf_toto24(v: AA24)
{
    @assert(v.x == 1)
    @assert(v.y == 10)
}

public struct AA25
{
    x:   s16
}

public func tcf_toto25(x, y: s32, v: AA25)
{
    @assert(v.x == 1)
}

public func tcf_toto26(a: string)->{ x: u8, y: u32 }
{
    return {1, 2}
}

public struct Vector2 { x, y: s32 }

public struct Vector2f { x, y: f32 }

public func tcf_toto27(a, b, c: Vector2)
{
    //@print(a.x, " ", b.x, " ", c.x, "\n")
    @assert(a.x == 1.0)
    @assert(b.x == 2.0)
    @assert(c.x == 3.0)
}

func tcf_toto28A(a, b, c, d: Vector2)
{
    //@print(a.x, " ", b.x, " ", c.x, " ", d.x, "\n")
    @assert(a.x == 1.0)
    @assert(b.x == 2.0)
    @assert(c.x == 3.0)
    @assert(d.x == 4.0)
}

public func tcf_toto28(a, b, c, d: Vector2, e: f32 = 0)->{ a: s32 }
{
    //@print(a.x, " ", b.x, " ", c.x, " ", d.x, "\n")
    tcf_toto28A(a, b, c, d)
    @assert(a.x == 1.0)
    @assert(b.x == 2.0)
    @assert(c.x == 3.0)
    @assert(d.x == 4.0)
    return {666}
}

func getLineLineIntersectCoefs1(start0, end0, start1, end1: Vector2f)->{ d0: f32, d1: f32 }
{
    var ua = (end1.x - start1.x) * (start0.y - start1.y)
    var ub = (end0.x - start0.x) * (start0.y - start1.y)
    var d  = (end1.y - start1.y) * (end0.x - start0.x) + (end1.x - start1.x) * (end0.y - start0.y)
    return {ua / d, ub / d}
}

//#[Swag.PrintBc]
public func lineLineIntersect1(start0, end0, start1, end1: Vector2f)->f32
{
    var aa = getLineLineIntersectCoefs1(start0, end0, start1, end1)
    return start0.y
}

public struct AZE
{
    x:   { a: s32 } = {0}
}

#premain
{
}
