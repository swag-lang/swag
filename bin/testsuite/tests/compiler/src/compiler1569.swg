
#global testerror "Err0213"

struct A
{
    x: s32 = 1
}

func toto(val: A)
{
    var y: A = #move val
}
