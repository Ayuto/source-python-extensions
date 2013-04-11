"""
$Rev$
$LastChangedDate$
"""

# =============================================================================
# Source-Python Extensions Library:
#   Base Library
# =============================================================================

# =============================================================================
# Imports
# =============================================================================

# Python Imports
import os
import binascii
import ctypes
from configobj import ConfigObj
from os import name as platform

# Eventscripts Imports
import es

# SPE Imports
from spe_C import *

# =============================================================================
# Versioning
# =============================================================================
__revision__ = '$Rev$'
__version__ = '1.6.0a r'
es.ServerVar(
    'spe_version', __version__ + __revision__.strip('$Rev: ')).makepublic()


# =============================================================================
# Exceptions
# =============================================================================
class InvalidFunctionNameException(Exception):
    pass


class FunctionAddressNotValid(Exception):
    pass


class ConventionError(Exception):
    pass


class MismatchedTypesAndNames(Exception):
    pass


class UndefinedTypeError(Exception):
    pass


class UnsupportedOperation(Exception):
    pass


# =============================================================================
# Enums
# =============================================================================
class Conventions:
    '''
    This class defines constants which denote various x86
    calling conventions. NOTE: Using the wrong calling convention will
    cause crashing!
    '''
    CDECL, THISCALL, STDCALL = range(3)


class HookAction:
    '''
    This class defines various constants for hook actions. A hook
    action is basically what you want to do after your callback is
    executed.

    Continue - Call original function like normal.
    Modified - Call original function with modified parameters.
    Override - Block original function and use custom return value.
    Error - Internal use only.
    '''
    Continue, Modified, Override = range(3)
    Error = -1


class HookType:
    '''
    This class defines the two hook types supported by SPE.
    PRE - Call your callback before the original function.
    POST - Call your callback after the original function.
    '''
    Pre, Post = range(2)


# =============================================================================
# >> Classes
# =============================================================================
class SPEObject(object):

    def __init__(self, name, basePtr, varPositions):
        '''
        This is the SPEObject constructor. DO NOT try to instantiate one of
        these directly. SPE will automatically parse and add attributes to this
        class for use with your datatype. Use spe.makeObject instead.
        '''
        #self.base = basePtr
        #self.name = name
        #self.varlist = {}
        self.__dict__['base'] = basePtr
        self.__dict__['name'] = name
        self.__dict__['varlist'] = {}

        # Temporary base pointer
        tmpBase = basePtr

        # Iterate over every member variable.
        for var in varPositions['typeinfo']:
            # If it is a native type...
            if var['type'] in ['i', 'f', 'b', 'p']:
                self.varlist[var['name']] = {
                    'addr': tmpBase, 'type': var['type']}
                tmpBase += 4

            # Char is special case because it is 1 byte.
            elif var['type'] in ['c']:
                self.varlist[var['name']] = {
                    'addr': tmpBase, 'type': var['type']}
                tmpBase += 1

            # Found a non-native type.
            else:
                # Is the type in the list?
                if var['type'] in gSPE.Types:
                    # Non-native type. Create the object. This basically
                    # recurses until we hit basetypes.
                    obj = makeObject(var['type'], tmpBase)

                    # Store the object.
                    self.varlist[var['name']] = obj

                    # Now Increment the base pointer.
                    tmpBase += obj.size

        # Yay, done parsing the object. We need to set the size.
        self.__dict__['size'] = varPositions['size']

    def __getattr__(self, name):
        '''
        We need to override this so that we can use spe.getLocVal
        in order to grab variable information.
        '''

        # Find the object in our variable list.
        if name in self.varlist:
            # Get the value.
            val = self.varlist[name]

            # If it's an integer, this means we're at an offset. We need to
            # Ask SPE to get this information for us.
            if type(val).__name__ == 'dict':
                return getLocVal(val['type'], val['addr'])

            # This means it was an SPEObject. Return that instead.
            return val

        return object.__getattribute__(self, name)

    def __setattr__(self, name, value):
        '''
        We override this in order to set values at locations.
        '''

        # Does the object exist in our variable list?
        if name in self.varlist:
            # Get the value.
            val = self.varlist[name]

            # If it's a dict, this means we are at a base type. We can
            # set basetypes using atomic operations.
            if type(val).__name__ == 'dict':
                setLocVal(val['type'], val['addr'], value)

            else:
                # Setting whole SPE objects is not supported yet.
                raise UnsupportedOperation('SPE currently does not '
                    'support setting SPEObject instances.')


# =============================================================================
# >> Signature Class
# =============================================================================
class Signature(object):
    '''
    This is the primary class for calling Signature-based functions.
    Requirements for this class are:
        * Identifier (identifier):
            - The actual "Windows Signature" or "Linux Symbol".
            - "Windows Signatures" must be previously formatted
                with "backslash x" instead of spaces.
        * Parameter Format (param_format):
            - The type of each argument, and the return type:
                p = Pointer
                i = Integer
                f = Float
                B = Boolean
                d = Double
                L = Long
                S = String
        * Convention (convention):
            - thiscall
                Used when calling funtions from (within a C++ class)
            - cdecl
                Used when calling functions that are global
                    (not part of a C++ class)
            - fastcall
                (Reserved)
    '''
    def __init__(self, identifier, param_format, convention):
        # IDENTIFIER (Windows Signature or Linux Symbol)
        self.identifier = str(identifier)

        # FUNCTION (memory address)
        if platform == 'nt':
            self.function = findFunction(
                self.identifier, len(self.identifier.decode('string_escape')))
        else:
            self.function = findSymbol(self.identifier)

        # PARAMETER FORMAT
        self.param_format = str(param_format)

        # CONVENTION TYPE
        if convention not in ['thiscall', 'cdecl', 'fastcall']:
            raise ConventionError('"' + str(convention) + '" is not a valid'
                ' convention: "thiscall", "cdecl", or "fastcall" only.')

        self.convention = str(convention)

    def call(self, args=()):
        # Weird... Somehow this fixes crashes on Linux, although it doesn't
        # change the variable "args"
        tuple(args)
        
        # Set the calling convention
        setCallingConvention(self.convention)

        # Call the function and return the results
        return callFunction(self.function, self.param_format, args)


# =============================================================================
# >> SPE Manager class
# =============================================================================
class CSPEManager(object):
    '''
    This class is the main SPE module manager.
    Do not access any methods from this class directly.
    Instead, use the exported functions down below.
    '''
    def __init__(self):
        # Setup the game name
        self.game_name = str(
            os.path.split(str(es.ServerVar('eventscripts_gamedir')))[1])

        # Setup signature dictionary
        self.Signatures = {}

        # Setup type dictionary.
        self.Types = {}

        # Initialize SPE
        self.initializeSPE()

    def initializeSPE(self):
        '''
        Parses shared and game-specific signatures from the ini files.
        Loads up the game specific module so it can be used by SPE.
        '''
        # Setup signatures
        self.parseINI('_libs/python/spe/ini/'
            'engines/shared.' + str(es.ServerVar('spe_engine') + '.ini'))
        self.parseINI('_libs/python/spe/ini/games/' + self.game_name + '.ini')

        # Setup types.
        self.parseTypesINI('_libs/python/spe/types/shared.ini')

        # Load the shared and game module
        self.loadModule('spe.games.shared')
        self.loadModule('spe.games.' + self.game_name)

    def parseTypesINI(self, path):
        '''
        Parses signatures from an INI file.
        '''
        gamedir = es.ServerVar('eventscripts_gamedir')

        # Create an INI object!
        INI = ConfigObj(str(gamedir) + '/addons/eventscripts/' + str(path))

        # Loop through each section in the INI
        for section in INI:

            # Time for the hard shit. Get the shortname of the type.
            shortname = INI[section]['shortname']

            # Skip this type if we already have it.
            if shortname in self.Types:
                continue

            # Get the type and name string.
            types = INI[section]['types']
            names = INI[section]['names']

            # Split them into lists.
            types = types.split(',')
            names = names.split(',')

            # Make sure the lengths match up (need a name for every var).
            if len(types) != len(names):
                raise MismatchedTypesAndNames(
                    'The name string and type string don\'t line up!')

            # Construct the variables for the type.
            self.Types[shortname] = {'typeinfo': [], 'size': 0}

            # Keeps track of the current variable offset.
            curOffset = 0

            # Iterate over all of the types.
            for name, type in zip(names, types):

                # Construct a dict for each variable in the object.
                typedict = {'name': name, 'type': type, 'offset': curOffset}

                # We need to figure out the offset of each variable.
                # If the current variable is a native type,
                # we can do this immediately.
                # If it isn't, we need to search the dictionary.
                if type in ['i', 'p', 'f', 'd']:
                    # Next free offset is 4 bytes away.
                    curOffset += 4

                elif type in ['c']:
                    # Next offset is 1 byte away.
                    curOffset += 1

                # If we are here, it means that the type is a non-native type.
                # Does it exist in the dictionary?
                else:
                    if type in self.Types:
                        # Calculate the next free offset.
                        curOffset += self.Types[type]['size']
                    else:
                        # This means we have an undefined type.
                        raise UndefinedTypeError('Type of name "' + str(type) +
                            '" not found in dictionary! Did you define it?')

                # Now push the constructed dict into the list.
                self.Types[shortname]['typeinfo'].append(typedict)

            # Store the size too.
            self.Types[shortname]['size'] = curOffset

    def parseINI(self, path):
        '''
        Parses signatures from an INI file.
        '''
        gamedir = es.ServerVar('eventscripts_gamedir')

        # Create an INI object!
        INI = ConfigObj(str(gamedir) + '/addons/eventscripts/' + str(path))

        # Loop through each section in the INI
        for section in INI:

            # Skip this signature if we already have it.
            if INI[section]['shortname'] in self.Signatures:
                continue

            # Check to see if the OS is Windows
            if platform == 'nt':
                # If the signature contains spaces,
                # convert the signature to proper readable form
                if ' ' in INI[section]['sig']:
                    sig = binascii.unhexlify(
                        ''.join(INI[section]['sig'].split()))
                # If there are no spaces, read the signature as-is
                else:
                    sig = INI[section]['sig']

                # Add the signature to the gSignatureDictionary as
                # a Signature() instance via the INI's "shortname"
                self.Signatures[INI[section]['shortname']] = Signature(
                    sig, INI[section]['param'], INI[section]['convention'])

            # If the OS is UNIX, add the symbol to the gSignatureDictionary
            # as a Signature() instance via the INI's "shortname"
            else:
                self.Signatures[INI[section]['shortname']] = Signature(
                    INI[section]['symbol'], INI[section]['param'],
                    INI[section]['convention'])

    def moduleExists(self, module_name):
        '''
        Tests to see if a particular module can be imported.
        '''
        try:
            mod = __import__(module_name)
        except ImportError:
            return False
        else:
            return True

    def loadModule(self, module_name):
        '''
        - Written by XE_ManUp! -

        ** THIS FUNCTION IS HIGHLY UNPYTHONIC **

        This basically takes all of the methods and class instances
        within the module of module_name and adds them to SPE.
        This way, you can call functions within that module
        directly from SPE. Example: spe.<functionInModule>(args).
        '''
        import inspect

        if self.moduleExists(module_name):
            mod = __import__(module_name, globals(), locals(), [''])
            for item in mod.__dict__:
                if (callable(mod.__dict__[item])
                  or inspect.isclass(mod.__dict__[item])):
                    globals()[item] = mod.__dict__[item]
                elif type(mod.__dict__[item]).__name__ in mod.__dict__:
                    if inspect.isclass(
                      mod.__dict__[type(mod.__dict__[item]).__name__]):
                        globals()[item] = mod.__dict__[item]

    def call(self, name, *args):
        '''
        Calls a function that is already in the global signature
            dictionary. These functions must be present within either
            the shared.ini or the mod specific ini file. The name
            parameter corresponds to the shortname of the function
            in the ini file.
        '''

        # If the function name is not found
        if not name in self.Signatures:

            # Raise an exception
            raise InvalidFunctionNameException(
                'Could not find ' + str(name) + ' in the dictionary!')

        # Otherwise, call the function using the Signature() instance
        # contained within the self.Signatures via the function name
        return self.Signatures[name].call(args)

    def detourFunction(self, functionName, hooktype, callback):
        # Is the function in the list?
        if not functionName in self.Signatures:

            # Raise an exception
            raise InvalidFunctionNameException(
                'Could not find ' + str(functionName) + ' in the dictionary!')

        # Get the signature object.
        sigObj = self.Signatures[functionName]

        # Determine the calling convention.
        convIdx = ['cdecl', 'thiscall', 'stdcall'].index(sigObj.convention)

        # Make sure the function address is valid.
        if sigObj.function == None:
            raise FunctionAddressNotValid(functionName + "\'s address is not valid!")

        # Hook the function
        hookFunction(sigObj.function, sigObj.param_format, convIdx, int(hooktype), callback)

    def undetourFunction(self, functionName, hooktype, callback):
        # Is the function in the list?
        if not functionName in self.Signatures:

            # Raise an exception
            raise InvalidFunctionNameException(
                'Could not find ' + str(functionName) + ' in the dictionary!')

        # Get the signature object.
        sigObj = self.Signatures[functionName]

        # Determine the calling convention.
        convIdx = ['cdecl', 'thiscall', 'stdcall'].index(sigObj.convention)

        # Make sure the function address is valid.
        if sigObj.function == None:
            raise FunctionAddressNotValid(functionName + "\'s address is not valid!")
        
        # Unhook the function
        unHookFunction(sigObj.function, int(hooktype), callback)

    def makeObject(self, objectName, baseAddress):
        # Do we have the requested object in our list?
        if objectName in self.Types:
            return SPEObject(objectName, baseAddress, self.Types[objectName])
        else:
            raise UndefinedTypeError(
                'Type ' + str(objectName) + ' not registered with SPE!')

gSPE = CSPEManager()


# =============================================================================
# >> Exported functions
# =============================================================================
def parseINI(path):
    '''
    This function parses signatures from an INI file.
    Path represents the path to the INI file, with the
    base being cstrike/addons/eventscripts/. You must
    put in the .ini extension manually into the path.
    It is not done for you.
    '''
    gSPE.parseINI(path)


def parseTypesINI(path):
    '''
    This function parses custom type definitions from an ini
    of your choosing. You can then pass the shortname of a
    custom type in the ini to spe.makeObject and it will
    return a class instance comprised of the symbolic names
    of all variables in the declared type.
    '''
    gSPE.parseTypesINI(path)


def call(name, *args):
    '''
    Calls a sigscanned function based upon its shortname
    in the INI file. args of course are the arguments to
    said function.
    '''
    return gSPE.call(name, *args)


def getPointer(signature, offset):
    '''
    This function is very low level. It allows one to rip
    out C++ class instances directly from memory which are
    not normally accessible by server plugins. The signature
    parameter (which requires that the signature that is
    passed in uses hex escape characters) is the signature
    of the function that the instance is referenced in. The
    offset parameter is the number of bytes away from the
    beginning of the signature that the instance itself
    is referenced at (the beginning of the address bytes
    of the instance).

    IF YOU DO NOT KNOW WHAT YOU ARE DOING, DON'T USE THIS!
    '''

    # Find the function
    pFunc = findFunction(signature, len(signature.decode('string_escape')))

    # Rip the pointer
    return (pFunc + offset)


def detourFunction(functionName, type, callback):
    '''
    This function will hook a function as defined by a signature,
    and redirect its execution to your own python callback.
    NOTE: You must have loaded the signature via spe.parseINI!
    '''
    gSPE.detourFunction(functionName, type, callback)


def undetourFunction(functionName, type, callback):
    '''
    This function will remove a python callback from a detour.
    '''
    gSPE.undetourFunction(functionName, type, callback)


def makeObject(objectName, baseAddress):
    '''
    Returns a special python wrapper around a C++ object instance.
    '''
    return gSPE.makeObject(objectName, baseAddress)
