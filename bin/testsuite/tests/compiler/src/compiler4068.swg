#global testerror
#test
{
    var toto: [..] s32 = [10, 20]
    r := &toto
    x := 0
    loop r
    {
        x += 1
    }

    @print(x)
    @assert(x == 2)
}