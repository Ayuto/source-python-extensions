# Why would I use SPE? #

Simply put, there are certain things normal server plugins cannot do with the given SDK. Examples would be respawning players, and blocking rounds from ending. SPE aims to provide a dynamic platform with which a clever scripter can do all of this (and more)!

# What games does SPE support? #

SPE (in theory) supports virtually all VALVe games, engines, and mods built on those engines, provided that the Server Plugin interface has not changed between any of them and the time of this writing (2011).

## Important Note ##
"Support" in this context means "loads and functions normally." It does not mean that SPE will have prebuilt signatures and libraries shipped for a particular game!

# What is an instance/pointer in SPE? #
In C++, a [pointer is a variable that stores a memory address](http://www.linuxconfig.org/c-understanding-pointers). Basically, when you retrieve an "instance" in SPE, this instance refers to the entity's memory address in C++. Therefore, this allows the scripter direct access to the C++ "object" for the entity. There are certain boundaries as to what you can do with the instances/pointers set forth by SPE as well as what can be accessed with the Source Engine's SDK, so having access to the pointer is not all-inclusive.