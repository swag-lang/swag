#test
{
    let md = @modules()
    visit t in md[0].types
    {
        var x = t.fullname
    }
}
