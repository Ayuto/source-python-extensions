#================================================================================
# Source-Python Extensions Library:
#   Counter-Strike Source Library
#================================================================================

#================================================================================
# Imports
#================================================================================
from os import name as platform
from spe import *
from spe.games.shared import *

#================================================================================
# Globals
#================================================================================
gpGameRules = None

#================================================================================
# Gets the gamerules all initialized
#================================================================================
def InitGameRules():
    global gpGameRules
    
    # If we're on windows, we need this signature
    if platform == 'nt':
        # Find the CreateGameRules function
        pFunc = findFunction("\x8B\x0D\x2A\x2A\x2A\x2A\x85\xC9\x74\x2A\x8B\x01\x6A\x01\xFF\x50", 16)
    
        # Rip out gamerules
        gpGameRules = ripPointer(pFunc, 2)
    else:
        # Find the symbol address
        gpGameRules = findSymbol("@g_pGameRules")

InitGameRules()

#================================================================================
# Respawns a player
#================================================================================
def Respawn( userid ):
    
    # Get player instance
    pPlayer = GetPlayer(userid)
    
    # Respawn the player
    Call("Respawn", pPlayer)

#================================================================================
# Switches a player's team without killing them.
#================================================================================    
def SwitchTeam( userid, team_index ):

    # Get player instance
    pPlayer = GetPlayer(userid)
    
    # Switch their team
    Call("ChangeTeam", pPlayer, int(team_index))

#================================================================================
# Returns an instance to a player's active weapon (the weapon they are currently
#   holding).
#================================================================================   
def GetActiveWeapon( userid ):

    # Get player instance
    pPlayer = GetPlayer( userid )
    
    # Call and return player's active weapon
    return Call("GetActiveWeapon", pPlayer)

#================================================================================
# Restarts the round with a specific win reason!
#================================================================================
def TerminateRound( time_delay, win_reason ):
    global gpGameRules

    if not isinstance(time_delay, float):   
        time_delay = float(time_delay)
    
    Call("TerminateRound", gpGameRules, time_delay, win_reason)


