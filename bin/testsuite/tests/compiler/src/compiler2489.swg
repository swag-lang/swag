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
    let itf: BB = a

    let ptr = cast(*AA) itf
    @assert(ptr == &a)
}