using test_foreign

func thisFunc0(v: string)
{
    @assert(v == "666")
}

func thisFunc1(v, w, x, y, z: s64)
{
    @assert(v == 333)
    @assert(w == 444)
    @assert(x == 555)
    @assert(y == 666)
    @assert(z == 777)
}

func thisFunc2(v: s8, w: s16, x: s32, y: string, z: s64)
{
    @assert(v == 33)
    @assert(w == 44)
    @assert(x == 55)
    @assert(y == "666")
    @assert(z == 777)
}

#test
{
    tcf_toto23("666", &thisFunc0)

    var ptr3 = &tcf_toto19
    ptr3(333, 444, 555, 666, 777, &thisFunc1)

    var ptr4 = &tcf_toto22
    ptr4(33, 44, 55, "666", 777, &thisFunc2)
}