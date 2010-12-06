# This file was created automatically by SWIG.
# Don't modify this file, modify the SWIG interface instead.
# This file is compatible with both classic and new-style classes.
import _fluxspacecore
def _swig_setattr(self,class_type,name,value):
    if (name == "this"):
        if isinstance(value, class_type):
            self.__dict__[name] = value.this
            if hasattr(value,"thisown"): self.__dict__["thisown"] = value.thisown
            del value.thisown
            return
    method = class_type.__swig_setmethods__.get(name,None)
    if method: return method(self,value)
    self.__dict__[name] = value

def _swig_getattr(self,class_type,name):
    method = class_type.__swig_getmethods__.get(name,None)
    if method: return method(self)
    raise AttributeError,name

import types
try:
    _object = types.ObjectType
    _newclass = 1
except AttributeError:
    class _object : pass
    _newclass = 0


class Error(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, Error, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, Error, name)
    def __init__(self,*args,**kwargs):
        _swig_setattr(self, Error, 'this', apply(_fluxspacecore.new_Error,args, kwargs))
        _swig_setattr(self, Error, 'thisown', 1)
    def __del__(self, destroy= _fluxspacecore.delete_Error):
        try:
            if self.thisown: destroy(self)
        except: pass
    __swig_setmethods__["msg"] = _fluxspacecore.Error_msg_set
    __swig_getmethods__["msg"] = _fluxspacecore.Error_msg_get
    if _newclass:msg = property(_fluxspacecore.Error_msg_get,_fluxspacecore.Error_msg_set)
    def __repr__(self):
        return "<C Error instance at %s>" % (self.this,)

class ErrorPtr(Error):
    def __init__(self,this):
        _swig_setattr(self, Error, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, Error, 'thisown', 0)
        _swig_setattr(self, Error,self.__class__,Error)
_fluxspacecore.Error_swigregister(ErrorPtr)

class InternalError(Error):
    __swig_setmethods__ = {}
    for _s in [Error]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, InternalError, name, value)
    __swig_getmethods__ = {}
    for _s in [Error]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, InternalError, name)
    def __init__(self,*args,**kwargs):
        _swig_setattr(self, InternalError, 'this', apply(_fluxspacecore.new_InternalError,args, kwargs))
        _swig_setattr(self, InternalError, 'thisown', 1)
    def __del__(self, destroy= _fluxspacecore.delete_InternalError):
        try:
            if self.thisown: destroy(self)
        except: pass
    def __repr__(self):
        return "<C InternalError instance at %s>" % (self.this,)

class InternalErrorPtr(InternalError):
    def __init__(self,this):
        _swig_setattr(self, InternalError, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, InternalError, 'thisown', 0)
        _swig_setattr(self, InternalError,self.__class__,InternalError)
_fluxspacecore.InternalError_swigregister(InternalErrorPtr)

class ErrorImportModule(Error):
    __swig_setmethods__ = {}
    for _s in [Error]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, ErrorImportModule, name, value)
    __swig_getmethods__ = {}
    for _s in [Error]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, ErrorImportModule, name)
    def __init__(self,*args,**kwargs):
        _swig_setattr(self, ErrorImportModule, 'this', apply(_fluxspacecore.new_ErrorImportModule,args, kwargs))
        _swig_setattr(self, ErrorImportModule, 'thisown', 1)
    def __del__(self, destroy= _fluxspacecore.delete_ErrorImportModule):
        try:
            if self.thisown: destroy(self)
        except: pass
    def __repr__(self):
        return "<C ErrorImportModule instance at %s>" % (self.this,)

class ErrorImportModulePtr(ErrorImportModule):
    def __init__(self,this):
        _swig_setattr(self, ErrorImportModule, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, ErrorImportModule, 'thisown', 0)
        _swig_setattr(self, ErrorImportModule,self.__class__,ErrorImportModule)
_fluxspacecore.ErrorImportModule_swigregister(ErrorImportModulePtr)

class ErrorNoDisplay(Error):
    __swig_setmethods__ = {}
    for _s in [Error]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, ErrorNoDisplay, name, value)
    __swig_getmethods__ = {}
    for _s in [Error]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, ErrorNoDisplay, name)
    def __init__(self,*args,**kwargs):
        _swig_setattr(self, ErrorNoDisplay, 'this', apply(_fluxspacecore.new_ErrorNoDisplay,args, kwargs))
        _swig_setattr(self, ErrorNoDisplay, 'thisown', 1)
    def __del__(self, destroy= _fluxspacecore.delete_ErrorNoDisplay):
        try:
            if self.thisown: destroy(self)
        except: pass
    def __repr__(self):
        return "<C ErrorNoDisplay instance at %s>" % (self.this,)

class ErrorNoDisplayPtr(ErrorNoDisplay):
    def __init__(self,this):
        _swig_setattr(self, ErrorNoDisplay, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, ErrorNoDisplay, 'thisown', 0)
        _swig_setattr(self, ErrorNoDisplay,self.__class__,ErrorNoDisplay)
_fluxspacecore.ErrorNoDisplay_swigregister(ErrorNoDisplayPtr)

class ErrorAccessFluxbox(Error):
    __swig_setmethods__ = {}
    for _s in [Error]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, ErrorAccessFluxbox, name, value)
    __swig_getmethods__ = {}
    for _s in [Error]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, ErrorAccessFluxbox, name)
    def __init__(self,*args,**kwargs):
        _swig_setattr(self, ErrorAccessFluxbox, 'this', apply(_fluxspacecore.new_ErrorAccessFluxbox,args, kwargs))
        _swig_setattr(self, ErrorAccessFluxbox, 'thisown', 1)
    def __del__(self, destroy= _fluxspacecore.delete_ErrorAccessFluxbox):
        try:
            if self.thisown: destroy(self)
        except: pass
    def __repr__(self):
        return "<C ErrorAccessFluxbox instance at %s>" % (self.this,)

class ErrorAccessFluxboxPtr(ErrorAccessFluxbox):
    def __init__(self,this):
        _swig_setattr(self, ErrorAccessFluxbox, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, ErrorAccessFluxbox, 'thisown', 0)
        _swig_setattr(self, ErrorAccessFluxbox,self.__class__,ErrorAccessFluxbox)
_fluxspacecore.ErrorAccessFluxbox_swigregister(ErrorAccessFluxboxPtr)

class ErrorAppExists(Error):
    __swig_setmethods__ = {}
    for _s in [Error]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, ErrorAppExists, name, value)
    __swig_getmethods__ = {}
    for _s in [Error]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, ErrorAppExists, name)
    def __init__(self,*args,**kwargs):
        _swig_setattr(self, ErrorAppExists, 'this', apply(_fluxspacecore.new_ErrorAppExists,args, kwargs))
        _swig_setattr(self, ErrorAppExists, 'thisown', 1)
    def __del__(self, destroy= _fluxspacecore.delete_ErrorAppExists):
        try:
            if self.thisown: destroy(self)
        except: pass
    def __repr__(self):
        return "<C ErrorAppExists instance at %s>" % (self.this,)

class ErrorAppExistsPtr(ErrorAppExists):
    def __init__(self,this):
        _swig_setattr(self, ErrorAppExists, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, ErrorAppExists, 'thisown', 0)
        _swig_setattr(self, ErrorAppExists,self.__class__,ErrorAppExists)
_fluxspacecore.ErrorAppExists_swigregister(ErrorAppExistsPtr)

class ErrorControllerExists(Error):
    __swig_setmethods__ = {}
    for _s in [Error]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, ErrorControllerExists, name, value)
    __swig_getmethods__ = {}
    for _s in [Error]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, ErrorControllerExists, name)
    def __init__(self,*args,**kwargs):
        _swig_setattr(self, ErrorControllerExists, 'this', apply(_fluxspacecore.new_ErrorControllerExists,args, kwargs))
        _swig_setattr(self, ErrorControllerExists, 'thisown', 1)
    def __del__(self, destroy= _fluxspacecore.delete_ErrorControllerExists):
        try:
            if self.thisown: destroy(self)
        except: pass
    def __repr__(self):
        return "<C ErrorControllerExists instance at %s>" % (self.this,)

class ErrorControllerExistsPtr(ErrorControllerExists):
    def __init__(self,this):
        _swig_setattr(self, ErrorControllerExists, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, ErrorControllerExists, 'thisown', 0)
        _swig_setattr(self, ErrorControllerExists,self.__class__,ErrorControllerExists)
_fluxspacecore.ErrorControllerExists_swigregister(ErrorControllerExistsPtr)

class ErrorNoController(Error):
    __swig_setmethods__ = {}
    for _s in [Error]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, ErrorNoController, name, value)
    __swig_getmethods__ = {}
    for _s in [Error]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, ErrorNoController, name)
    def __init__(self,*args,**kwargs):
        _swig_setattr(self, ErrorNoController, 'this', apply(_fluxspacecore.new_ErrorNoController,args, kwargs))
        _swig_setattr(self, ErrorNoController, 'thisown', 1)
    def __del__(self, destroy= _fluxspacecore.delete_ErrorNoController):
        try:
            if self.thisown: destroy(self)
        except: pass
    def __repr__(self):
        return "<C ErrorNoController instance at %s>" % (self.this,)

class ErrorNoControllerPtr(ErrorNoController):
    def __init__(self,this):
        _swig_setattr(self, ErrorNoController, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, ErrorNoController, 'thisown', 0)
        _swig_setattr(self, ErrorNoController,self.__class__,ErrorNoController)
_fluxspacecore.ErrorNoController_swigregister(ErrorNoControllerPtr)

class ErrorEventLoopRunning(Error):
    __swig_setmethods__ = {}
    for _s in [Error]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, ErrorEventLoopRunning, name, value)
    __swig_getmethods__ = {}
    for _s in [Error]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, ErrorEventLoopRunning, name)
    def __init__(self,*args,**kwargs):
        _swig_setattr(self, ErrorEventLoopRunning, 'this', apply(_fluxspacecore.new_ErrorEventLoopRunning,args, kwargs))
        _swig_setattr(self, ErrorEventLoopRunning, 'thisown', 1)
    def __del__(self, destroy= _fluxspacecore.delete_ErrorEventLoopRunning):
        try:
            if self.thisown: destroy(self)
        except: pass
    def __repr__(self):
        return "<C ErrorEventLoopRunning instance at %s>" % (self.this,)

class ErrorEventLoopRunningPtr(ErrorEventLoopRunning):
    def __init__(self,this):
        _swig_setattr(self, ErrorEventLoopRunning, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, ErrorEventLoopRunning, 'thisown', 0)
        _swig_setattr(self, ErrorEventLoopRunning,self.__class__,ErrorEventLoopRunning)
_fluxspacecore.ErrorEventLoopRunning_swigregister(ErrorEventLoopRunningPtr)

class ErrorBadController(Error):
    __swig_setmethods__ = {}
    for _s in [Error]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, ErrorBadController, name, value)
    __swig_getmethods__ = {}
    for _s in [Error]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, ErrorBadController, name)
    def __init__(self,*args,**kwargs):
        _swig_setattr(self, ErrorBadController, 'this', apply(_fluxspacecore.new_ErrorBadController,args, kwargs))
        _swig_setattr(self, ErrorBadController, 'thisown', 1)
    def __del__(self, destroy= _fluxspacecore.delete_ErrorBadController):
        try:
            if self.thisown: destroy(self)
        except: pass
    def __repr__(self):
        return "<C ErrorBadController instance at %s>" % (self.this,)

class ErrorBadControllerPtr(ErrorBadController):
    def __init__(self,this):
        _swig_setattr(self, ErrorBadController, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, ErrorBadController, 'thisown', 0)
        _swig_setattr(self, ErrorBadController,self.__class__,ErrorBadController)
_fluxspacecore.ErrorBadController_swigregister(ErrorBadControllerPtr)

class ErrorNoWindow(Error):
    __swig_setmethods__ = {}
    for _s in [Error]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, ErrorNoWindow, name, value)
    __swig_getmethods__ = {}
    for _s in [Error]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, ErrorNoWindow, name)
    def __init__(self,*args,**kwargs):
        _swig_setattr(self, ErrorNoWindow, 'this', apply(_fluxspacecore.new_ErrorNoWindow,args, kwargs))
        _swig_setattr(self, ErrorNoWindow, 'thisown', 1)
    def __del__(self, destroy= _fluxspacecore.delete_ErrorNoWindow):
        try:
            if self.thisown: destroy(self)
        except: pass
    def __repr__(self):
        return "<C ErrorNoWindow instance at %s>" % (self.this,)

class ErrorNoWindowPtr(ErrorNoWindow):
    def __init__(self,this):
        _swig_setattr(self, ErrorNoWindow, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, ErrorNoWindow, 'thisown', 0)
        _swig_setattr(self, ErrorNoWindow,self.__class__,ErrorNoWindow)
_fluxspacecore.ErrorNoWindow_swigregister(ErrorNoWindowPtr)

class ErrorWindowProperty(Error):
    __swig_setmethods__ = {}
    for _s in [Error]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, ErrorWindowProperty, name, value)
    __swig_getmethods__ = {}
    for _s in [Error]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, ErrorWindowProperty, name)
    def __init__(self,*args,**kwargs):
        _swig_setattr(self, ErrorWindowProperty, 'this', apply(_fluxspacecore.new_ErrorWindowProperty,args, kwargs))
        _swig_setattr(self, ErrorWindowProperty, 'thisown', 1)
    def __del__(self, destroy= _fluxspacecore.delete_ErrorWindowProperty):
        try:
            if self.thisown: destroy(self)
        except: pass
    def __repr__(self):
        return "<C ErrorWindowProperty instance at %s>" % (self.this,)

class ErrorWindowPropertyPtr(ErrorWindowProperty):
    def __init__(self,this):
        _swig_setattr(self, ErrorWindowProperty, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, ErrorWindowProperty, 'thisown', 0)
        _swig_setattr(self, ErrorWindowProperty,self.__class__,ErrorWindowProperty)
_fluxspacecore.ErrorWindowProperty_swigregister(ErrorWindowPropertyPtr)

class Controller(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, Controller, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, Controller, name)
    def __init__(self,*args,**kwargs):
        _swig_setattr(self, Controller, 'this', apply(_fluxspacecore.new_Controller,args, kwargs))
        _swig_setattr(self, Controller, 'thisown', 1)
    def __del__(self, destroy= _fluxspacecore.delete_Controller):
        try:
            if self.thisown: destroy(self)
        except: pass
    def SetSOAPHandler(*args, **kwargs): return apply(_fluxspacecore.Controller_SetSOAPHandler,args, kwargs)
    def RegisterFluxlet(*args, **kwargs): return apply(_fluxspacecore.Controller_RegisterFluxlet,args, kwargs)
    def StartEvents(*args, **kwargs): return apply(_fluxspacecore.Controller_StartEvents,args, kwargs)
    def SetWidth(*args, **kwargs): return apply(_fluxspacecore.Controller_SetWidth,args, kwargs)
    def SetFrameWidth(*args, **kwargs): return apply(_fluxspacecore.Controller_SetFrameWidth,args, kwargs)
    def SetRowGap(*args, **kwargs): return apply(_fluxspacecore.Controller_SetRowGap,args, kwargs)
    def SetAntialias(*args, **kwargs): return apply(_fluxspacecore.Controller_SetAntialias,args, kwargs)
    def SetNormalFont(*args, **kwargs): return apply(_fluxspacecore.Controller_SetNormalFont,args, kwargs)
    def SetSmallFont(*args, **kwargs): return apply(_fluxspacecore.Controller_SetSmallFont,args, kwargs)
    def SetLargeFont(*args, **kwargs): return apply(_fluxspacecore.Controller_SetLargeFont,args, kwargs)
    def Trace(*args, **kwargs): return apply(_fluxspacecore.Controller_Trace,args, kwargs)
    def Message(*args, **kwargs): return apply(_fluxspacecore.Controller_Message,args, kwargs)
    __swig_setmethods__["displayName"] = _fluxspacecore.Controller_displayName_set
    __swig_getmethods__["displayName"] = _fluxspacecore.Controller_displayName_get
    if _newclass:displayName = property(_fluxspacecore.Controller_displayName_get,_fluxspacecore.Controller_displayName_set)
    __swig_setmethods__["verbose"] = _fluxspacecore.Controller_verbose_set
    __swig_getmethods__["verbose"] = _fluxspacecore.Controller_verbose_get
    if _newclass:verbose = property(_fluxspacecore.Controller_verbose_get,_fluxspacecore.Controller_verbose_set)
    def __repr__(self):
        return "<C Controller instance at %s>" % (self.this,)

class ControllerPtr(Controller):
    def __init__(self,this):
        _swig_setattr(self, Controller, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, Controller, 'thisown', 0)
        _swig_setattr(self, Controller,self.__class__,Controller)
_fluxspacecore.Controller_swigregister(ControllerPtr)

class FluxletHelper(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, FluxletHelper, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, FluxletHelper, name)
    def __init__(self,*args,**kwargs):
        _swig_setattr(self, FluxletHelper, 'this', apply(_fluxspacecore.new_FluxletHelper,args, kwargs))
        _swig_setattr(self, FluxletHelper, 'thisown', 1)
    def __del__(self, destroy= _fluxspacecore.delete_FluxletHelper):
        try:
            if self.thisown: destroy(self)
        except: pass
    def AddHandler(*args, **kwargs): return apply(_fluxspacecore.FluxletHelper_AddHandler,args, kwargs)
    def RegisterSOAPObject(*args, **kwargs): return apply(_fluxspacecore.FluxletHelper_RegisterSOAPObject,args, kwargs)
    def SetRoot(*args, **kwargs): return apply(_fluxspacecore.FluxletHelper_SetRoot,args, kwargs)
    def AddRow(*args, **kwargs): return apply(_fluxspacecore.FluxletHelper_AddRow,args, kwargs)
    def AddCommand(*args, **kwargs): return apply(_fluxspacecore.FluxletHelper_AddCommand,args, kwargs)
    def RemoveCommand(*args, **kwargs): return apply(_fluxspacecore.FluxletHelper_RemoveCommand,args, kwargs)
    def DrawTextLeft(*args, **kwargs): return apply(_fluxspacecore.FluxletHelper_DrawTextLeft,args, kwargs)
    def DrawTextRight(*args, **kwargs): return apply(_fluxspacecore.FluxletHelper_DrawTextRight,args, kwargs)
    def DrawTextCenter(*args, **kwargs): return apply(_fluxspacecore.FluxletHelper_DrawTextCenter,args, kwargs)
    def ClearRow(*args, **kwargs): return apply(_fluxspacecore.FluxletHelper_ClearRow,args, kwargs)
    def Trace(*args, **kwargs): return apply(_fluxspacecore.FluxletHelper_Trace,args, kwargs)
    def Message(*args, **kwargs): return apply(_fluxspacecore.FluxletHelper_Message,args, kwargs)
    __swig_setmethods__["verbose"] = _fluxspacecore.FluxletHelper_verbose_set
    __swig_getmethods__["verbose"] = _fluxspacecore.FluxletHelper_verbose_get
    if _newclass:verbose = property(_fluxspacecore.FluxletHelper_verbose_get,_fluxspacecore.FluxletHelper_verbose_set)
    __swig_setmethods__["name"] = _fluxspacecore.FluxletHelper_name_set
    __swig_getmethods__["name"] = _fluxspacecore.FluxletHelper_name_get
    if _newclass:name = property(_fluxspacecore.FluxletHelper_name_get,_fluxspacecore.FluxletHelper_name_set)
    def __repr__(self):
        return "<C FluxletHelper instance at %s>" % (self.this,)

class FluxletHelperPtr(FluxletHelper):
    def __init__(self,this):
        _swig_setattr(self, FluxletHelper, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, FluxletHelper, 'thisown', 0)
        _swig_setattr(self, FluxletHelper,self.__class__,FluxletHelper)
_fluxspacecore.FluxletHelper_swigregister(FluxletHelperPtr)
cvar = _fluxspacecore.cvar
NOFONT = cvar.NOFONT


