#global testerror "Err0600"
#test
{
    var toto: [..] s32 = [10, 20]
    var r = &toto
    var x = 0
    loop r
    {
        x += 1
    }

    @print(x)
    @assert(x == 2)
}