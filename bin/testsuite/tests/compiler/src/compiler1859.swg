//var g = "toto"
var g = 5's8
var g1 = "toto"
var g2 = "titi"

func tt()
{
    @print("toto")
}

func tt1(x: s64)
{
    @print(x)
}

//#[Swag.PrintBc]
func tt2(x: f64, y: s64)
{
    @print(x)
    @print(y)
}

//#[Swag.PrintBc]
func tt3(x: s64)->s64
{
    return x + 1
}

//#[Swag.PrintBc]
func tt4()->string
{
    return "string"
}

//#[Swag.PrintBc]
func tt5()->const [..] s64
{
    return [10, 20, 30, 40]
}

struct A
{
    x, y: s64 = 5
    z: s64
}

struct B
{
    z: s64 = 5
    y: s32
}

//#[Swag.PrintBc]
func variadic(x: ...)
{
    @print(cast(s64) @countof(x))
}

func variadic1(x: ...)
{
    @print(cast(s64) x[0])
    @print(cast(s64) x[1])
}

func variadic2(z: s32, x: ...)
{
    @print(cast(s64) x[0])
    @print(cast(s64) x[1])
    @print(cast(s64) z)
}

//#[Swag.PrintBc]
func retS()->B
{
    var x: B = {666,777}
    return x
}

struct TT
{
    x: s32
}

impl TT
{
	func opCmp(self, other: TT)->s32
	{
		if self.x < other.x
			return -1
		if self.x > other.x
			return 1
		return 0
	}
}

func aaa(ptr: *s64)
{
    dref ptr = 555
}

//#[Swag.PrintBc]
#test
{
    if false
    {
        {
            var ptr = @getcontext()
        }

        @print(cast(s64) @countof(@args()))
        @print(@args()[0])
        @print("\n")

        {
            var x = 666's64
            aaa(&x)
            @print(x)
            var y = 666's64
            aaa(&y)
            @print(y)
        }
        {
            var x, y = 0
            var diff: s64 = cast(^u32) &y - cast(^u32) &x
            @print(diff)
        }

        {
            var x = 0xFFFFFFFF_FFFFFFFF'u64
            var y = cast(u32) x
            var z = cast(u64) y
            @print(cast(s64) z)
        }

        {
            var x = 1'u64
            var y = cast(f32) x
            @print(cast(f64) y)
        }
        {
            var x = 1'u32
            var y = cast(f32) x
            @print(cast(f64) y)
        }

        {
            var a, b: TT
            a.x = 0; b.x = 1;
            @print(a < b ? "-1" : "?")
            a.x = 1; b.x = 0;
            @print(a < b ? "-1" : "?")

            a.x = 1; b.x = 1;
            @print(a <= b ? "-1" : "?")
            a.x = 2; b.x = 1;
            @print(a > b ? "-1" : "?")
            @print(a >= b ? "-1" : "?")
        }

        {
            var x = [1, 2, 3]
            var y = [1, 3, 3]
            @print(cast(s64) @memcmp(&x[0], &y[0], 3'u64 * 4))
            var z = [1, 2, 3]
            @print(cast(s64) @memcmp(&x[0], &z[0], 3'u64 * 4))
        }

        {
            var x = [1, 2, 3]
            @print(cast(s64) x[0])
            @print(cast(s64) x[1])
            @print(cast(s64) x[2])
            @memset(&x[0], 0, cast(u64) 3 * @sizeof(u32))
            @print(cast(s64) x[0])
            @print(cast(s64) x[1])
            @print(cast(s64) x[2])
        }

        {
            var x = [1, 2, 3]
            var y = [10, 20, 30]
            @memcpy(&x[0], &y[0], cast(u64) 3 * @sizeof(u32))
            @print(cast(s64) x[0])
            @print(cast(s64) x[1])
            @print(cast(s64) x[2])
        }
        {
            var x = 12's64
            @print(cast(f64) x)
        }

        {
            var x = [0, 0]
            @init(&x[0], 2)(1)
            @print(cast(s64) x[0])
            @print(cast(s64) x[1])
        }

        {
            var x = 8's8
            x = ~x
            @print(cast(s64)x)
        }
        {
            var x = 0xFFFF's16
            x = ~x
            @print(cast(s64)x)
        }
        {
            var x = 0's32
            x = ~x
            @print(cast(s64)x)
        }
        {
            var x = 0's64
            x = ~x
            @print(cast(s64)x)
        }

        {
            var x = 8'u64
            x %= 3
            @print(cast(s64)x)
        }
        {
            var x = 8'u32
            x %= 3
            @print(cast(s64)x)
        }
        {
            var x = 8'u16
            x %= 3
            @print(cast(s64)x)
        }
        {
            var x = 8'u8
            x %= 3
            @print(cast(s64)x)
        }

        {
            var x = 8's64
            x %= 3
            @print(cast(s64)x)
        }
        {
            var x = 8's32
            x %= 3
            @print(cast(s64)x)
        }
        {
            var x = 8's16
            x %= 3
            @print(cast(s64)x)
        }
        {
            var x = 8's8
            x %= 3
            @print(cast(s64)x)
        }

        {
            var x = 66.6'f64
            var y = cast(f32) x
            @print(cast(f64) x)
        }

        {
            var x = 66.6'f32
            var y = cast(s32) x
            @print(cast(s64) x)
        }
        {
            var x = 125's32
            var y = cast(f32) x
            @print(cast(f64) x)
        }
        {
            var x = 1'u8
            if(x)
                @print("t")
            else
                @print("f")
        }
        {
            var x = 0'u16
            if(x)
                @print("t")
            else
                @print("f")
        }
        {
            var x = 0'u64
            if(x)
                @print("t")
            else
                @print("f")
        }

        {
            var x = 0x12'u32
            var y = cast(u16) x
            @print(cast(s64) y)
        }

        {
            var x = retS()
            @print(cast(s64) x.y)
            @print(cast(s64) x.z)
        }

        variadic2(555, 666's64, 777's64)

        {
            var x = 10'u64
            x <<= 1
            @print(cast(s64) x)
        }
        {
            var x = 10'u8
            x <<= 1
            @print(cast(s64) x)
        }
        {
            var x = 10'u16
            x <<= 1
            @print(cast(s64) x)
        }
        {
            var x = 10'u32
            x <<= 1
            @print(cast(s64) x)
        }

        {
            var x = 10's64
            x <<= 1
            @print(cast(s64) x)
        }
        {
            var x = 10's8
            x <<= 1
            @print(cast(s64) x)
        }
        {
            var x = 10's16
            x <<= 1
            @print(cast(s64) x)
        }
        {
            var x = 10's32
            x <<= 1
            @print(cast(s64) x)
        }

        {
            var x = 20'u64
            x >>= 1
            @print(cast(s64) x)
        }
        {
            var x = 0xFF'u8
            x >>= 1
            @print(cast(s64) x)
        }
        {
            var x = 20'u16
            x >>= 1
            @print(cast(s64) x)
        }
        {
            var x = 20'u32
            x >>= 1
            @print(cast(s64) x)
        }

        {
            var x = 10's64
            x >>= 1
            @print(cast(s64) x)
        }
        {
            var x = 10's8
            x >>= 1
            @print(cast(s64) x)
        }
        {
            var x = 10's16
            x >>= 1
            @print(cast(s64) x)
        }
        {
            var x = 10's32
            x >>= 1
            @print(cast(s64) x)
        }

        variadic1(666's64, 555's64)
        variadic(666's64)
        variadic(10's64,10's64)

        {
            var x = [1's64, 2, 3]
            var ptr = &x[2]
            ptr = ptr - 1
            @print(dref ptr)
            ptr -= 1
            @print(dref ptr)
            ptr += 2
            @print(dref ptr)
        }
        {
            var ptr = &tt3
            @print(ptr(5))
        }

        {
            var ptr = &tt4
            @print(ptr())
        }
        {
            var x: any = 0
            if(@kindof(x) == s32)
                @print("true")
            else
                @print("false")
        }
        {
            var x: any = 0
            if(@kindof(x) == s64)
                @print("true")
            else
                @print("false")
        }
        {
            var ptr = @alloc(10)
            ptr = @realloc(ptr, 20)
            @free(ptr)
        }
        {
            var x: [4] s64
            x[1] = 666
            @print(x[1])
            var cpt = 2
            @init(&x[0], cpt)
            @print(x[1])
        }

        {
            var x: B
            @print(cast(s64) x.y)
        }

        {
            var x = [1's64, 2]
            var ptr = &x[0]
            @print(ptr[1])
            ptr[1] = 33
            @print(ptr[1])
        }

        {
            var x = [1's64]
            var ptr = &x[0]
            @print(ptr[0])
            ptr[0] = 10
            @print(ptr[0])
        }
        {
            var y: u32
            @print(cast(s64) y)
        }

        {
            var x: A
            @print(x.x)
            @print(x.z)
        }

        @print(g1)
        @print(g2)
        {
            var x = "toto"
            if x == "toto"
                @print("true")
            if x == "toto1"
                @print("true")
            else
                @print("false")
        }

        {
            var x = 0'u64
            var y = 1'u64
            var z = x > y
            @print(cast(s64) z)
        }

        {
            var x = 1'u32
            var y = 0'u32
            var z = x > y
            @print(cast(s64) z)
        }

        @print(tt4())

        {
            var x = tt5()
            @print(x[3])
        }

        @print(tt3(555))
        tt()
        tt1(666)
        tt2(666.0, 50)

        {
            var x = 0'u64
            var y = 1'u64
            var z = x < y
            @print(cast(s64) z)
        }

        {
            var x = 0'u32
            var y = 1'u32
            var z = x < y
            @print(cast(s64) z)
        }

        {
            var ptr = @typeof(s32)
            @print(ptr.fullname)
        }
        {
            var ptr = @typeof(s32)
            @print(cast(s64) ptr.sizeof)
        }
        {
            var x = 0xFFFFFFFF
            x = x >> 1
            @print(cast(s64) x)
        }
        {
            var x = 0xFFFFFFFF_FFFFFFFF
            x = x >> 1
            @print(cast(s64) x)
        }
        {
            var x = 5's8
            x &= 30
            @print(cast(s64) x)
        }
        {
            var x = 5's16
            x &= 7
            @print(cast(s64) x)
        }
        {
            var x = 5's32
            x &= 18
            @print(cast(s64) x)
        }
        {
            var x = 5's64
            x &= 31
            @print(cast(s64) x)
        }

        {
            var x = 5's8
            x |= 30
            @print(cast(s64) x)
        }
        {
            var x = 5's16
            x |= 7
            @print(cast(s64) x)
        }
        {
            var x = 5's32
            x |= 18
            @print(cast(s64) x)
        }
        {
            var x = 5's64
            x |= 31
            @print(cast(s64) x)
        }

        {
            var x = 5's8
            x ^= 3
            @print(cast(s64) x)
        }
        {
            var x = 5's16
            x ^= 3
            @print(cast(s64) x)
        }
        {
            var x = 5's32
            x ^= 3
            @print(cast(s64) x)
        }
        {
            var x = 5's64
            x ^= 3
            @print(cast(s64) x)
        }
        {
            var x = -5's32
            x = x % 3
            @print(cast(s64) x)
        }
        {
            var x = -5's64
            x = x % 3
            @print(cast(s64) x)
        }
        {
            var x = 5'u64
            x = x % 3
            @print(cast(s64) x)
        }
        {
            var x = 5'u64
            x = x % 3
            @print(cast(s64) x)
        }

        {
            var x = -5's32
            x = x / 2
            @print(cast(s64) x)
        }
        {
            var x = -50's64
            x = x / 2
            @print(cast(s64) x)
        }

        {
            var x = 5'u32
            x = x / 2
            @print(cast(s64) x)
        }
        {
            var x = 50'u64
            x = x / 2
            @print(cast(s64) x)
        }

        {
            var x = 5'f32
            x = x / 2
            @print(cast(f64) x)
        }
        {
            var x = 7'f64
            x = x / 2
            @print(cast(f64) x)
        }
        {
            var x = 10'u32
            x = x >> 2
            @print(cast(s64) x)
        }
        {
            var x = 30'u64
            x = x >> 1
            @print(cast(s64) x)
        }

        {
            var x = 3'u32
            x = x << 4
            @print(cast(s64) x)
        }
        {
            var x = 3'u64
            x = x << 2
            @print(cast(s64) x)
        }

        {
            var x = 3'u32
            x = x ^ 5
            @print(cast(s64) x)
        }
        {
            var x = 3'u64
            x = x ^ 5
            @print(cast(s64) x)
        }

        {
            var x = 3'f64
            x = x * 3
            @print(cast(f64) x)
        }

        {
            var x = 3'f32
            x = x * 3
            @print(cast(f64) x)
        }
        {
            var x = 3's32
            x = x * 3
            @print(cast(s64) x)
        }
        {
            var x = 3's64
            x = x * 3
            @print(cast(s64) x)
        }

        {
            var x = 3'f32
            x = x - 1.5
            @print(cast(f64) x)
        }
        {
            var x = 3'f64
            x = x - 1.5
            @print(cast(f64) x)
        }

        {
            var x = 3'f32
            x = x + 1.5
            @print(cast(f64) x)
        }
        {
            var x = 3'f64
            x = x + 1.5
            @print(cast(f64) x)
        }
        {
            var a = 1
            a = a - 5
            @print(cast(s64) a)
        }
        {
            var a = 1's64
            a = a - 5
            @print(cast(s64) a)
        }

        {
            var a = 1
            a = a + 5
            @print(cast(s64) a)
        }
        {
            var a = 1's64
            a = a + 5
            @print(cast(s64) a)
        }

        {
            var a, b = 1
            var x = a and b
            @print(cast(s64) x)
        }

        {
            var a, b = 100
            var x = a or b
            @print(cast(s64) x)
        }

        {
            var a = 6
            a = a | 1
            @print(cast(s64) a)
        }
        {
            var a = 6's64
            a = a | 1
            @print(cast(s64) a)
        }
        {
            var a = 6
            a = a & 4
            @print(cast(s64) a)
        }
        {
            var a = 6's64
            a = a & 4
            @print(cast(s64) a)
        }

        {
            loop 4
                @print(cast(s64) @index)
        }

        {
            var x = -1'f64
            x = -x
            @print(cast(f64) x)
        }

        {
            var x = 1'f32
            x = -x
            @print(cast(f64) x)
        }

        {
            var x = -1's64
            x = -x
            @print(cast(s64) x)
        }

        {
            var x = 1's32
            x = -x
            @print(cast(s64) x)
        }

        {
            var x = 10'f64
            if(x > 1.5)
                @print("a")
            else
                @print("b")
        }

        {
            var x = 1'f32
            if(x > 1.5)
                @print("a")
            else
                @print("b")
        }

        {
            var x = 2's8
            if(x > 1)
                @print("a")
            else
                @print("b")
        }

        {
            var x = 1's64
            if(x > 1)
                @print("a")
            else
                @print("b")
        }

        {
            var x = 0.5'f64
            if(x < 0.1)
                @print("a")
            else
                @print("b")
        }

        {
            var x = 0.5'f32
            if(x < 1.0)
                @print("a")
            else
                @print("b")
        }

        {
            var x = 1's64
            if(x < 1)
                @print("a")
        }

        {
            var x = 5'f32
            x *= -2
            @print(cast(f64) x)
        }

        {
            var x = 5'f64
            x *= -2
            @print(cast(f64) x)
        }

        {
            var x = 5's8
            x *= -2
            @print(cast(s64) x)
        }

        {
            var x = -5's16
            x *= -2
            @print(cast(s64) x)
        }

        {
            var x = -5's32
            x *= -2
            @print(cast(s64) x)
        }

        {
            var x = -5's64
            x *= -5
            @print(cast(s64) x)
        }

        {
            var x = 5'f64
            x -= 2
            @print(cast(f64) x)
        }

        {
            var x = 5'f32
            x -= 2
            @print(cast(f64) x)
        }

        {
            var x = 5's64
            x -= 2
            @print(cast(s64) x)
        }

        {
            var x = 5's32
            x -= 2
            @print(cast(s64) x)
        }

        {
            var x = 5's16
            x -= 2
            @print(cast(s64) x)
        }

        {
            var x = 5's8
            x -= 2
            @print(cast(s64) x)
        }

        {
            var x = 5'f32
            x += 2
            @print(cast(f64) x)
        }

        {
            var x = 5'f64
            x += 2
            @print(cast(f64) x)
        }

        {
            var x = 5'f64
            x /= 2
            @print(cast(f64) x)
        }

        {
            var x = 5'f64
            x /= 2
            @print(cast(f64) x)
        }

        {
            var x = 5'f64
            x /= 2
            @print(cast(f64) x)
        }

        {
            var x = 5'f64
            x /= 2
            @print(cast(f64) x)
        }

        {
            var x = 5'f32
            x /= 2
            @print(cast(f64) x)
        }

        {
            var x = 10'u64
            x /= 2
            @print(cast(s64) x)
        }

        {
            var x = 10'u32
            x /= 2
            @print(cast(s64) x)
        }

        {
            var x = 10'u16
            x /= 2
            @print(cast(s64) x)
        }

        {
            var x = 10'u8
            x /= 2
            @print(cast(s64) x)
        }

        {
            var x = -12's64
            x /= -2
            @print(cast(s64) x)
        }

        {
            var x = -1's64
            x /= 2
            @print(cast(s64) x)
        }

        {
            var x = -12's16
            x /= -2
            @print(cast(s64) x)
        }

        {
            var x = -1's16
            x /= 2
            @print(cast(s64) x)
        }

        {
            var x = -1's32
            x /= 2
            @print(cast(s64) x)
        }

        {
            var x = -10's8
            x /= -2
            @print(cast(s64) x)
        }

        {
            var x = 10
            x /= -2
            @print(cast(s64) x)
        }

        {
            var x = 10
            x /= 2
            @print(cast(s64) x)
        }

        {
            var x, y: ^u8 = null
            y += 1
            var z = x orelse y
            if z != null
                @print("A")
        }

        {
            var x, y: *u8 = null
            var z = x orelse y
            if z != null
                @print("X")
            else
                @print("Y")
        }

        {
            var x = 1's16
            switch x
            {
                case 0: @assert(false)
                case 1: @print("1")
            }
        }

        {
            var x = 1's8
            switch x
            {
                case 0: @assert(false)
                case 1: @print("1")
            }
        }

        {
            var x = 10's8
            switch x
            {
                case 0: @assert(false)
                case 1: @print("1")
                default: @print("2")
            }
        }

        {
            var x = 0's8
            var y = x == 1 ? 10 : 20
            @print(cast(s64) y)
        }

        {
            var x = 0's8
            x += -1
            @print(cast(s64) x)
        }

        {
            var x = 0's16
            x += -1
            @print(cast(s64) x)
        }

        {
            var x = 0's64
            x += -1
            @print(cast(s64) x)
        }

        {
            var x = [1'u8]
            var ptr = &x[0]
            var y = ptr[0]
            @print(cast(s64) y)
        }

        {
            var x = [5'u16]
            var ptr = &x[0]
            var y = ptr[0]
            @print(cast(s64) y)
        }

        {
            var x = 0
            var y = 1
            @print(cast(s64) y)
        }

        {
            var y: u64
            @print(cast(s64) y)
        }

        {
            var y: u16
            @print(cast(s64) y)
        }

        {
            var y: u8
            @print(cast(s64) y)
        }

        {
            var y: u32
            @print(cast(s64) y)
        }

        {
            var cpt = 0
            while cpt < 4
            {
                @print("X")
                cpt += 1
            }
        }
        {
            var x = [1's64, 2, 3]
            var ptr = &x[0]
            @print(ptr[0])
            @print(ptr[1])
        }

        {
            var x = [1's64, 2, 3]
            @print(x[0])
            @print(x[1])
            @print(x[2])
        }

        {
            var a = 5
            if a != 5
                @print("a")
            else
                @print("b")
        }

        {
            var a = 5
            if a == 5
                @print("a")
            else
                @print("b")
        }

        {
            var a = 5
            if a == 4
                @print("a")
            else
                @print("b")
        }

        loop 5 @print("g")

        {
            var x = -6's8
            @print(cast(s64) x)
        }

        {
            let x = -6's64
            @print(cast(s64) x)
            @print(cast(s64) g)
            @print("fripouille")
            @print(666's64)
            @print(7666'f64)
        }
    }
}