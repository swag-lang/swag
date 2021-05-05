//#global skip
#global testerror
namespace drawing
{
    struct Color
    {

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
        }
    }
}