struct(K, V) HashTableEntry
{
    key:    K
    value:  V
}

struct(K, V) HashTable
{
    //table:  *HashTableEntry'(K, V)
}

var G = 0'u32

impl HashTable
{
    func grow(using self)
    {
        /*@print(@typeof(HashTableEntry.K).name)
        @print(@typeof(HashTableEntry.V).name)
        @print(@typeof(HashTableEntry).name)
        @print(cast(s64) @sizeof(HashTableEntry))
        @print("\n")*/

        G += @sizeof(HashTableEntry)
        G += @sizeof(HashTableEntry)
    }
}

#test
{
    var arr1: HashTable'(u32, u32)
    arr1.grow()

    var arr2: HashTable'(u64, u64)
    arr2.grow()

    @assert(G == 8 + 8 + 16 + 16)
}