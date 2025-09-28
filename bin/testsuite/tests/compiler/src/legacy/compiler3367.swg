//#global testerror

//#[Swag.NoCopy]
struct AA { x: s32; }

func toto()->[1] AA
{
    var result: retval
    result = [{1}]
    return result
}

//#[Swag.PrintBc]
#test
{
    var v: [1] AA
    v = toto()
    @assert(v[0].x == 1)
}