#[swag.macro]
func opVisit(x: u32, stmt: code)
{
    loop x
    {
        #macro
        {
            let @alias0 = `x
            #mixin `stmt
        }
    }
}

#test
{
    cpt := 0
    visit c: 16
    {
        cpt += 1
        //@print("toto\n")
    }

    @assert(cpt == 16)
    //@print(cast(s64) cpt)
    //@print(" ")
}