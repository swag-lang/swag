struct AA
{
    x: *u8
}

impl AA
{
    func opDrop(using self)
    {
        @free(x)
        /*@print("x ")
        @print(cast(s64) cast(uint) x)
        @print(" drop ")
        @print(cast(s64) cast(uint) self)
        @print("\n")*/
    }

    func opPostCopy(using self)
    {
        x = @alloc(10)
        /*@print("x ")
        @print(cast(s64) cast(uint) x)
        @print(" copy ")
        @print(cast(s64) cast(uint) self)
        @print("\n")*/
    }

    /*func opPostMove(using self)
    {
        @print("move\n")
    }*/
}

//#[Swag.printbc]
func titi()->[1] AA
{
    var ptrptr: *u8 = @alloc(10)
    /*@print("ptr ")
    @print(cast(s64) cast(uint) ptrptr)
    @print("\n")*/
    return @[{ptrptr}]
}

//#[Swag.printbc]
#test
{
    v := titi()
}
