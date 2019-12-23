#[swag.macro]
func opVisit(x: string, stmt: code)
{
    let data = @dataof(x)
    loop x
    {
        #macro
        {
            let @alias0 = `data[@index]
            #mixin `stmt
        }
    }
}

#test
{
    cpt := 0
    visit c: "toto"
    {
        cpt += cast(s32) c
    }

    @assert(cpt == 454)
    //@print(cast(s64) cpt)
    //@print(" ")
}