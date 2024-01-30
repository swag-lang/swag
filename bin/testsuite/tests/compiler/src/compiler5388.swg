interface I
{
    mtd const x()
}

struct(T) A
{
    buffer: ^T
}

var G: I

impl A
{
    #[Swag.Inline]
    mtd opIndex(x: s32)->const &T
    {
        return &G
    }
}

func toto()
{
    var arr: A'I
    arr[0].x()
}