#global testerror
struct AA
{
    x: s32
}

#[Swag.ConstExpr]
func toto()->{AA, s32}
{
    return @{{2}, 4}
}

const X = toto()
#assert X.item0.x == 2