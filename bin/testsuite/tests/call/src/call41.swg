using test_foreign

//#[Swag.PrintBc, Swag.Optim(false)]
#test
{
    toto41()

    m := @modules()
    @assert(@countof(m) == 2)
}