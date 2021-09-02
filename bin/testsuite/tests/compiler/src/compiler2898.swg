#global testerror

struct A
{

}

impl A
{
    func trim(using self)
    {
    }
}

#test
{
    var t: A
    var r: const&A = t
    r.trim()
}