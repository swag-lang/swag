/*struct(T) AA
{
    v: [2] T
}

impl AA
{
    func opCast(using self)->const [..] T
    {
        return v
    }
}

func(T) toto(x: const [..] T)
{
}

#test
{
    var x: AA'u32
    toto(x)
}*/