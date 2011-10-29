# ../_libs/python/spe/tools/__init__.py

# =============================================================================
# >> IMPORTS
# =============================================================================
# SPE Imports
from spe import getEntityClassName
from spe import getEntityOfIndex
from spe import getLocVal
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

            # Return the current value of the offset
            return getLocVal(self.offsets[item].type,
                self.pointer + self.offsets[item].offset)

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

            # Type-cast the set value to make sure it is the correct type
            value = RETURN_TYPES[self.offsets[item].type](values[0])

            # Set the offset to the new value
            setLocVal(self.offsets[item].type,
                self.pointer + self.offsets[item].offset, value)

            # Return, since the loop should not run anymore
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
        values = OFFSETS['entity']

        # Update the dictionary with the entity's offsets
        values.update(OFFSETS[self.entity])

        # Return the dictionary
        return values

    @property
    def functions(self):
        '''Returns a dictionary of all functions available to the entity'''

        # Get the base entity functions
        values = FUNCTIONS['entity']

        # Update the dictionary with the entity's functions
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
