# Table of Contents #


# Introduction #

The SPE module is the main gateway to for scripters to use SPE. This module will automatically load the methods and globals from spe/games/shared.py as well as spe/games/`game-name`.py.

# Methods #

---


## parseINI ##
### Description ###
```python

def parseINI( path ):
'''
This function parses and loads signatures in from an INI file.
Path represents the path to the INI file, with the
base being mod-name/addons/eventscripts/. You must
put in the .ini extension manually into the path.
It is not done for you. After parsing an INI, you can call functions
defined within that ini via spe.call.

NOTE: If you change a signature and reload your script, parseINI
will not update the signature addresses for you. You need to
do reload(spe) at the top of your file for this to happen (which
forces python to recompile and load SPE over again. Do not do this in
production code.
'''
```

### Example ###
```python

def loadMyINI():
# This will load an ini located at addons/eventscripts/test/test.ini
spe.parseINI('test/test.ini')

# Call the function with the shortname 'MyFunction' which takes 2 arguments.
# spe.call can utilize an infinite amount of arguments.
spe.call('MyFunction', arg1, arg2)
```


---

## parseTypesINI ##
### Description ###
```python

def parseTypesINI( path ):
'''
This function parses custom type definitions from an ini
of your choosing. You can then pass the shortname of a
custom type in the ini to spe.makeObject and it will
return a class instance comprised of the symbolic names
of all variables in the declared type.

As with parseINI, the base path is addons/eventscripts/.
You must append the .ini extension to the filename.
'''
```

### Example ###
This is the INI that we'll be using in this example script:
```
[Vector]
shortname = "Vector"
types = "f,f,f"
names = "x,y,z"

[EHandle]
shortname = "EHANDLE"
types = "i"
names = "index"

[TakeDamageInfo]
shortname = "CTakeDamageInfo"
types = "Vector,Vector,Vector,EHANDLE,EHANDLE,EHANDLE,f,f,f,i,i,i,i"
names = "vecDamageForce,vecDamagePosition,vecReportedPosition,hInflictor,hAttacker,hWeapon,flDamage,flMaxDamage,flBaseDamage,bitsDamageType,iDamageCustom,iDamageStats,iAmmoType"
```

```
def load():
    # Hook CCSPlayer::OnTakeDamage.
    # C++ Prototype: CCSPlayer::OnTakeDamage( CTakeDamageInfo &input )
    spe.detourFunction('OnTakeDamage', HookType.Pre, takedamage_callback)

    # Load in the ini containing a type definition for CTakeDamageInfo.
    spe.parseTypesINI('test/testtypes.ini')

def unload():
    # You need to unregister any hooks you create or else you risk crashing
    # your server!
    spe.undetourFunction('OnTakeDamage', HookType.Pre, takedamage_callback)

def takedamage_callback(args):
    # args[0] - The CTakeDamageInfo pointer passed to this function.
    # args[1] - The CCSPlayer instance that was passed to this function. (self pointer).

    # Construct an SPEObject from the CTakeDamageInfo pointer passed to this function.
    takeDamageInfo = spe.makeObject('CTakeDamageInfo', args[0])

    # Print out the damage caused by the attack.
    es.msg(takeDamageInfo.flDamage)    
```