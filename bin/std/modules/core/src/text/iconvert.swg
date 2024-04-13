#global public

namespace StrConv
{
    interface IConvert
    {
        mtd convert(buf: *ConcatBuffer, strFormat: string);
    }
}
