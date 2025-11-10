struct A
{
    x:        s32 = 50
    name:     string
}

const countries: [?] A = [
    {name: "titi"}]

#test
{
    @assert(countries[0].x == 50)
    @assert(countries[0].name == "titi")
}
