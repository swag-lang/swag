struct Theme {}

impl Theme
{
    mtd getIcon24() => 666
}

struct Wnd
{
    theme: Theme
}

impl Wnd
{
    #[Swag.Inline]
    mtd getTheme() => &me.theme
}

#test
{
    var v: Wnd
    let x  = v.getTheme().getIcon24()
    @assert(x == 666)
}
