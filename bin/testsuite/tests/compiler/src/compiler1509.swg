/*#[Swag.Compiler, Swag.Inline]
func set()
{
    @assert(@isbytecode())
}

//#[Swag.PrintBc]
#test
{
    var x = 0
    for 10
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