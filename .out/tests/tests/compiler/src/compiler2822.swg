/*namespace a
{
    func toto1()
    {
    }
}*/

func toto(x: s32)
{
    @assert(false)
}

func toto(x: string)
{
}

#test
{
    a := 0
    a.toto()
}