#[Swag.ConstExpr]
struct V
{
    x, y: f32 = 0
}

func toto(x, y: V)->V
{
    return @{x.x + y.x, x.y + y.y}
}

const C = #run toto(@{1, 2}, @{10, 20})
#assert C.x == 11
#assert C.y == 22