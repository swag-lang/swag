#[swag.attributeUsage(swag.AttributeUsage.Field)]
attr toto();

struct A
{
    #[toto]
    x: s32
}

const C: A{0}