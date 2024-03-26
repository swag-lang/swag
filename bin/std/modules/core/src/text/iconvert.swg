#global public

namespace StrConv
{
    interface IConvert
    {
        mtd convert(buf: *ConcatBuffer, fmt: const *ConvertFormat, strFormat: string);
    }
}
