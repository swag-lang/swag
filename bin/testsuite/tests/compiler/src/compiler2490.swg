#global testerror "Err0202"

struct AA
{
}

interface BB
{
    func fct(self);
}

#test
{
    var a: AA
    var itf: BB

    var ptr = cast(*AA) itf
}