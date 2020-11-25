struct AA
{
    x: s64 = 1
}

impl AA
{
    func opDrop(using self)
    {
        //@print(x)
        @assert(x == 1)
        x = 666
        //@print("drop")
    }
}


func toto()-> {x: AA}
{
    var result: retval
    return result
}

//#[swag.printbc]
#test
{
    (v) := toto()
    //v1 := v.item0
}