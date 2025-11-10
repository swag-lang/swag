#test
{
    #[Swag.Mixin]
    func myMixin(what: #code void)
    {
        #inject(what)
    }

    var a = 0
    myMixin()
    {
        a += 1; }
    myMixin()
    {
        a += 1; }
    myMixin()
    {
        a += 1; }
    myMixin()
    {
        a += 1; }
    @assert(a == 4)
}
