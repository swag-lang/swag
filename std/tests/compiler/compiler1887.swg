//#[swag.compiler]
func(T) checker()
{
    //@print(v)
    ptr := @typeof(T)
    @print(ptr.name)
    //@assert(ptr == s32)
}

//#[swag.printbc]
func(T) toto()
{
    //#print "toto"
    #run
    {
        a := 666's64
        @print(a)
        checker'T()
    }
}

//#[swag.printbc]
func titi()
{
    toto's32()
}

#run
{
    @print("tataTATA")
}

#run @print("XXXXX")