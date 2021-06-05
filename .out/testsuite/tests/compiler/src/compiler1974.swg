
#global testerror

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
    func grow()
    {
    }
}

#test
{
    var arr1: HashTable'(u64, u32)
    HashTable.grow()
}