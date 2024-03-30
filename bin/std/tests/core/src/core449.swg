using Core

#test
{
    let str = "Testing 1, 2, 3 Testing 1, 2, 3 Testing 1, 2, 3 Testing 1, 2, 3"
    let v   = Hash.City32.compute(str)
    @assert(v == 0x86da05bf)
    //Console.printf("%{X}", v)
}
