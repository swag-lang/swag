#import "std"
using std

#test
{
    var map: HashTable'(s32, bool)
    //@assert(!map.contains(666))
    //map.add(666, true)
    //@assert(map.contains(666))
}