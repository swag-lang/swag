struct A
{
    x:        s32
    name:     string
}

const countries: [?] A = [
    {name: "titi", x: 10}]

#test
{
    @assert(countries[0].x == 10)
    @assert(countries[0].name == "titi")
}
