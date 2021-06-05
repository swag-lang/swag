struct AA
{
}

interface BB
{
    fct: func(self)
}

impl BB for AA
{
    func fct(using self)
    {

    }
}

#test
{
    var a: AA
    var itf: BB = a

    ptr := cast(*AA) itf
    @assert(ptr == &a)
}