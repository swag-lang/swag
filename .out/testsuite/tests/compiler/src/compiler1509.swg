/*#[Swag.Compiler, Swag.Inline]
func set()
{
    @assert(@isbytecode())
}

//#[Swag.PrintBc]
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