struct A
{
    #ast 
    {
        return "x: s32; y: s32 = 666"
    }

    z: s32 = 555
}

//#[swag.printbc]
#test
{
    var a: A
    a.x = 0
    @assert(a.y == 666)
    @assert(a.z == 555)
}