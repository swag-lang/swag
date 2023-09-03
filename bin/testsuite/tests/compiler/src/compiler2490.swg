#global testerror "Syn0133"

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