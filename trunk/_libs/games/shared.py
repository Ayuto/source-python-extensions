#================================================================================
# Source-Python Extensions Library:
#   Global Library
#================================================================================

#================================================================================
# Imports
#================================================================================
import es
import spe

#================================================================================
# Creates an entity by name, and returns an instance to it.
#================================================================================
def createEntity( entity_name ):
    
    # Call the function
    return spe.call("CreateEntity", entity_name, -1) # Last parameter must be -1.
    
#================================================================================
# Returns an entity instance by its index.
#================================================================================
def getEntityOfIndex( entity_index ):
    
    # Make sure the index is an int
    entity_index = int(entity_index)

    # Call and return
    return spe.call("EntityByIndex", entity_index)

#================================================================================
# Courtesy of Einlanzers and XE_Manup:
#   Returns the index of an entity. -1 means no entity exists at this index.
#================================================================================
def getIndexOfEntity( entity_instance, classname=None ):
    if not entity_instance:
        return None
        
    if classname:
        entlist = es.createentitylist(classname)
    else:
        entlist = es.createentitylist()

    for index in entlist:
        if entity_instance == entityByIndex(index):
            return index
    return None
    
#================================================================================
# These are for backwards compatibility.
#================================================================================
def entityByIndex( entity_index ):
    return getEntityOfIndex( entity_index )
    
def instanceToIndex( entity_instance, classname=None ):
    return getIndexOfEntity( entity_instance, classname )


#================================================================================
# Returns the instance of a player's weapon of type weapon_name.
#   Returns None if player doesn't own that particular weapon.
#================================================================================
def ownsWeapon( userid, weapon_name ):

    # Get player instance
    pPlayer = spe.getPlayer(userid)
    
    if pPlayer == None:
        return None
    
    # Call function and return weapon instance
    return spe.call("OwnsWeapon", pPlayer, weapon_name, 0)

#================================================================================
# Returns a weapon instance from a player's slot.
#================================================================================
def getWeaponFromSlot( userid, weapon_slot ):

    # Get player instance
    pPlayer = spe.getPlayer(userid)
    
    if pPlayer == None:
        return None
    
    # Call function and return player weapon instance
    return spe.call("GetWeapon", pPlayer, int(weapon_slot))
    
#================================================================================
# Removes an entity by it's index.
#================================================================================   
def removeEntityByIndex( entity_index ):
    
    # Get entity instance
    pEntity = entityByIndex( int(entity_index) )
    
    # Make sure it's valid
    if pEntity != None:
        
        # Remove it!
        spe.call("Remove", pEntity)
        
        return True
    
    # Return false if the entity was None.
    return False

#================================================================================
# Removes an entity by its instance
#================================================================================   
def removeEntityByInstance( entity_instance ):

    # Make sure it's valid
    if entity_instance != None:
    
        # Remove it!
        spe.call("Remove", entity_instance)
        
        return True
    
    return False
    
#================================================================================
# Sets an entity's string keyvalue.
#================================================================================  
def setStringKeyvalue( entity_index, keyvalue_name, new_value ):

    # Get entity instance
    pEntity = entityByIndex( int(entity_index) )
    
    # Make sure the entity is valid
    if pEntity != None:
    
        # Set the keyvalue
        spe.call("setkv_string", pEntity, keyvalue_name, new_value)
        
        return True
    
    return False