public func toto41()
{
    var m = @modules
    @assert(@countof(m) == 1)
    @assert(m[0].name == "test_foreign")
}
