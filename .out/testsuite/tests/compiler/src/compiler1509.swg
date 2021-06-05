/*#[Swag.compiler, Swag.inline]
func set()
{
    @assert(@isbytecode())
}

//#[Swag.printbc]
#test
{
    x := 0
    loop 10
    {
        #run
        {
            @assert(@isbytecode())
            set()
        }

        x += 1
    }

    @assert(x == 10)
}*/