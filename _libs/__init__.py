"""
$Rev$
$LastChangedDate$
"""

#==============================================================================
# Source-Python Extensions Library:
#   Base Library
#==============================================================================

#==============================================================================
# Imports
#==============================================================================

# Python Imports
import os
import binascii
from configobj import ConfigObj
from os import name as platform

# Eventscripts Imports
import es

# SPE Imports
from spe_C import *

#==============================================================================
# Versioning
#==============================================================================
__revision__ = "$Rev$"
__version__ = '1.5.0f r'
es.ServerVar("spe_version",
    __version__ + __revision__.strip("$Rev: ")).makepublic()


#==============================================================================
# Exceptions
#==============================================================================
class InvalidFunctionNameException(Exception):
    pass


class ConventionError(Exception):
    pass


#==============================================================================
# >> Classes
#==============================================================================


#==============================================================================
# >> Signature Class
#==============================================================================
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
            self.function = findFunction(self.identifier,
                len(self.identifier.decode('string_escape')))
        else:
            self.function = findSymbol(self.identifier)

        # PARAMETER FORMAT
        self.param_format = str(param_format)

        # CONVENTION TYPE
        if convention not in ['thiscall', 'cdecl', 'fastcall']:
            raise (ConventionError, '"%s" is not a valid convention: ' +
                '"thiscall", "cdecl", or "fastcall" only.' % convention)

        self.convention = str(convention)

    def call(self, args=()):

        # Set the calling convention
        setCallingConvention(self.convention)

        # Call the function and return the results
        return callFunction(self.function, self.param_format, *args)


#==============================================================================
# >> SPE Manager class
#==============================================================================
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

        # Initialize SPE
        self.initializeSPE()

    def initializeSPE(self):
        '''
        Parses shared and game-specific signatures from the ini files.
        Loads up the game specific module so it can be used by SPE.
        '''
        # Setup signatures
        self.parseINI("_libs/python/spe/ini/engines/shared." +
            str(es.ServerVar("spe_engine")) + ".ini")
        self.parseINI("_libs/python/spe/ini/games/%s.ini" % self.game_name)

        # Load the shared and game module
        self.loadModule("spe.games.shared")
        self.loadModule("spe.games.%s" % self.game_name)

    def parseINI(self, path):
        '''
        Parses signatures from an INI file.
        '''
        gamedir = es.ServerVar("eventscripts_gamedir")

        # Create an INI object!
        INI = ConfigObj('%s/addons/eventscripts/%s' % (gamedir, path))

        # Loop through each section in the INI
        for section in INI:
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

                # Add the signature to the gSignatureDictionary as a
                # Signature() instance via the INI's "shortname"
                self.Signatures[INI[section]['shortname']] = Signature(
                    sig, INI[section]['param'], INI[section]['convention'])

            # If the OS is UNIX, add the symbol to the gSignatureDictionary as
            # a Signature() instance via the INI's "shortname"
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
                if (callable(mod.__dict__[item]) or
                  inspect.isclass(mod.__dict__[item])):
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
            raise (InvalidFunctionNameException,
                "Could not find %s in the dictionary!" % name)

        # Otherwise, call the function using the Signature() instance
        # contained within the self.Signatures via the function name
        return self.Signatures[name].call(args)

gSPE = CSPEManager()


#==============================================================================
# >> Exported functions
#==============================================================================
def parseINI(path):
    '''
    This function parses signatures from an INI file.
    Path represents the path to the INI file, with the
    base being cstrike/addons/eventscripts/. You must
    put in the .ini extension manually into the path.
    It is not done for you.
    '''
    gSPE.parseINI(path)


def call(name, *args):
    '''
    Calls a sigscanned function based upon its shortname
    in the INI file. args of course are the arguments to
    said function.
    '''
    return gSPE.call(name, args)


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
