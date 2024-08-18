using Core

#test
{
    let x = @modules()
    @assert(@countof(x) >= 4)
    var cpt = 0
    visit m in x
    {
        switch m.name
        {
        case "test_core":
            cpt |= 1
        case "win32":
            cpt |= 2
        case "xinput":
            cpt |= 4
        case "core":
            cpt |= 8
        }
    }

    @assert(cpt == 8 + 4 + 2 + 1)
}
