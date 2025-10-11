//#global skip
struct Duration
{
    timeInSeconds: f32
}

impl Duration
{
    #[Swag.Inline]
    func fromMs(valueMs: u32) => Duration{valueMs / 1000.0}
}

#test
{
    let a = Duration.fromMs(1000)
    @assert(a.timeInSeconds == 1.0)
}
