/*#[Swag.Compiler, Swag.Inline]
func set()
{
    @assert(@bytecode)
}

//#[Swag.PrintBc]
#test
{
    var x = 0
    for 10
    {
        #run
        {
            @assert(@bytecode)
            set()
        }

        x += 1
    }

    @assert(x == 10)
}*/
