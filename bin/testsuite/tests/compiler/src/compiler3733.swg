#test
{
    let x = @pinfos
    @assert(@countof(x.modules) == 1)
    @assert(x.modules[0].name == "test_compiler")
}
