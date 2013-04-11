# ../_libs/python/spe/tools/functions.py

# =============================================================================
# >> IMPORTS
# =============================================================================
# Python Imports
from binascii import unhexlify
from configobj import ConfigObj
from os import name as os_name
from path import path

# Eventscripts Imports
from es import getGameName
from es import ServerVar

# SPE Imports
from spe import call
from spe import getEntityOfIndex
from spe import getIndexOfEntity
from spe import gSPE
from spe import Signature


# =============================================================================
# >> GLOBAL VARIABLES
# =============================================================================
RETURN_TYPES = {
    'p': lambda pointer: getEntityOfIndex(getIndexOfEntity(pointer)),
    'i': int,
    'f': float,
    'B': bool,
    'd': float,
    'L': long,
    'S': str,
}

basepath = path(__file__).parent
gamename = str(getGameName()).lower()
engine = str(ServerVar('spe_engine'))


# =============================================================================
# >> CLASSES
# =============================================================================
class DictionaryOfFunctions(dict):
    '''Stores all functions by entity type in a dictionary'''

    def __getitem__(self, name):
        '''Gets all functions for the given entity name'''

        # Has the entity already been added to the dictionary?
        if name in self:

            # If so, return the entity's dictionary
            return super(DictionaryOfFunctions, self).__getitem__(name)

        # Add the entity to the dictionary, and get any functions
        values = self[name] = get_entity_functions(name)

        # Return the entity's dictionary
        return values

# Get the DictionaryOfFunctions() instance
FUNCTIONS = DictionaryOfFunctions()


class FunctionHandler(object):
    '''Handler class that stores and calls functions in memory'''

    # The pointer should always be None unless it is being used for a function
    current_pointer = None

    def __init__(self, name, spe_name, params):
        '''Store the necessary data for the current function'''

        # name is the "shortname" that is used to know the correct function
        self.name = name

        self.spe_name = spe_name

        # Store the parameters for when listing functions
        self.parameters = params

        # True if a pointer is needed as the first argument
        self.first = params[0] == 'p'

        # List of parameters by type needed for input
        # Removes pointer as the first argument if pointer should be the first
        self.params = [x for x in params.split('p', 1)[1].split(')')[0]]

        # True if the function needs to return the value(s)
        self.return_value = params.split(')')[1] != 'v'

        # Number of arguments required for the function
        self.args = len(self.params)

    def pointer_call_function(self, *values):
        '''Called when SPEBaseEntity's .get() method is called.
            self.current_pointer should have been set inside the .get() method.
        '''

        # Call the main method with the current_pointer as the first argument
        return self.call_function(self.current_pointer, *values)

    def call_function(self, pointer=None, *values):
        '''Calls the function with the given parameters'''

        # Is the value of values a single tuple/list argument?
        if len(values) == 1 and type(values[0]).__name__ in ('list', 'tuple'):

            # Correct the value of values.
            # This is for when using:
            # <class instance>.<function> = <tuple>
            # Example:
            # SPEPlayer(<userid>).give = ('weapon_awp', 0)
            values = tuple(values[0])

        # Are there enough arguments?
        if len(values) < self.args:

            # Raise an error for not enough arguments
            raise IndexError('Not enough parameters for ' + self.name +
                '.\nNeed %s and received %s' % (self.args, len(values)))

        # Are there too many arguments?
        if len(values) > self.args:

            # Raise an error for too many arguments
            raise IndexError('Too many parameters for ' + self.name +
                '.\nNeed %s and received %s' % (self.args, len(values)))

        # Create a list to store the parameters
        call_values = []

        # Is there a pointer?
        if not pointer is None:

            # If so, add it as the first argument
            call_values.append(pointer)

        # Was the first argument supposed to be a pointer, but was not?
        elif self.first:

            # Raise an error
            raise ValueError('Pointer expected as first argument')

        # Loop through the arguments given to us,
        # and verify that they are of the correct type
        for value in xrange(self.args):

            # Try to typecast the value to make sure it is the proper type
            try:

                # Try to convert the value if needed
                new_value = RETURN_TYPES[self.params[value]](values[value])

                # Is the value supposed to be a pointer?
                if self.params[value] == 'p':

                    # Is the value a valid pointer?
                    if new_value is None:

                        # If it is not, raise an error
                        raise ValueError

            # If the value isn't the proper type
            except:

                # Raise a proper error
                raise ValueError('Invalid parameter "' + str(values[value]) +
                    '" for type "' + self.params[value] + '"')

            # If all went well, add the value to the list of arguments
            call_values.append(new_value)

        # Reset the pointer for the next use
        self.current_pointer = None

        # Does the function need to return any value(s)?
        if self.return_value:

            # Return the value(s) from the function
            return call(self.spe_name, *call_values)

        # If no return values were needed, still call the function
        call(self.spe_name, *call_values)


# =============================================================================
# >> FUNCTIONS
# =============================================================================
def get_entity_functions(entity):
    '''Returns a dictionary of functions for the given entity'''

    # Create a dictionary to store the functions
    return_functions = {}

    # Get the path to the current game's functions
    gamefile = basepath.joinpath(entity, 'ini', 'games', gamename + '.ini')

    # Get the path to the current engine's functions
    enginefile = basepath.joinpath(entity, 'ini', 'engines', engine + '.ini')

    # Check both the game's functions and the engine's functions files
    for filepath in (gamefile, enginefile):

        # Does the file exist?
        if not filepath.isfile():
            continue

        # Get the functions for the current file
        ini = ConfigObj(filepath)

        # Loop through all the functions in the file
        for key in ini:

            # Get the functions "shortname"
            name = ini[key]['shortname']

            # Set the SPE shortname
            spe_name = entity + '_' + name

            # Get the parameters
            params = ini[key]['param']

            # Get the calling convention
            convention = ini[key]['convention']

            # Is the OS Windows?
            if os_name == 'nt':

                # Get the signature for the function
                sig = ini[key]['sig']

                # Does the signature contain spaces?
                if ' ' in sig:

                    # Convert it to a proper readable format
                    sig = unhexlify(sig.replace(' ', ''))

                # Add the signature to SPE
                gSPE.Signatures[spe_name] = Signature(sig, params, convention)

            # Is the OS not Windows?
            else:

                # Add the symbol to SPE
                gSPE.Signatures[spe_name] = Signature(
                    ini[key]['symbol'], params, convention)

            # Does the "shortname" already exist for the entity?
            # This basically makes sure the engine's ini
            # file doesn't overwrite the game's ini file.
            if not name in return_functions:

                # Add the "shortname" to the dictionary with it's parameters
                return_functions[name] = FunctionHandler(
                    name, spe_name, params)

    # Return the dictionary of functions
    return return_functions
