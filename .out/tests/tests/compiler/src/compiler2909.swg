#test
{
    @assert(@countof(@typeof(drawing.Color).interfaces) == 1)
}

//#global skip
//#global testerror
namespace drawing
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

    impl IParse for drawing.Color
    {
        func xoxo(using self)
        {
            #assert @sizeof(Color) == 8
        }
    }
}
