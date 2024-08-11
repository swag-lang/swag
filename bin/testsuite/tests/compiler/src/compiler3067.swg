#global testerror "Err0026"
struct AA
{
    x: s32
}

var G: AA

#[Swag.ConstExpr]
func toto()->*AA
{
    return &G
}

const X = toto()