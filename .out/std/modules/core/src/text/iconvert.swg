#global public

namespace StrConv
{
    interface IConvert
    {
        convert: func(self, *ConcatBuffer, const *ConvertFormat, string)
    }
}
