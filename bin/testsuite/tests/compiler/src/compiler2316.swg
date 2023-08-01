#[Swag.Strict]
typealias T = s32

func toto(x: T)
{
    @assert(cast(s64) x == 555)
}

#test
{
    toto(cast(T) 555)
}