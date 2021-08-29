#global public

namespace Parse
{
    public interface IPokeValue
    {
        poke: func(self, string)->string throw;
    }
}