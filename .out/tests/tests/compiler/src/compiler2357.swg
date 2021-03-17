#[swag.mixin]
func mkVar(name: string, value: s32)
{
    #ast
    {
        return name ~ " := " ~ value
    }
}

mkVar("titi", 555)
mkVar("tata", 666)

#placeholder titi
#placeholder tata

#test
{
    @assert(titi == 555)
    @assert(tata == 666)
}