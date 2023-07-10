#test
{
    let md = @modules()
    visit t: md[0].types
    {
        var x = t.fullname
    }
}
