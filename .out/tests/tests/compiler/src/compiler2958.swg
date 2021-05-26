#global testerror

#[Swag.constexpr]
struct V
{
    x, y, z: f32 = 666
}

impl V
{
    func opDrop(self){}
}

func toto()->V
{
    return @{1, 2, 3}
}

const C = #run toto()
#assert C.x == 1
#assert C.y == 2
#assert C.z == 3