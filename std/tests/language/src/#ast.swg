#test
{
    // '#ast' is executed at compile time and returns a string that will be compiled inplace
    {
        #ast "x := 666"
        @assert(x == 666)
    }

    {
        cpt := 2
        #ast
        {
            const INC = 5
            return "cpt += " ~INC
        }

        @assert(cpt == 7)
    }

    // '#ast' can be used to generate the content of a struct/enum
    {
        struct MyStruct
        {
            #ast
            {
                return "x, y: s32 = 666"
            }
        }

        var v: MyStruct
        @assert(v.x == 666)
        @assert(v.y == 666)
    }

    // Works with generics too, and can be mixed with static declarations
    {
        struct(T) MyStruct
        {
            #ast
            {
                return "x, y: " ~ @typeof(T).name
            }

            z: string
        }

        var v: MyStruct'bool
        @assert(@typeof(v.x) == bool)
        @assert(@typeof(v.y) == bool)
        @assert(@typeof(v.z) == string)

        var v1: MyStruct'f64
        @assert(@typeof(v1.x) == f64)
        @assert(@typeof(v1.y) == f64)
        @assert(@typeof(v1.z) == string)
    }

    // '#ast' needs to return a 'string like' value, that can be dynamic
    {
        func append(buf: *u8, val: string)
        {
            len := 0
            while buf[len] len += 1
            @memcpy(buf + len, @dataof(val), cast(uint) @countof(val) + 1)
        }

        struct Vector3
        {
            #ast
            {
                var buf: [256] u8
                append(buf, "x: f32 = 1\n")
                append(buf, "y: f32 = 2\n")
                append(buf, "z: f32 = 3\n")
                return cast(string) buf
            }
        }

        var v: Vector3
        @assert(v.x == 1)
        @assert(v.y == 2)
        @assert(v.z == 3)
    }
}