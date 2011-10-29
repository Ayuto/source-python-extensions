# ../_libs/python/spe/tools/player/__init__.py

# =============================================================================
# >> IMPORTS
# =============================================================================
# SPE Imports
from spe import getIndexOfEntity
from spe import getPlayer

# SPE Tools Imports
from spe.tools import SPEBaseEntity


# =============================================================================
# >> CLASSES
# =============================================================================
class SPEPlayer(SPEBaseEntity):
    '''Create a class for player entities'''

    def __new__(cls, userid):
        '''Create a new SPEPlayer instance for the given userid'''

        # Create the new object
        self = object.__new__(cls)

        # Use super() to set our base attribute without calling our __setattr__
        super(SPEBaseEntity, self).__setattr__('userid', int(userid))

        # Return the object
        return self

    def raise_error(self):
        '''Raises an error when the userid's pointer is not found on the server
        '''

        # Raise an error, since the userid is no longer on the server
        raise IndexError(
            'Userid "%s" does not exist on the server' % self.userid)

    @property
    def pointer(self):
        '''Returns the pointer for the instance's userid'''

        # Get the pointer of our player
        return getPlayer(self.userid)

    @property
    def index(self):
        '''Returns the index for the instance's pointer'''

        # Get the player's index
        return getIndexOfEntity(self.pointer)
