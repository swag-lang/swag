#global public

namespace strconv
{
    interface IConvert
    {
        convert: func(self, *ConcatBuffer, const *ConvertFormat, string)
    }
}

impl strconv.IConvert for String
{
    func convert(using self, buf: *ConcatBuffer, fmt: const *strconv.ConvertFormat, strFormat: string)
    {
        buf.addBytes(@mkslice(buffer, length))
    }
}
