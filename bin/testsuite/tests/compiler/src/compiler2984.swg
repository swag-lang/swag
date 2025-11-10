struct AA {}

enum Col { AA, BB }

#test
{
    let v = Col.AA
    switch v
    {
    case AA:
        break
    }
}
