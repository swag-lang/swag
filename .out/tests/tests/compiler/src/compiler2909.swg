#test
{
    @assert(@countof(@typeof(Drawing.Color).interfaces) == 1)
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

namespace parse
{
    interface IParse
    {
        xoxo: func(self)
    }

    impl IParse for Drawing.Color
    {
        func xoxo(using self)
        {
            #assert @sizeof(Color) == 8
        }
    }
}
