var G: u64

func entry(param: u64)
{
    G = param
}

func create(lambda: func(u64), userParam: u64)
{
    lambda(userParam)
}

#test
{
    create(&entry, 0x12345678AABBCCDD)
    @assert(G == 0x12345678AABBCCDD)
}