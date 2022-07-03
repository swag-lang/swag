#test
{
    md := @modules()
    visit t: md[0].types
    {
        x := t.fullname
    }
}
