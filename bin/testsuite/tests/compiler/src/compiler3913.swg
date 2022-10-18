#global skip

struct(T) A
{
    v: s32
}

impl A
{
    mtd toto(x: const [1] s32, b: f32)
    {
        @print(v, " ", b, " ", @typeof(T).name, "\n")
    }
}

#test
{
    var a: A'(const [1] s32)
    var b: A'([1] s32)
    a.v = 1
    b.v = 2

    a.toto(@[1], 1.0)
    b.toto(@[1], 1.0)
}