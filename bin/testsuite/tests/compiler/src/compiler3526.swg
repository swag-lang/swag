//#global skip
struct Circle {}

interface IShape
{
    func area(me)->s32;
}

impl IShape for Circle
{
    mtd impl area() => 123
}

func area_per_perimeter(shape: IShape) => shape.area()

#test
{
    let a      = Circle{}
    let v      = cast(IShape) a
    let lambda = &area_per_perimeter

    @assert(v.lambda() == 123)
}
