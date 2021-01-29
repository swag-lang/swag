#global testerror

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

    ptr := cast(*AA) itf
}