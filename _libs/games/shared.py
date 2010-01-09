#================================================================================
# Source-Python Extensions Library:
#   Global Library
#================================================================================

#================================================================================
# Imports
#================================================================================
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
    # Call and return
    return spe.call("EntityByIndex", int(entity_index))

#================================================================================
# Courtesy of Einlanzers and XE_Manup:
#   Returns the index of an entity. None means no entity exists at this index.
#================================================================================
def getIndexOfEntity( entity_instance ):
    return spe.getEntityIndex( entity_instance )

#================================================================================
# Returns the instance of a player's weapon of type weapon_name.
#   Returns None if player doesn't own that particular weapon.
#================================================================================
def ownsWeapon( userid, weapon_name ):
    # Get player instance
    pPlayer = spe.getPlayer(int(userid))

    if not pPlayer:
        return None

    # Call function and return weapon instance
    return spe.call("OwnsWeapon", pPlayer, weapon_name, 0)

#================================================================================
# Returns a weapon instance from a player's slot.
#================================================================================
def getWeaponFromSlot( userid, weapon_slot ):
    # Get player instance
    pPlayer = spe.getPlayer(int(userid))

    if not pPlayer:
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
    if not pEntity:
        # Return false if the entity was None.
        return False

    # Remove it!
    spe.call("Remove", pEntity)

    return True

#================================================================================
# Removes an entity by its instance
#================================================================================   
def removeEntityByInstance( entity_instance ):
    # Make sure it's valid
    if not entity_instance:
        # Return false if the entity was None.
        return False
        
    # Remove it!
    spe.call("Remove", entity_instance)

    return True

#================================================================================
# Sets an entity's string keyvalue.
#================================================================================  
def setStringKeyvalue( entity_index, keyvalue_name, new_value ):
    # Get entity instance
    pEntity = entityByIndex( int(entity_index) )

    # Make sure the entity is valid
    if not pEntity:
        # Return False if the entity was None.
        return False

    # Set the keyvalue
    spe.call("setkv_string", pEntity, keyvalue_name, new_value)

    return True

#================================================================================
# Retrieve the index of a player's weapon
#================================================================================ 
def getWeaponIndex(userid, weapon_name):
    # Retrieve the weapon pointer
    weapon = spe.ownsWeapon(userid, weapon_name)

    # Make sure the weapon pointer is valid
    if not weapon:
        # Return None if the weapon pointer is not valid
        return None

    # Return the index of the weapon
    return spe.getEntityIndex(weapon)


#================================================================================
# Retrieve a list of weapon names that the player has in their inventory
#================================================================================ 
def getWeaponNameList( userid ):
    # Make sure the player is valid
    if not spe.getPlayer(int(userid)):
        # Return None due to the invalid player instance
        return None

    # Set up the list
    weapon_names = []

    # Loop through the maximum range of 48 weapons
    for i in range(0, 48):

        # Retrieve the weapon instance/pointer
        wPointer = spe.getWeaponFromSlot(userid, i)

        # Make sure the weapon instance/pointer is valid
        if wPointer:

            # Append the weapon name to the list
            weapon_names.append(spe.getEntityClassName(wPointer))

    # Return the populated list
    return weapon_names

#==============================================================================
# Retrieve a list of weapon instances that the player has in their inventory
#==============================================================================
def getWeaponInstanceList( userid ):
    # Make sure the player is valid
    if not spe.getPlayer(int(userid)):
        # Return None due to the invalid player instance
        return None

    # Set up the list
    weapon_pointers = []

    # Loop through the maximum range of 48 weapons
    for i in range(0, 48):

        # Retrieve the weapon instance/pointer
        wPointer = spe.getWeaponFromSlot(userid, i)

        # Make sure the weapon instance/pointer is valid
        if wPointer:

            # Append the valid instance/pointer to the list
            weapon_pointers.append(wPointer)

    # Return the populated list
    return weapon_pointers

#==============================================================================
# Retrieve a list of weapon indexes that the player has in their inventory
#==============================================================================
def getWeaponIndexList( userid ):
    # Make sure the player is valid
    if not spe.getPlayer(int(userid)):
        # Return None due to the invalid player instance
        return None

    # Return a list of weapon indexes
    return [spe.getEntityIndex(i) for i in spe.getWeaponInstanceList(userid)]

#==============================================================================
# Retrieve a dictionary of weapons in the player's inventory where the key is
#   the weapon_name which contains an additional dictionary with the keys of
#   "instance" (stores the pointer foe the weapon) and "slot" (stores the slot
#   that the weapon can be found in while using getWeaponFromSlot(), or as an
#   index to the lists: getWeaponNameList(), getWeaponInstanceList(), or
#   getWeaponIndexList().
#==============================================================================
def getWeaponDict( userid ):
    # Make sure the player is valid
    if not spe.getPlayer(int(userid)):
        # Return None due to the invalid player instance
        return None

    # Set up the list
    weapons = {}

    # Loop through the maximum range of 48 weapons
    for i in range(0, 48):

        # Retrieve the weapon instance/pointer
        wPointer = spe.getWeaponFromSlot(userid, i)

        # Make sure the weapon instance/pointer is valid
        if wPointer:

            # Create the valid key:value pair
            weapons[spe.getEntityClassName(wPointer)] = {
                "instance":wPointer,
                "slot":i,
                "index":spe.getEntityIndex(wPointer)}

    # Return the populated dictionary
    return weapons