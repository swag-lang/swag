#test
{
    #[Swag.Mixin]
    func mkVar(name: string, value: s32)
    {
        #ast "var " ++ name ++ " = " ++ value
    }

    mkVar("titi", 555)
    mkVar("tata", 666)

    @assert(titi == 555)
    @assert(tata == 666)
}