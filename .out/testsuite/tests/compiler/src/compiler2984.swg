struct AA
{

}

enum Col { AA; BB; }

#test
{
    v := Col.AA
    switch(v)
    {
    case AA: break
    }
}