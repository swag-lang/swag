attr T()-> "enumvalue"
enum TITI
{
    //#[T]
    //{
        A
        B   
    //}
}

struct TOTO
{
    x: s32
    #[swag.offset("x")]
    y: s32
}