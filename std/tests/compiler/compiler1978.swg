struct(K, V) HashTableEntry
{
    key:    K
    value:  V
}

struct(K, V) HashTable
{
    table:  *HashTableEntry'(K, V)
    #assert @typeof(K) == AA
    #assert @typeof(V) == u32
}

struct AA
{
}

#test
{
    var arr1: HashTable'(AA, u32)
    //arr1.toto()
}