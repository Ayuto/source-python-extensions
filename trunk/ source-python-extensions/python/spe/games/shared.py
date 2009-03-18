#================================================================================
# Source-Python Extensions Library:
#   Global Library
#================================================================================

#================================================================================
# Imports
#================================================================================
import spe

#================================================================================
# Returns a player instance
#================================================================================
def GetPlayer( userid ):
    return spe.getPlayer( int(userid) )

#================================================================================
# Creates an entity by name, and returns an instance to it.
#================================================================================
def CreateEntity( entity_name ):
    
    # Call the function
    return spe.Call("CreateEntity", entity_name, -1) # Last parameter must be -1.

#================================================================================
# Returns an entity instance by its index.
#================================================================================
def EntityByIndex( entity_index ):
    
    # Make sure the index is an int
    entity_index = int(entity_index)

    # Call and return
    return spe.Call("EntityByIndex", entity_index)

#================================================================================
# Returns the instance of a player's weapon of type weapon_name.
#   Returns None if player doesn't own that particular weapon.
#================================================================================
def OwnsWeapon( userid, weapon_name ):

    # Get player instance
    pPlayer = GetPlayer(userid)
    
    ''' TODO: Make sure the weapon name is valid. '''
    
    # Call function and return weapon instance
    return spe.Call("OwnsWeapon", pPlayer, weapon_name)

#================================================================================
# Returns a weapon instance from a player's slot.
#================================================================================
def GetWeaponFromSlot( userid, weapon_slot ):

    # Get player instance
    pPlayer = GetPlayer(userid)
    
    # Call function and return player weapon instance
    return spe.Call("GetWeapon", pPlayer, int(weapon_slot))
    
#================================================================================
# Removes an entity by it's index.
#================================================================================   
def RemoveEntityByIndex( entity_index ):
    
    # Get entity instance
    pEntity = EntityByIndex( int(entity_index) )
    
    # Make sure it's valid
    if pEntity != None:
        
        # Remove it!
        spe.Call("Remove", pEntity)
        
        return True
    
    # Return false if the entity was None.
    return False

#================================================================================
# Removes an entity by its instance
#================================================================================   
def RemoveEntityByInstance( entity_instance ):

    # Make sure it's valid
    if entity_instance != None:
    
        # Remove it!
        spe.Call("Remove", entity_instance)
        
        return True
    
    return False
    
#================================================================================
# Sets an entity's string keyvalue.
#================================================================================  
def SetStringKeyvalue( entity_index, keyvalue_name, new_value ):

    # Get entity instance
    pEntity = EntityByIndex( int(entity_index) )
    
    # Make sure the entity is valid
    if pEntity != None:
    
        # Set the keyvalue
        spe.Call("setkv_string", pEntity, keyvalue_name, new_value)
        
        return True
    
    return False




    
    
    
    