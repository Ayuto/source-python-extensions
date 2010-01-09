#================================================================================
# Source-Python Extensions Library:
#   Counter-Strike Source Library
#================================================================================

#================================================================================
# Imports
#================================================================================

# Python imports
from os import name as platform

# Eventscripts imports.
import es

# SPE imports
import spe

#================================================================================
# Exported Functions
#================================================================================

#================================================================================
# Respawns a player
#================================================================================
def respawn( userid ):

    # Get player instance
    pPlayer = spe.getPlayer(int(userid))

    if pPlayer:
        # Respawn the player
        spe.call("Respawn", pPlayer)

#================================================================================
# Switches a player's team without killing them.
#================================================================================    
def switchTeam( userid, team_index ):

    # Get player instance
    pPlayer = spe.getPlayer(int(userid))

    if pPlayer:

        # Switch their team
        spe.call("ChangeTeam", pPlayer, int(team_index))

#================================================================================
# Returns an instance to a player's active weapon (the weapon they are currently
#   holding).
#================================================================================   
def getActiveWeapon( userid ):

    # Get player instance
    pPlayer = spe.getPlayer(int(userid))

    if pPlayer:

        # Call and return player's active weapon
        return spe.call("GetActiveWeapon", pPlayer)

#================================================================================
# Gives a player a named item.
#================================================================================
def giveNamedItem( userid, item_name ):

    pPlayer = spe.getPlayer(int(userid))

    if pPlayer:

        # Give the player the item
        spe.call('GiveNamedItem', pPlayer, str(item_name))

#================================================================================
# If the player owns the weapon_instance entity, it forces them to drop it.
#================================================================================
def dropWeapon( userid, weapon_name ):

    pPlayer = spe.getPlayer(int(userid))

    if not pPlayer:
        return

    weapon_instance = spe.ownsWeapon( userid, weapon_name )

    if weapon_instance:

        # Make them drop it
        spe.call('DropWeapon', pPlayer, weapon_instance)