struct AA
{
}

interface BB
{
    func fct(self)
}

impl BB for AA
{
    func impl fct(using self)
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