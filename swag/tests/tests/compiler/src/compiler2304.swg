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
    }
}

#assert @typeof(BB).flags & swag.TypeInfoFlags.HasDrop

#test
{
}