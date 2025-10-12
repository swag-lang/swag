struct Game
{
    answer: s32
}

//#[Swag.PrintBc]
//#[Swag.Optimize(.All, false)]
func play(game: *Game)
{
    //@breakpoint()
    dref game = update(game)
}

//#[Swag.Optimize(.All, false)]
//#[Swag.PrintBc]
func update(game: Game)->Game
{
    @assert(game.answer == 666)
    let result: retval = game
    @assert(game.answer == 666)
    return result
}

#test
{
    var v = Game{}
    v.answer = 666
    //v = update(&v)
    play(&v)
}
