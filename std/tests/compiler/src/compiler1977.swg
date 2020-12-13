//
//#unittest error

struct(K, V) HashTableEntry
{
    key:    K
    value:  V
}

struct(K, V) HashTable
{
    table:  *HashTableEntry
}

impl HashTable
{
    func grow(self)
    {
        /*@print(@typeof(K).name)
        @print(@typeof(V).name)
        @print("\n")*/
        toto()
    }
}

namespace N
{
    alias A = s32
}

#test
{
    var arr1: HashTable'(N.A, u32)
}