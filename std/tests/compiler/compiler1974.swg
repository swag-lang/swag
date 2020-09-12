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
    func grow()
    {
        @print(cast(s64) @sizeof(K))
    }
}

#test
{
    var arr1: HashTable'(u64, u32)
    //HashTable.grow()
}