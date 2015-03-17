

# Introduction #
Version 1.6.0a+ of Source Python Extensions contains a new dynamic hooking system, capable of offering unprecedented power to python scripts. In a nutshell, this system allows you to 'hook' almost any function in the Source Engine. Then, when the engine calls the function you hooked, it will callback to your python function, allowing you to modify its arguments as well as blocking its execution.

# Initializing hooks #
In order to hook a function, you need to provide SPE with some basic information about the function. This is done via an INI:
```
[OnTakeDamage]
shortname = "OnTakeDamage"
sig = " 55 8B EC 81 EC 30 01 00 00 56 89 2A 2A 2A 2A 2A 8B 45 08"
symbol = ""
param = "pp)i"
convention = "thiscall"
```

Astute readers will note that this information looks exactly like the information in a regular signature file. That's because it the hooking system was designed to utilize the same information the function calling system uses.

To load this information into SPE, just do the following:
```
spe.parseINI('mymod/mysignatures.ini')
```

Where _mysignatures.ini_ contains the INI information listed above. Now we are ready to hook the function called `OnTakeDamage` (note, I'm referring to the shortname here).

# Hooking a function #
Now we are ready to hook a function. To register a hook, we use the function `spe.detourFunction`:

```
spe.detourFunction('OnTakeDamage', HookType.Pre, damage_callback_pre)
```

The first parameter tells SPE that we want to hook the function called `OnTakeDamage`. The second parameter tells SPE that we want our callback executed **before** the original function is executed. Currently, post callbacks are **not** implemented. The last parameter tells SPE which function to execute when `CCSPlayer::OnTakeDamage` is called.

**Note: You must unregister all hooks when your script is unloaded or else you risk crashing your server!**

# Unhooking a function #
To unhook a function, we must use `spe.undetourFunction`:
```
spe.undetourFunction('OnTakeDamage', HookType.Pre, damage_callback_pre)
```

The arguments given to this function must **exactly** match the arguments given to `spe.detourFunction`. This is necessary because SPE searches an internal list for your callback.

# Callbacks #
After registering a callback, the next time the Source Engine executes the function you hooked, SPE will execute the registered the callback.

