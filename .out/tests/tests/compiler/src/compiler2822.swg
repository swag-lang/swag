/*namespace a
{
    func toto1()
    {
    }
}*/

func toto()
{
    @assert(false)
}

#test
{
    a := 0
    a.toto()
}