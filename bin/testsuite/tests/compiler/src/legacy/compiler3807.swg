struct A { x, y: s32 }

func aa()
{
    var a: A
    with a
    {
        .x = 0
        .y = 0
    }
}
