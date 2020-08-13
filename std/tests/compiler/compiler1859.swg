//var g = "toto"
var g = 5's8

//#[swag.printbc]
#test
{
    {
        x := 5's8
        x |= 30
        @print(cast(s64) x)
    }
    {
        x := 5's16
        x |= 7
        @print(cast(s64) x)
    }
    {
        x := 5's32
        x |= 18
        @print(cast(s64) x)
    }
    {
        x := 5's64
        x |= 31
        @print(cast(s64) x)
    }

    if false
    {
        {
            x := 5's8
            x ^= 3
            @print(cast(s64) x)
        }
        {
            x := 5's16
            x ^= 3
            @print(cast(s64) x)
        }
        {
            x := 5's32
            x ^= 3
            @print(cast(s64) x)
        }
        {
            x := 5's64
            x ^= 3
            @print(cast(s64) x)
        }
        {
            x := -5's32
            x = x % 3
            @print(cast(s64) x)
        }
        {
            x := -5's64
            x = x % 3
            @print(cast(s64) x)
        }
        {
            x := 5'u64
            x = x % 3
            @print(cast(s64) x)
        }
        {
            x := 5'u64
            x = x % 3
            @print(cast(s64) x)
        }

        {
            x := -5's32
            x = x / 2
            @print(cast(s64) x)
        }
        {
            x := -50's64
            x = x / 2
            @print(cast(s64) x)
        }

        {
            x := 5'u32
            x = x / 2
            @print(cast(s64) x)
        }
        {
            x := 50'u64
            x = x / 2
            @print(cast(s64) x)
        }

        {
            x := 5'f32
            x = x / 2
            @print(cast(f64) x)
        }
        {
            x := 7'f64
            x = x / 2
            @print(cast(f64) x)
        }
        {
            x := 10'u32
            x = x >> 2
            @print(cast(s64) x)
        }
        {
            x := 30'u64
            x = x >> 1
            @print(cast(s64) x)
        }

        {
            x := 3'u32
            x = x << 4
            @print(cast(s64) x)
        }
        {
            x := 3'u64
            x = x << 2
            @print(cast(s64) x)
        }

        {
            x := 3'u32
            x = x ^ 5
            @print(cast(s64) x)
        }
        {
            x := 3'u64
            x = x ^ 5
            @print(cast(s64) x)
        }

        {
            x := 3'f64
            x = x * 3
            @print(cast(f64) x)
        }

        {
            x := 3'f32
            x = x * 3
            @print(cast(f64) x)
        }
        {
            x := 3's32
            x = x * 3
            @print(cast(s64) x)
        }
        {
            x := 3's64
            x = x * 3
            @print(cast(s64) x)
        }

        {
            x := 3'f32
            x = x - 1.5
            @print(cast(f64) x)
        }
        {
            x := 3'f64
            x = x - 1.5
            @print(cast(f64) x)
        }

        {
            x := 3'f32
            x = x + 1.5
            @print(cast(f64) x)
        }
        {
            x := 3'f64
            x = x + 1.5
            @print(cast(f64) x)
        }
        {
            a := 1
            a = a - 5
            @print(cast(s64) a)
        }
        {
            a := 1's64
            a = a - 5
            @print(cast(s64) a)
        }

        {
            a := 1
            a = a + 5
            @print(cast(s64) a)
        }
        {
            a := 1's64
            a = a + 5
            @print(cast(s64) a)
        }

        {
            a, b := 1
            x := a && b
            @print(cast(s64) x)
        }

        {
            a, b := 100
            x := a || b
            @print(cast(s64) x)
        }

        {
            a := 6
            a = a | 1
            @print(cast(s64) a)
        }
        {
            a := 6's64
            a = a | 1
            @print(cast(s64) a)
        }
        {
            a := 6
            a = a & 4
            @print(cast(s64) a)
        }
        {
            a := 6's64
            a = a & 4
            @print(cast(s64) a)
        }

        {
            loop 4
                @print(cast(s64) @index)
        }

        {
            x := -1'f64
            x = -x
            @print(cast(f64) x)
        }

        {
            x := 1'f32
            x = -x
            @print(cast(f64) x)
        }

        {
            x := -1's64
            x = -x
            @print(cast(s64) x)
        }

        {
            x := 1's32
            x = -x
            @print(cast(s64) x)
        }

        {
            x := 10'f64
            if(x > 1.5)
                @print("a")
            else
                @print("b")
        }

        {
            x := 1'f32
            if(x > 1.5)
                @print("a")
            else
                @print("b")
        }

        {
            x := 2's8
            if(x > 1)
                @print("a")
            else
                @print("b")
        }

        {
            x := 1's64
            if(x > 1)
                @print("a")
            else
                @print("b")
        }

        {
            x := 0.5'f64
            if(x < 0.1)
                @print("a")
            else
                @print("b")
        }

        {
            x := 0.5'f32
            if(x < 1.0)
                @print("a")
            else
                @print("b")
        }

        {
            x := 1's64
            if(x < 1)
                @print("a")
        }

        {
            x := 5'f32
            x *= -2
            @print(cast(f64) x)
        }

        {
            x := 5'f64
            x *= -2
            @print(cast(f64) x)
        }

        {
            x := 5's8
            x *= -2
            @print(cast(s64) x)
        }

        {
            x := -5's16
            x *= -2
            @print(cast(s64) x)
        }

        {
            x := -5's32
            x *= -2
            @print(cast(s64) x)
        }

        {
            x := -5's64
            x *= -5
            @print(cast(s64) x)
        }

        {
            x := 5'f64
            x -= 2
            @print(cast(f64) x)
        }

        {
            x := 5'f32
            x -= 2
            @print(cast(f64) x)
        }

        {
            x := 5's64
            x -= 2
            @print(cast(s64) x)
        }

        {
            x := 5's32
            x -= 2
            @print(cast(s64) x)
        }

        {
            x := 5's16
            x -= 2
            @print(cast(s64) x)
        }

        {
            x := 5's8
            x -= 2
            @print(cast(s64) x)
        }

        {
            x := 5'f32
            x += 2
            @print(cast(f64) x)
        }

        {
            x := 5'f64
            x += 2
            @print(cast(f64) x)
        }

        {
            x := 5'f64
            x /= 2
            @assert(cast(f64) x == 2.5)
        }

        {
            x := 5'f64
            x /= 2
            @assert(cast(f64) x == 2.5)
        }

        {
            x := 5'f64
            x /= 2
            @assert(cast(f64) x == 2.5)
        }

        {
            x := 5'f64
            x /= 2
            @assert(cast(f64) x == 2.5)
        }

        {
            x := 5'f32
            x /= 2
            @assert(cast(f64) x == 2.5)
        }

        {
            x := 10'u64
            x /= 2
            @assert(cast(s64) x == 5)
        }

        {
            x := 10'u32
            x /= 2
            @assert(cast(s64) x == 5)
        }

        {
            x := 10'u16
            x /= 2
            @assert(cast(s64) x == 5)
        }

        {
            x := 10'u8
            x /= 2
            @assert(cast(s64) x == 5)
        }

        {
            x := -12's64
            x /= -2
            @print(cast(s64) x)
        }

        {
            x := -1's64
            x /= 2
            @print(cast(s64) x)
        }

        {
            x := -12's16
            x /= -2
            @print(cast(s64) x)
        }

        {
            x := -1's16
            x /= 2
            @print(cast(s64) x)
        }

        {
            x := -1's32
            x /= 2
            @print(cast(s64) x)
        }

        {
            x := -10's8
            x /= -2
            @print(cast(s64) x)
        }

        {
            x := 10
            x /= -2
            @print(cast(s64) x)
        }

        {
            x := 10
            x /= 2
            @print(cast(s64) x)
        }

        {
            var x, y: *u8 = null
            y += 1
            z := x ?? y
            if z != null
                @print("A")
        }

        {
            var x, y: *u8 = null
            z := x ?? y
            if z != null
                @print("X")
            else
                @print("Y")
        }

        {
            x := 1's16
            switch x
            {
                case 0: @assert(false)
                case 1: @print("1")
            }
        }

        {
            x := 1's8
            switch x
            {
                case 0: @assert(false)
                case 1: @print("1")
            }
        }

        {
            x := 10's8
            switch x
            {
                case 0: @assert(false)
                case 1: @print("1")
                default: @print("2")
            }
        }

        {
            x := 0's8
            y := x == 1 ? 10 : 20
            @print(cast(s64) y)
        }

        {
            x := 0's8
            x += -1
            @print(cast(s64) x)
        }

        {
            x := 0's16
            x += -1
            @print(cast(s64) x)
        }

        {
            x := 0's64
            x += -1
            @print(cast(s64) x)
        }

        {
            x := @[1'u8]
            ptr := &x[0]
            y := ptr[0]
            @print(cast(s64) y)
        }

        {
            x := @[5'u16]
            ptr := &x[0]
            y := ptr[0]
            @print(cast(s64) y)
        }

        {
            x := 0
            y := 1
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
            cpt := 0
            while cpt < 4
            {
                @print("X")
                cpt += 1
            }
        }
        {
            x := @[1's64, 2, 3]
            ptr := &x[0]
            @print(ptr[0])
            @print(ptr[1])
        }

        {
            x := @[1's64, 2, 3]
            @print(x[0])
            @print(x[1])
            @print(x[2])
        }

        {
            a := 5
            if a != 5
                @print("a")
            else
                @print("b")
        }

        {
            a := 5
            if a == 5
                @print("a")
            else
                @print("b")
        }

        {
            a := 5
            if a == 4
                @print("a")
            else
                @print("b")
        }

        loop 5 @print("g")

        {
            x := -6's8
            @assert(cast(s64) x == -6)
        }

        {
            x := -6's64
            @print(cast(s64) x)
            @print(cast(s64) g)
            @print("fripouille")
            @print(666's64)
            @print(7666'f64)
        }
    }
}