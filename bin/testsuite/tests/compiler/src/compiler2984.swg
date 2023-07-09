struct AA
{

}

enum Col { AA; BB; }

#test
{
    var v = Col.AA
    switch(v)
    {
    case AA: break
    }
}