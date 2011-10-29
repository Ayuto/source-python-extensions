# ../_libs/python/spe/tools/offsets.py

# =============================================================================
# >> IMPORTS
# =============================================================================
# Python Imports
from configobj import ConfigObj
from os import name as os_name
if os_name != 'nt':
    os_name = 'linux'
from path import path

# Eventscripts Imports
from es import getGameName


# =============================================================================
# >> GLOBAL VARIABLES
# =============================================================================
basepath = path(__file__).parent
gamename = str(getGameName()).lower()


# =============================================================================
# >> CLASSES
# =============================================================================
class DictionaryOfOffsets(dict):
    '''Stores all offsets by entity type in a dictionary'''

    def __getitem__(self, name):
        '''Gets all the offsets for the given entity name'''

        # Has the entity already been added to the dictionary?
        if name in self:

            # If so, return the entity's dictionary
            return super(DictionaryOfOffsets, self).__getitem__(name)

        # Add the entity to the dictionary, and get any offsets
        values = self[name] = get_entity_offsets(name)

        # Return the entity's dictionary
        return values

# Get the DictionaryOfOffsets() instance
OFFSETS = DictionaryOfOffsets()


class OffsetDictionary(dict):
    '''Stores the offset amount and value type for the offset'''

    def __init__(self, offset, value_type):
        '''Store the offset amount and value type'''

        self.offset = offset
        self.type = value_type

    def __getattr__(self, name):
        '''Redirect to __getitem__, since this is a dictionary'''

        return self.__getitem__(name)

    def __setattr__(self, name, value):
        '''Redirect to __setitem__, since this is a dictionary'''

        self.__setitem__(name, value)


# =============================================================================
# >> FUNCTIONS
# =============================================================================
def get_entity_offsets(entity):
    '''Returns a dictionary of offsets for the given entity'''

    # Create a dictionary to store the offsets
    game_offsets = {}

    # Get the path to the entity's offsets for the current game
    inifile = basepath.joinpath(entity, 'ini', 'offsets', gamename + '.ini')

    # Does the file exist?
    if not inifile.isfile():

        # Return the empty dictionary
        return game_offsets

    # Get the offsets
    ini = ConfigObj(inifile)

    # Loop through the offsets
    for key in ini:

        # Get the offset's shortname
        name = ini[key]['shortname']

        # Get the offset
        offset = ini[key][os_name]

        # Make sure the offset is an integer
        offset = int(offset)

        # Get the offset's value type
        value_type = ini[key]['type']

        # Add the offset to the dictionary
        game_offsets[name] = OffsetDictionary(offset, value_type)

    # Return the dictionary of offsets
    return game_offsets
