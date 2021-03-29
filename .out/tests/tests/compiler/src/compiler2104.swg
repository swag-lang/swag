#[swag.printbc]
struct AA
{
    x, y: f64

    #[swag.printbc]
    struct BB
    {
        x, y: string = "true"
    }

    z: BB
}

#test
{
    var v: AA
    @assert(v.z.x == "true")
    @assert(v.z.y == "true")
    ptr := @typeof(AA.BB)
    @assert(@sizeof(v) == 6 * 8)
}