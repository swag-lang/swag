struct AA
{
    x: *u8
}

impl AA
{
    func opDrop(using self)
    {
        @free(x)
    }

    func opPostCopy(using self)
    {
        x = @alloc(10)
    }
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
