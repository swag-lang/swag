#global skip

#[Swag.Safety(.All, .All, false)]
#[Swag.Sanity(true)]
func toto()
{
    var x: [4] s32
    for i in 10
    {
        @assert(x[i] == 0)
    }
}