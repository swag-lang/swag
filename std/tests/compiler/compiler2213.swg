struct A
{
    x = 0's64
}

impl A
{
    func opDrop(using self)
    {
    }

    func opPostMove(using self)
    {
    }
}

func toto()->A
{
    var res: retval
    res.x = 1
    return res
}

//#[swag.printbc]
#[swag.inline]
func titi()->A
{
    return toto()
}

#[swag.printbc]
#test
{
    x := titi()
    @print(x.x)
}