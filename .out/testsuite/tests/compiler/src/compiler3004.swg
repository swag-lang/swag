/*struct R
{
    x: s32
}

impl R
{
    #[Swag.inline]
    func Y(using self)->s32
    {
        return x + 1
    }
}

func toto(x: R)
{

}

#test
{
    var v: R
    toto(@{v.Y()})
}*/