"""
$Rev: 59 $
$LastChangedDate: 2010-01-20 21:41:38 -0500 (Wed, 20 Jan 2010) $
"""

#================================================================================
# Source-Python Extensions Library:
#   Left 4 Dead Library
#================================================================================

#================================================================================
# Imports
#================================================================================

# Python imports
from os import name as platform

# Eventscripts imports
import es

# SPE imports
import spe

#================================================================================
# Exported Functions
#================================================================================
def createEntity( entity_name ):
    """
    Left4Dead's CreateEntityByName function has an extra parameter.
    I set this to true.
    
    This function returns a pointer to the entity you just created.
    It returns None if the entity could not be created.
    """

    return spe.call("CreateEntity", entity_name, -1, 1)