//
//#unittest error

struct TT
{
    x, y, z:  s64 = 456
}

//#[swag.printbc]
func C(x: ...)
{
    ptr1 := cast(const & TT) x[0]
    ptr2 := cast(TT) x[0]
    @assert(ptr1.x == 456)
    @assert(ptr2.y == 456)
    ptr2.x = 100
    @assert(ptr1.x == 456)

    /*@print(@kindof(x[0]).name); @print("\n")
    @print(@typeof(ptr1).name); @print("\n")
    @print(@typeof(ptr2).name); @print("\n")*/
    //ptr1.value = 0
}

#run
{
    var x: TT
    C(x)
}