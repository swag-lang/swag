using test_foreign

//#[Swag.PrintBc, Swag.Optim(false)]
#test
{
    toto41()

    var m = @modules()
    @assert(@countof(m) == 2)
    @assert(@countof(m[0].types) > 0)
    @assert(@countof(m[1].types) > 0)

    /*visit x: m
    {
        @print("XXXXX ", x.name, "XXXXXX\n")
        @print(@countof(x.types), "\n")
        visit t: x.types
        {
            @print(t.name, "\n")
        }
    }*/
}