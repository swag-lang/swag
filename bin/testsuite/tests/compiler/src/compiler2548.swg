struct AA
{
    x: *u8
}

impl AA
{
    func opDrop(using me)
    {
        @free(x)
        /*@print("x ")
        @print(cast(s64) cast(u64) x)
        @print(" drop ")
        @print(cast(s64) cast(u64) me)
        @print("\n")*/
    }

    func opPostCopy(using me)
    {
        x = @alloc(10)
        /*@print("x ")
        @print(cast(s64) cast(u64) x)
        @print(" copy ")
        @print(cast(s64) cast(u64) me)
        @print("\n")*/
    }

    /*func opPostMove(using me)
    {
        @print("move\n")
    }*/
}

//#[Swag.PrintBc]
func titi()->[1] AA
{
    let ptrptr: *u8 = @alloc(10)
    /*@print("ptr ")
    @print(cast(s64) cast(u64) ptrptr)
    @print("\n")*/
    return [{ptrptr}]
}

//#[Swag.PrintBc]
#test
{
    var v = titi()
}
