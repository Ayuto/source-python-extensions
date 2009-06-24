#================================================================================
# Source-Python Extensions Library:
#   Counter-Strike Source Library
#================================================================================

#================================================================================
# Imports
#================================================================================

# Python imports
from os import name as platform

import es

# SPE imports
import spe
from spe.games.shared import *

#================================================================================
# Globals
#================================================================================
gpGameRules = None

#================================================================================
# GameRules initialization function.
#================================================================================
def initGameRules():
    '''
    Rips out a pointer to an instance of CCSGameRules from memory.
    '''
    global gpGameRules
    
    # If we're on windows, we need this signature
    if platform == 'nt':
    
        # Rip out gamerules
        gpGameRules = spe.getPointer("\x8B\x0D\x2A\x2A\x2A\x2A\x85\xC9\x74\x2A\x8B\x01\x6A\x01\xFF\x50", 2)
    
    else:
        # Find the symbol address
        gpGameRules = spe.findSymbol("g_pGameRules")

initGameRules()

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


