struct BaseRenderOgl
{
}

struct RenderOgl
{
    using base: BaseRenderOgl
}

impl BaseRenderOgl
{
    func toto(using self)
    {

    }
}

#test
{
    var render: RenderOgl
    BaseRenderOgl.toto(&render)
}