func toto() {}

#test
{
    var t: [2] func()
    t[0] = &toto
    let x = t[0]
    x()
}
