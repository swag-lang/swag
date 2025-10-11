//#global testerror "Err0024"

struct Point { x, y: f32 }

impl Point
{
    func contains(me, x: f32) {}

    func toto(me)
    {
        contains(0'f32)
    }
}

func contains(x: f32)
{
    @assert(false)
}
