typealias T: ()->void

//#[swag.printbc]
#test
{  
    var x: T
    if(!x)
    {
    }
    
    if(x)
    {
        @assert(false)
    }
}