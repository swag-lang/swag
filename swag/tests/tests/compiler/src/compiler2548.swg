struct AA
{
    x: *u8
}

impl AA
{
    func opDrop(using self)
    {
        @print("x ")
        @print(cast(s64) cast(uint) x)
        //@free(x)
        @print(" drop ")
        @print(cast(s64) cast(uint) self)
        @print("\n")
    }

    func opPostCopy(using self)
    {
        //x = @alloc(10)
        @print("x ")
        @print(cast(s64) cast(uint) x)
        //@free(x)
        @print(" copy ")
        @print(cast(s64) cast(uint) self)
        @print("\n")
    }

    func opPostMove(using self)
    {
        @print("move\n")
    }
}

#[swag.printbc]
func titi()->[1] AA
{
    var ptrptr: *u8 = @alloc(10)
    @print("ptr ")
    @print(cast(s64) cast(uint) ptrptr)
    @print("\n")
    return @[{ptrptr}]
}

#[swag.printbc]
//#[swag.optim("", false)]
#test
{
    v := titi()
}
