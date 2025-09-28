struct BB
{
    b: AAPtr
}

struct AA
{
    b: BBPtr
}

alias AAPtr = *AA
alias BBPtr = *BB