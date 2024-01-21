#global testerror "Err0294"

struct AA
{
}

interface BB
{
    fct: func(self)
}

#test
{
    var a: AA
    var itf: BB

    var ptr = cast(*AA) itf
}