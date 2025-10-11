struct T
{
    x:     s32 = 10
    y:     s32 = 20
}

func toto(x: string, y: T, z: ...)
{
    //@print(x)
}

#test
{
    toto("toto", {100, 200}, 10, 20)
}
