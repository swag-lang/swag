#[Swag.Mixin]
func TITI(stmt: code)
{    
    #run
    {
        @assert(@strlen(@stringof(stmt)) > 0)
    }
}

TITI()
{
    struct A
    {   
        a: b1
        b: b1
        c: b1
    }
}
