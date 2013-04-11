# ../_libs/python/spe/tools/__init__.py

# =============================================================================
# >> IMPORTS
# =============================================================================
# EventScripts Imports
from es import gethandlefromindex
from es import getindexfromhandle

# SPE Imports
from spe import getEntityClassName
from spe import getEntityOfIndex
from spe import getIndexOfEntity
from spe import getLocVal
from spe import gSPE
from spe import makeObject
from spe import setLocVal

# SPE Tools Imports
#   Functions
from spe.tools.functions import FUNCTIONS
from spe.tools.functions import RETURN_TYPES
#   Offsets
from spe.tools.offsets import OFFSETS


# =============================================================================
# >> CLASSES
# =============================================================================
class SPEBaseEntity(object):
    '''Base class for getting/setting offsets and running functions'''

    def __new__(cls, index):
        '''Creates a new SPEBaseEntity instance for the entity index'''

        # Create the new object
        self = object.__new__(cls)

        # Use super() to set the base attribute without calling __setattr__
        super(SPEBaseEntity, self).__setattr__('index', int(index))

        # Return the object
        return self

    def __getitem__(self, item):
        '''Redirects getting an item to the get() method'''

        # Return using the get() method
        return self.get(item)

    def __getattr__(self, item):
        '''Redirects getting an attribute to the get() method'''

        # Return using the get() method
        return self.get(item)

    def get(self, item):
        '''Gets offsets/functions for the class entity'''

        # Is the index on the server?
        if self.pointer is None:

            # Raise an error
            self.raise_error()

        # Is the item an offset for the class entity?
        if item in self.offsets:

            # Store the base offset address
            offset = self.pointer + self.offsets[item].offset

            # Is the item a "type"?
            if self.offsets[item].type in gSPE.Types:

                # Return an spe.makeObject object from the given offset
                return makeObject(self.offsets[item].type, offset)

            # Is the offset of type String?
            if self.offsets[item].type == 'S':

                # Create an empty string to start with
                return_value = ''

                # Set a variable to store the current character position
                current_offset = 0

                # Start a while loop to get all values of the string
                while True:

                    # Get the current offset's value
                    value = getLocVal('i', offset + current_offset)

                    # Get the remainder of the current offset's value
                    value %= 256

                    # Is the value 0?
                    if value == 0:

                        # Stop the loop at the end of the character string
                        break

                    # Get the character value of the current offset's value
                    value = chr(value)

                    # Add the character to the return value
                    return_value += value

                    # Add 1 to the offset for the next loop
                    current_offset += 1

                # Return the value
                return return_value

            # Return the current value of the offset
            return getLocVal(self.offsets[item].type, offset)

        # Is the item a function for the class entity?
        if item in self.functions:

            # Make sure the first parameter is
            # the pointer when the function is called
            self.functions[item].current_pointer = self.pointer

            # Return the function so that it gets called
            return self.functions[item].pointer_call_function

        # Raise an error, since the item wasn't
        # an offset or function for the class entity
        raise AttributeError('Invalid attribute "' + item +
            '" for "' + self.entity + '"')

    def __setitem__(self, item, value):
        '''Redirects setting an item to the set() method'''

        # Forward to the set() method
        self.set(item, value)

    def __setattr__(self, item, value):
        '''Redirects setting an attribute to the set() method'''

        # Forward to the set() method
        self.set(item, value)

    def set(self, item, *values):
        '''Sets offsets/functions for the class entity'''

        # Is the index on the server?
        if self.pointer is None:

            # Raise an error
            self.raise_error()

        # Is the item an offset for the class entity?
        if item in self.offsets:

            # Store the base offset address
            offset = self.pointer + self.offsets[item].offset

            # Is the item a "type"?
            if self.offsets[item].type in gSPE.Types:

                # Store the types dictionary
                info = gSPE.Types[self.offsets[item].type]['typeinfo']

                # Is the value of values a single tuple/list argument?
                if len(values) == 1 and type(
                  values[0]).__name__ in ('list', 'tuple'):

                    # Correct the value of values
                    values = tuple(values[0])

                # Are there the proper number of arguments?
                if len(values) != len(info):

                    # Raise an error
                    raise TypeError(
                        'Wrong number of arguments passed for %s. ' % item +
                        'Requires %s gave %s' % (len(info), len(values)))

                # Get the SPEObject instance
                offset_object = makeObject(self.offsets[item].type, offset)

                # Loop through the number of values to change
                for attribute in xrange(len(info)):

                    # Make sure the given value is of the correct type
                    value = RETURN_TYPES[
                        info[attribute]['type']](values[attribute])

                    # Set the offset
                    offset_object.__setattr__(info[attribute]['name'], value)

                # No need to go further, so return
                return

            # Is the offset of type String?
            if self.offsets[item].type == 'S':

                # Set values to the first argument given
                values = values[0]

                # Find out the length of the string
                length = len(values)

                # Loop through the length of the string being set
                for value in xrange(length):

                    # Set the value of the current character
                    setLocVal('i', offset + value, ord(values[value]))

                # Set the next character to \0 to end the string
                setLocVal('i', offset + length, 0)

                # No need to go further, so return
                return

            # Type-cast the set value to make sure it is the correct type
            value = RETURN_TYPES[self.offsets[item].type](values[0])

            # Set the offset to the new value
            setLocVal(self.offsets[item].type, offset, value)

            # No need to go further, so return
            return

        # Is the item a function for the class entity?
        if item in self.functions:

            # Call the function with the pointer as the first argument
            # Return any value(s) from the function
            return self.functions[item].call_function(self.pointer, *values)

        # Raise an error, since the item wasn't
        # an offset or function for the entity
        raise AttributeError('Invalid attribute "' + item +
            '" for "' + self.entity + '"')

    def raise_error(self):
        '''Raises an error when the index's pointer is not found on the server
        '''

        # Raise the error
        raise IndexError(
            'Index "%s" does not exist on the server' % self.index)

    def get_offsets(self):
        '''Returns a dictionary of offsets for the entity'''

        # Return a dictionary of offsets for the entity
        return self.offsets

    def get_functions(self):
        '''Returns a dictionary of functions for the entity'''

        # Get the functions for the entity
        values = self.functions

        # Return a dictionary with all the attributes for the functions
        return dict(zip(
            values.keys(), [values[item].__dict__ for item in values]))

    @property
    def entity(self):
        '''Returns the name of the class (the type of entity)'''

        # Return the entity type's name
        return self.__class__.__name__.lower()[3:]

    @property
    def offsets(self):
        '''Returns a dictionary of all offsets available to the entity'''

        # Get the base entity offsets
        values = dict(OFFSETS['entity'])

        # Update the dictionary with the entity's offsets
        values.update(OFFSETS[self.entity])

        # Return the dictionary
        return values

    @property
    def functions(self):
        '''Returns a dictionary of all functions available to the entity'''

        # Get the base entity functions
        values = dict(FUNCTIONS['entity'])

        # Update the dictionary with the entity's functions.
        # Using the dictionary method "update" ensures that if a
        # function shortname exists for both the base entity functions
        # and the classes entity functions, that the classes will be used
        values.update(FUNCTIONS[self.entity])

        # Return the dictionary
        return values

    @property
    def pointer(self):
        '''Returns the pointer for the instance's index'''

        # Return the pointer of the entity index
        return getEntityOfIndex(self.index)

    @property
    def classname(self):
        '''Returns the classname of the instance's index'''

        # Return the classname of the entity index
        return getEntityClassName(self.pointer)

    @property
    def handle(self):
        '''Returns the handle of the instance's index'''

        # Return the handle of the entity index
        return gethandlefromindex(self.index)

    @classmethod
    def get_instance_from_handle(cls, handle):
        '''Returns a class instance for the given handle'''

        # Get the index of the handle
        index = getindexfromhandle(handle)

        # Return a class instance for the index
        return cls(index)

    @classmethod
    def get_instance_from_pointer(cls, pointer):
        '''Returns a class instance for the given pointer'''

        # Get the index of the pointer
        index = getIndexOfEntity(pointer)

        # Return a class instance for the index
        return cls(index)


# =============================================================================
# >> FUNCTIONS
# =============================================================================
def get_offsets(entity='entity'):
    '''Returns a dictionary of all the offsets
        available to the specified entity'''

    # Get the base entity offsets
    values = dict(OFFSETS['entity'])

    # Update the dictionary with the specified entity's offsets
    values.update(OFFSETS[entity])

    # Return the dictionary
    return values


def get_functions(entity='entity'):
    '''Returns a dictionary of all the functions
        available to the specified entity'''

    # Get the base entity functions
    values = dict(FUNCTIONS['entity'])

    # Update the dictionary with the specified entity's functions
    values.update(FUNCTIONS[entity])

    # Return a dictionary with all the attributes for the entity's functions
    return dict(zip(values.keys(), [values[item].__dict__ for item in values]))
