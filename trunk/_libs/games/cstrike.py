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
    pPlayer = spe.getPlayer(userid)
    
    if pPlayer == None:
        return

    # Respawn the player
    spe.call("Respawn", pPlayer)

#================================================================================
# Switches a player's team without killing them.
#================================================================================    
def switchTeam( userid, team_index ):

    # Get player instance
    pPlayer = spe.getPlayer(userid)
    
    if pPlayer == None:
        return
    
    # Switch their team
    spe.call("ChangeTeam", pPlayer, int(team_index))

#================================================================================
# Returns an instance to a player's active weapon (the weapon they are currently
#   holding).
#================================================================================   
def getActiveWeapon( userid ):

    # Get player instance
    pPlayer = spe.getPlayer( userid )
    
    if pPlayer == None:
        return None
    
    # Call and return player's active weapon
    return spe.call("GetActiveWeapon", pPlayer)


