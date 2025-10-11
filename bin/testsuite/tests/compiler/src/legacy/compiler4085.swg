struct A
{
    x:        s32 = 50
    name:     string
}

const countries: [?] A = [
    {name: "titi", x: 42}]

#test
{
    @assert(countries[0].x == 42)
    @assert(countries[0].name == "titi")
}
