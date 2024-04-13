#global public

namespace StrConv
{
    interface IConvert
    {
        mtd convert(buf: *ConcatBuffer, convFmt: ConvertFormat, strFormat: string);
    }
}
