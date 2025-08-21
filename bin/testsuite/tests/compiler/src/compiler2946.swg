struct BaseRenderOgl
{
}

struct RenderOgl
{
    using base: BaseRenderOgl
}

impl BaseRenderOgl
{
    func toto(using me)
    {

    }
}

#test
{
    var render: RenderOgl
    render.base.toto()
}