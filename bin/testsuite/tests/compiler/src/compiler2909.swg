#test
{
    @assert(@countof(#typeof(Drawing.Color).interfaces) == 1)
}

//#global skip
//#global testerror
namespace Drawing
{
    struct Color
    {
        f: f64
    }
}

namespace Parse
{
    interface IParse
    {
        func xoxo(me);
    }

    impl IParse for Drawing.Color
    {
        func impl xoxo(using me)
        {
            #assert(#sizeof(Color) == 8)
        }
    }
}
