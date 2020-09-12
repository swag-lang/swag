//#module "compiler1974"
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
    func toto()
    {
        @print(@typeof(K).name)
        @print(@typeof(V).name)
        @print("\n")
    }

    func grow(self)
    {
        @print(@typeof(K).name)
        @print(@typeof(V).name)
        @print("\n")
        toto()
    }
}

#test
{
    var arr1: HashTable'(u64, u32)
    HashTable.grow(arr1)

    var arr2: HashTable'(bool, f32)
    HashTable.grow(arr2)
}