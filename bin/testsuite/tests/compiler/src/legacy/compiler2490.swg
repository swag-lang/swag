#global testerror "Err0202"

struct AA
{
}

interface BB
{
    func fct(me);
}

#test
{
    var a: AA
    var itf: BB

    var ptr = cast(*AA) itf
}