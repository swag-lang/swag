struct A
{
    x: s64 = 1
}

impl A
{
    func opDrop(self)
    {
        @print("drop\n")
        //@assert(self.x == 1)
        self.x = 666
    }

    func opPostMove(self)
    {
        @print("postmove\n")
    }

    func opPostCopy(self)
    {
        @print("postcpy\n")
    }
}

#[swag.printbc, swag.nooptim]
func toto() -> {A, bool}
{
    var x: A
    x.x = 555
    return @{x, false}
}

#test
{
    v := toto()
    @print(v.item0.x)
}