#test
{
    #[Swag.mixin]
    func myMixin(what: code)
    {
        #mixin what
    }

    a := 0
    myMixin() { a += 1; }
    myMixin() { a += 1; }
    myMixin() { a += 1; }
    myMixin() { a += 1; }
    @assert(a == 4)
}
