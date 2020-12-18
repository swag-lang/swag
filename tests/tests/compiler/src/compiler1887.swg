//#[swag.compiler]
func(T) checker()
{
    //@print(v)
    ptr := @typeof(T)
    @assert(ptr == s32)
}

//#[swag.printbc]
func(T) toto()
{
    //#print "toto"
    #run
    {
        checker'T()
    }
}

//#[swag.printbc]
func titi()
{
    toto's32()
}
