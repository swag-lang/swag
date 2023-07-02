//#global skip
struct Circle {}

interface IShape
{
    func area(self)->s32
}

impl IShape for Circle
{
    mtd impl area() => 123
}

func area_per_perimeter(shape: IShape) => shape.area()

#test
{
    a := Circle{}
    v := cast(IShape) a
    lambda := &area_per_perimeter

    @assert(v.lambda() == 123)
}