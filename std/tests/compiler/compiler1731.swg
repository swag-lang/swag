//#[swag.printbc]
#test
{
    struct A
    {
        name: u64 = 666
    }

    struct B
    {
        ggg:        u64
        using root: A
    }

    /*func F(ptr: *A)
    {
        @assert(ptr.name == "toto")
    }*/

    var v: B
    @assert(v.name == 666)
    //v.name = "titi"
    //@assert(v.name == "titi")
    //F(&v)
}