//
//#global testerror

func(T) contains(x: bool)
{
    @assert(false)
}

struct Point
{
    x, y: f32
}

impl Point
{
    func contains(me, x: f32)
    {
    }

    func toto(using me)
    {
        contains(0'f32)
    }
}