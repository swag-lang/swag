struct AA {}

interface BB
{
    func fct(me);
}

impl BB for AA
{
    func impl fct(me) {}
}

#test
{
    var a:   AA
    let itf: BB = a

    let ptr = cast(*AA) itf
    @assert(ptr == &a)
}
