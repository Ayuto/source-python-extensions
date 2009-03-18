#================================================================================
# Source-Python Extensions Library:
#   Base Library
#================================================================================

#================================================================================
# Imports
#================================================================================
# EventScripts Imports
import es

# Source-Python Extensions Imports
from esx_C import *

# Python Imports
from os import name as platform
from configobj import ConfigObj
import binascii
import os

#================================================================================
# Globals
#================================================================================
gSignatureDictionary = {}
__version__ = '1.0.5c'
es.ServerVar('spe_version', __version__).makepublic()
gamePathName = str(os.path.split(str(es.ServerVar('eventscripts_gamedir')))[1])

#================================================================================
# Exceptions
#================================================================================
class InvalidFunctionNameException(Exception):
    pass

class ConventionError(Exception):
    pass

#================================================================================
# >> Signature Class
#================================================================================
class Signature(object):
    '''
    This is the primary class for calling Signature-based functions. Requirements for this class are:
        * Identifier (identifier):
            - The actual "Windows Signature" or "Linux Symbol".
            - "Windows Signatures" must be previously formatted with "backslash x" instead of spaces.
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
                Used when calling functions that are global (not part of a C++ class)
            - fastcall
                (Reserved)
    '''
    def __init__(self, identifier, param_format, convention):
        # IDENTIFIER (Windows Signature or Linux Symbol)
        self.identifier = str(identifier)
        
        # FUNCTION (memory address)
        if platform == 'nt':
            self.function = findFunction(self.identifier, len(self.identifier.decode('string_escape')))
        else:
            self.function = findSymbol(self.identifier)
            
        # PARAMETER FORMAT
        self.param_format = str(param_format)
        
        # CONVENTION TYPE
        if convention not in ['thiscall', 'cdecl', 'fastcall']:
            raise ConventionError('"%s" is not a valid convention: "thiscall", "cdecl", or "fastcall" only.' %convention)
            
        self.convention = str(convention)
            
    def call(self, args=()):
        # Convert the arguments to a tuple
        if not isinstance(args, tuple):
            args = (args,)
            
        # Set the calling convention
        setCallingConvention(self.convention)
        
        # Call the function and return the results
        return callFunction(self.function, self.param_format, args)

#================================================================================
# >> Parses an ini file and adds the signatures to a dictionary.
#================================================================================
def ParseSignatures( ini_name ):
    # Get gamedirectory
    gamedir = es.ServerVar('eventscripts_gamedir')
    
    # Create an INI object!
    INI = ConfigObj('%s/addons/eventscripts/_libs/python/spe/ini/%s.ini' % (gamedir, ini_name))
    
    # Loop through each section in the INI
    for section in INI:
        # Loop through each key in the section of the INI
        for key in INI[section]:
            # Check to see if the OS is Windows
            if platform == 'nt':
                # If the signature contains spaces, convert the signature to proper readable form
                if ' ' in INI[section]['sig']:
                    sig = binascii.unhexlify(''.join(INI[section]['sig'].split()))
                # If there are no spaces, read the signature as-is
                else:
                    sig = INI[section]['sig']
                # Add the signature to the gSignatureDictionary as a Signature() instance via the INI's "shortname"
                gSignatureDictionary[INI[section]['shortname']] = Signature(sig, INI[section]['param'], INI[section]['convention'])
            # If the OS is UNIX, add the symbol to the gSignatureDictionary as a Signature() instance via the INI's "shortname"
            else:
                gSignatureDictionary[INI[section]['shortname']] = Signature(INI[section]['symbol'], INI[section]['param'], INI[section]['convention'])

#================================================================================
# >> Initializes Signatures
#================================================================================
def InitializeSignatures():
    # Load the global INI first
    ParseSignatures('shared')
    
    # Load the game specific INI
    ParseSignatures(gamePathName)
    
# Initialize the signatures
InitializeSignatures()

#================================================================================
# >> Calls a function
#================================================================================
def Call(name, *args):
    # Make sure the argument is a tuple
    if not isinstance(args, tuple):
        args = (args,)

    # If the function name is not found
    if not gSignatureDictionary.has_key(name):
        # Raise an exception
        raise InvalidFunctionNameException("Could not find %s in the dictionary!" % name)
    
    # Otherwise, call the function using the Signature() instance contained within the gSignatureDictionary via the function name
    return gSignatureDictionary[name].call(args)

#================================================================================
# >> Calls a function shortcut
#================================================================================
def CallShortcut(function, *args):
    # Import the mod-specific module
    mod = __import__('spe.games.%s' %gamePathName, fromlist=['games'])
    
    # Import the shared module
    shared = __import__('spe.games.shared', fromlist=['games'])
    
    # Attempt to call the "mod" function
    if mod.__dict__.has_key(function):
        if not callable(mod.__dict__[function]):
            return
        mod.__dict__[function](*args)
    # Attempt to call the "shared" function
    elif shared.__dict__.has_key(function):
        if not callable(shared.__dict__[function]):
            return
        shared.__dict__[function](*args)
    # No function found
    else:
        raise InvalidFunctionNameException('"%s" function not found in the "shared" or "%s" function list.'
            %(str(function), gamePathName))