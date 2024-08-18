#global skip

#[Swag.Safety("", false)]
#[Swag.Safety("sanity", true)]
func toto()
{
    var x: [4] s32
    loop i in 10
    {
        @assert(x[i] == 0)
    }
}