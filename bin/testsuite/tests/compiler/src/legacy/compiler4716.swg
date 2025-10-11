struct A
{
    tbl: *AI
}

struct AI
{
    call: func(me1: *A, x: s32 = 0)
}

func myCall(me1: *A, x: s32) {}

#test
{
    var a:  A
    var ai: AI

    a.tbl      = &ai
    a.tbl.call = &myCall
    a.tbl.call(&a)
}
