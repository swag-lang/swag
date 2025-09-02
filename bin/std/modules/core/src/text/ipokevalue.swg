#global public

namespace StrConv
{
    public interface IPokeValue
    {
        mtd poke(buf: string)->string throw;
    }
}
