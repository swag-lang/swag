#global public

namespace StrConv
{
    interface IConvert
    {
        convert: func(self, *ConcatBuffer, const *ConvertFormat, string)
    }
}

impl StrConv.IConvert for String
{
    func convert(using self, buf: *ConcatBuffer, fmt: const *StrConv.ConvertFormat, strFormat: string)
    {
        buf.addBytes(@mkslice(buffer, length))
    }
}
