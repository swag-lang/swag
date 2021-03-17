using swag

struct TOTO
{
    x: func(*TOTO)
}

//#[swag.printbc]
#test
{   
    ptr := cast(const *TypeInfoStruct) @typeof(TOTO)
}