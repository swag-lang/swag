struct BB
{
    b: AAPtr
}

struct AA
{
    b: BBPtr
}

typealias AAPtr = *AA
typealias BBPtr = *BB