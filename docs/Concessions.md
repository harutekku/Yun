# Concessions

Since this is a hobby/first-year academic project, I had to make some concessions
when writing it. Here they are:
- Parser/VM error reporting: it just sucks and I can't be bothered to make it better right now [as of 17.06 A.D 2022]
- Optimizations: who needs them either way?
- Type-checking and runtime cost: abandon all the hope, ye, who try to find information about graph algorithms.
  I would love to implement a quick and dirty DFG just to get rid of dumb runtime type-checking, but unfortunately
  this isn't possible right now.
- Array instructions: I could either extend instruction size to 8-bytes or stick with variable-length 
  instruction format. Both sucked, so I stuck with "array views", as I like to call them.
- Arrays: I could either add another 22 instructions just to handle basic arrays, or use good old
  `memcpy` to handle their values as I please. The second option was more promising, so I went with it.
- Doxygen: I HATE that thing. It's a crapy documentation tool that sells itself as a Holy Grail of Docs.
  Docs in Markdown and self documenting code (I guess you can grasp the basic concepts of an instruction
  emitter in 30 minutes or less) are the way.
- GCC extensions: C++ standard is braindead when it comes to `union`s. C got it right.
