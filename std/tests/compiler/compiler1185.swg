#module "compiler1185"
#unittest error

struct TOTO
{
    x: s32
    #[swag.offset("x1")]
    y: s32
}