struct A { r, g, b, d: s32 }

var G = 0

#[Swag.Strict]
alias AL = A

func toto(x: AL) {}

#test
{
    toto({1, 2})
}
