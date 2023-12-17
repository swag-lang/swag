using Core

#test
{
    var r: Parser.RegExp

    {
        assume r.compile(#"^[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\.[A-Z|a-z]{2,}$"#)
        @assert(r.match("user@example.com"))
        @assert(!r.match("not_a_valid_email"))
    }

    {
        assume r.compile(#"^(\d{4})-(\d{2})-(\d{2})$"#)
        @assert(r.match("2023-12-17"))
        @assert(!r.match("12/17/2023"))
    }

    {
        assume r.compile(#"^(http|https)://[a-zA-Z0-9./?=_-]+$"#)
        @assert(r.match("https://www.example.com"))
        @assert(!r.match("ftp://invalid.url"))
    }
}