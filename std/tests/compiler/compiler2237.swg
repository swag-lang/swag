struct Pt
{
    x: s64 = 1
}

func toto(x: Pt)
{
    //@assert(x.x == 555)
}

#[swag.printbc]
func offset()->s64 => 555

#[swag.printbc, swag.optim(false)]
#test
{
    toto(Pt{offset()})
}