struct AA
{
    xxx: f64
}

struct BB
{
    x: AA
}

impl AA
{
    func opDrop(using self)
    {
        xxx = 4
    }
}

#assert @typeof(BB).flags & Swag.TypeInfoFlags.HasDrop

#test
{
}