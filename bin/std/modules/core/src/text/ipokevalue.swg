#global public

namespace StrConv
{
    public interface IPokeValue
    {
        poke: func(self, string)->string throw;
    }
}