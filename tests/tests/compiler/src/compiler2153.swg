
#unittest error

func contains(x: f32)
{
    @assert(false)
}

struct Point
{
    x, y: f32
}

impl Point
{
    func contains(self, x: f32)
    {
    }

    func toto(using self)
    {
        contains(0'f32)
    }
}
