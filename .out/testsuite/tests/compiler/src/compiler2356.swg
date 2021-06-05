#[Swag.mixin]
func mkVar(name: string, value: s32)
{
    #ast
    {
        return name ~ " := " ~ value
    }
}

//#placeholder titi
//#placeholder tata

#test
{
    mkVar("titi", 555)
    mkVar("tata", 666)

    @assert(titi == 555)
    @assert(tata == 666)
}