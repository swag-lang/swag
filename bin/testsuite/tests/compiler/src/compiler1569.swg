
#global testerror "Err0238"

struct A
{
    x: s32 = 1
}

func toto(val: A)
{
    var y: A = #move val
}
