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

var G = 0'u32

impl HashTable
{
    func toto()
    {
        /*@print(@typeof(K).name)
        @print(@typeof(V).name)
        @print("\n")*/
        G += @sizeof(K)
    }

    func grow(self)
    {
        /*@print(@typeof(K).name)
        @print(@typeof(V).name)
        @print("\n")*/
        G += @sizeof(K)
        toto()
    }
}

#test
{
    var arr1: HashTable'(u64, u32)
    HashTable.grow(arr1)

    var arr2: HashTable'(bool, f32)
    HashTable.grow(arr2)

    @assert(cast(s64) G == 8 + 8 + 1 + 1)
}