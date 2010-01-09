#================================================================================
# Source-Python Extensions Library:
#   Counter-Strike Source Library
#================================================================================

#================================================================================
# Imports
#================================================================================

# SPE imports
import spe

#================================================================================
# Exported Functions
#================================================================================

#================================================================================
# Respawns a player
#================================================================================
def respawn( userid ):
    # Get the player instance
    pPlayer = spe.getPlayer(int(userid))

    # Make sure the player instance is valid
    if not pPlayer:
        # Return False since the player instance was not valid
        return False

    # Respawn the player
    spe.call("Respawn", pPlayer)

    return True

#================================================================================
# Switches a player's team without killing them.
#================================================================================    
def switchTeam( userid, team_index ):
    # Get the player instance
    pPlayer = spe.getPlayer(int(userid))

    # Is the player instance valid?
    if not pPlayer:
        # Return False since the player instance was not valid
        return False

    # Switch their team
    spe.call("ChangeTeam", pPlayer, int(team_index))

    return True

#================================================================================
# Returns an instance to a player's active weapon (the weapon they are currently
#   holding).
#================================================================================   
def getActiveWeapon( userid ):
    # Get the player instance
    pPlayer = spe.getPlayer(int(userid))

    # Is the player instance valid?
    if not pPlayer:
        # Return None since the player instance was not valid
        return None

    # Call and return player's active weapon
    return spe.call("GetActiveWeapon", pPlayer)

#================================================================================
# Gives a player a named item.
#================================================================================
def giveNamedItem( userid, item_name ):
    # Get the player instance
    pPlayer = spe.getPlayer(int(userid))

    # Is the player instance valid?
    if not pPlayer:
        # Return False since the player instance was not valid
        return False

    # Give the player the item
    spe.call('GiveNamedItem', pPlayer, str(item_name))

    return True

#================================================================================
# If the player owns the weapon_instance entity, it forces them to drop it.
#================================================================================
def dropWeapon( userid, weapon_name ):
    # Get the player instance
    pPlayer = spe.getPlayer(int(userid))

    # Is the player instance valid?
    if not pPlayer:
        # Return False since the player instance was not valid
        return False

    # Get the weapon instance
    weapon_instance = spe.ownsWeapon( userid, weapon_name )

    # Is the weapon instance valid?
    if not weapon_instance:
        # Return False since the weapon instance was not valid
        return False

    # Make them drop it
    spe.call('DropWeapon', pPlayer, weapon_instance)

    return True