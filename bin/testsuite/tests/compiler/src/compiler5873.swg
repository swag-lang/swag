struct A {}

interface I
{
    mtd free5873(addr: *AllocatorRequest);
    mtd alloc(addr: *AllocatorRequest);
}

impl I for A
{
    mtd impl free5873(r: *AllocatorRequest) {}

    mtd impl alloc(r: *AllocatorRequest)
    {
        me.free5873(r)
    }
}

struct AllocatorRequest {}

impl A
{
    mtd free5873(addr: *void, size: u64) {}
}
