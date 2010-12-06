//  PythonInterface.cc for fluxspace.
//
//  Copyright (c) 2002 Steve Cooper, stevencooper@isomedia.com
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//
// (See the included file COPYING / GPL-2.0)
//

#include "PythonInterface.hh"
#include <stdio.h>
#include <string>

size_t PythonInterface::moduleCount = 0;

class PythonInterfaceObject
{
  public:
    PythonInterfaceObject(PyObject* object_, int tickSeconds_, int row_ = -1)
      : next(NULL),
        object(object_),
        methInitialize(NULL),
        methWindowCreate(NULL),
        methWindowDestroy(NULL),
        methWindowAdd(NULL),
        methWindowRemove(NULL),
        methWindowChangeAttributes(NULL),
        methWindowFocus(NULL),
        methWindowRaise(NULL),
        methWindowLower(NULL),
        methWindowConfigure(NULL),
        methWindowChangeParent(NULL),
        methWindowMap(NULL),
        methWindowUnmap(NULL),
        methWorkspaceChangeCount(NULL),
        methWorkspaceIn(NULL),
        methWorkspaceOut(NULL),
        methCommand(NULL),
        methTick(NULL),
        tickSeconds(tickSeconds_),
        tickCount(0),
        row(row_)
    {
        // A valid row # indicates it's a command handler
        if (row >= 0)
        {
            methCommand = new MyPyMethod(object, "Command", true);
        }
        else
        {
            methInitialize             = new MyPyMethod(object, "Initialize",             true);
            methWindowCreate           = new MyPyMethod(object, "WindowCreate",           true);
            methWindowDestroy          = new MyPyMethod(object, "WindowDestroy",          true);
            methWindowAdd              = new MyPyMethod(object, "WindowAdd",              true);
            methWindowRemove           = new MyPyMethod(object, "WindowRemove",           true);
            methWindowChangeAttributes = new MyPyMethod(object, "WindowChangeAttributes", true);
            methWindowFocus            = new MyPyMethod(object, "WindowFocus",            true);
            methWindowRaise            = new MyPyMethod(object, "WindowRaise",            true);
            methWindowLower            = new MyPyMethod(object, "WindowLower",            true);
            methWindowConfigure        = new MyPyMethod(object, "WindowConfigure",        true);
            methWindowChangeParent     = new MyPyMethod(object, "WindowChangeParent",     true);
            methWindowMap              = new MyPyMethod(object, "WindowMap",              true);
            methWindowUnmap            = new MyPyMethod(object, "WindowUnmap",            true);
            methWorkspaceChangeCount   = new MyPyMethod(object, "WorkspaceChangeCount",   true);
            methWorkspaceIn            = new MyPyMethod(object, "WorkspaceIn",            true);
            methWorkspaceOut           = new MyPyMethod(object, "WorkspaceOut",           true);
            methTick                   = new MyPyMethod(object, "Tick",                   true);
        }
    }

    virtual ~PythonInterfaceObject()
    {
        delete methInitialize;
        delete methWindowCreate;
        delete methWindowDestroy;
        delete methWindowAdd;
        delete methWindowRemove;
        delete methWindowChangeAttributes;
        delete methWindowFocus;
        delete methWindowRaise;
        delete methWindowLower;
        delete methWindowConfigure;
        delete methWindowChangeParent;
        delete methWindowMap;
        delete methWindowUnmap;
        delete methWorkspaceChangeCount;
        delete methWorkspaceIn;
        delete methWorkspaceOut;
        delete methCommand;
        delete methTick;
    }

    PythonInterfaceObject* next;
    static PythonInterfaceObject* head;

    static void Append(PythonInterfaceObject* newObject)
    {
        if (head == NULL)
        {
            head = newObject;
        }
        else
        {
            PythonInterfaceObject* object = head;
            while (true)
            {
                if (object->next == NULL)
                {
                    object->next = newObject;
                    break;
                }
                object = object->next;
            }
        }
    }

    static void PurgeAll()
    {
        PythonInterfaceObject* object = head;
        while (object != NULL)
        {
            PythonInterfaceObject* next = object->next;
            delete object;
            object = next;
        }
    }

    void Initialize()
    {
        if (methInitialize != NULL)
            methInitialize->Call();
    }

    void WindowCreate(Window window, Window parent)
    {
        if (methWindowCreate != NULL)
        {
            methWindowCreate->BeginArgs(2);
            methWindowCreate->AddArgInt(window);
            methWindowCreate->AddArgInt(parent);
            methWindowCreate->Call();
        }
    }

    void WindowDestroy(Window window)
    {
        if (methWindowDestroy != NULL)
        {
            methWindowDestroy->BeginArgs(1);
            methWindowDestroy->AddArgInt(window);
            methWindowDestroy->Call();
        }
    }

    void WindowAdd(Window window, int workspace)
    {
        if (methWindowAdd != NULL)
        {
            methWindowAdd->BeginArgs(2);
            methWindowAdd->AddArgInt(window);
            methWindowAdd->AddArgInt(workspace);
            methWindowAdd->Call();
        }
    }

    void WindowRemove(Window window)
    {
        if (methWindowRemove != NULL)
        {
            methWindowRemove->BeginArgs(1);
            methWindowRemove->AddArgInt(window);
            methWindowRemove->Call();
        }
    }

    void WindowChangeAttributes(Window window)
    {
        if (methWindowChangeAttributes != NULL)
        {
            methWindowChangeAttributes->BeginArgs(1);
            methWindowChangeAttributes->AddArgInt(window);
            methWindowChangeAttributes->Call();
        }
    }

    void WindowFocus(Window window)
    {
        if (methWindowFocus != NULL)
        {
            methWindowFocus->BeginArgs(1);
            methWindowFocus->AddArgInt(window);
            methWindowFocus->Call();
        }
    }

    void WindowRaise(Window window)
    {
        if (methWindowRaise != NULL)
        {
            methWindowRaise->BeginArgs(1);
            methWindowRaise->AddArgInt(window);
            methWindowRaise->Call();
        }
    }

    void WindowLower(Window window)
    {
        if (methWindowLower != NULL)
        {
            methWindowLower->BeginArgs(1);
            methWindowLower->AddArgInt(window);
            methWindowLower->Call();
        }
    }

    void WindowConfigure(Window window)
    {
        if (methWindowConfigure != NULL)
        {
            methWindowConfigure->BeginArgs(1);
            methWindowConfigure->AddArgInt(window);
            methWindowConfigure->Call();
        }
    }

    void WindowChangeParent(Window window, Window parent)
    {
        if (methWindowChangeParent != NULL)
        {
            methWindowChangeParent->BeginArgs(2);
            methWindowChangeParent->AddArgInt(window);
            methWindowChangeParent->AddArgInt(parent);
            methWindowChangeParent->Call();
        }
    }

    void WindowMap(Window window)
    {
        if (methWindowMap != NULL)
        {
            methWindowMap->BeginArgs(1);
            methWindowMap->AddArgInt(window);
            methWindowMap->Call();
        }
    }

    void WindowUnmap(Window window)
    {
        if (methWindowUnmap != NULL)
        {
            methWindowUnmap->BeginArgs(1);
            methWindowUnmap->AddArgInt(window);
            methWindowUnmap->Call();
        }
    }

    void WorkspaceChangeCount(int nWorkspaces)
    {
        if (methWorkspaceChangeCount != NULL)
        {
            methWorkspaceChangeCount->BeginArgs(1);
            methWorkspaceChangeCount->AddArgInt(nWorkspaces);
            methWorkspaceChangeCount->Call();
        }
    }

    void WorkspaceIn(int workspace)
    {
        if (methWorkspaceIn != NULL)
        {
            methWorkspaceIn->BeginArgs(1);
            methWorkspaceIn->AddArgInt(workspace);
            methWorkspaceIn->Call();
        }
    }

    void WorkspaceOut(int workspace)
    {
        if (methWorkspaceOut != NULL)
        {
            methWorkspaceOut->BeginArgs(1);
            methWorkspaceOut->AddArgInt(workspace);
            methWorkspaceOut->Call();
        }
    }

    void Command(int row_, const string& command, int button, const string& data)
    {
        if (methCommand != NULL && row == row_)
        {
            methCommand->BeginArgs(3);
            methCommand->AddArgString(command);
            methCommand->AddArgInt(button);
            methCommand->AddArgString(data);
            methCommand->Call();
        }
    }

    void Tick()
    {
        if (methTick != NULL)
        {
            if (tickSeconds > 0)
            {
                if (++tickCount >= tickSeconds)
                {
                    methTick->BeginArgs(0);
                    methTick->Call();
                    tickCount = 0;
                }
            }
        }
    }

  private:
    MyPyObjectRef object;   // Use "Ref" to increment reference count
    MyPyMethod* methInitialize;
    MyPyMethod* methWindowCreate;
    MyPyMethod* methWindowDestroy;
    MyPyMethod* methWindowAdd;
    MyPyMethod* methWindowRemove;
    MyPyMethod* methWindowChangeAttributes;
    MyPyMethod* methWindowFocus;
    MyPyMethod* methWindowRaise;
    MyPyMethod* methWindowLower;
    MyPyMethod* methWindowConfigure;
    MyPyMethod* methWindowChangeParent;
    MyPyMethod* methWindowMap;
    MyPyMethod* methWindowUnmap;
    MyPyMethod* methWorkspaceChangeCount;
    MyPyMethod* methWorkspaceIn;
    MyPyMethod* methWorkspaceOut;
    MyPyMethod* methCommand;
    MyPyMethod* methTick;
    int tickSeconds;
    int tickCount;
    int row;
};

PythonInterfaceObject* PythonInterfaceObject::head = NULL;

class PythonInterfaceModule
{
  public:
    PythonInterfaceModule::PythonInterfaceModule(const string& name_, PyObject* config_)
      : name(name_),
        next(NULL),
        module(name_),
        config(config_),
        FSInitialize(NULL)
    {
        FSInitialize = new MyPyFunctionPtr(module.dict.GetItemString("fluxlet_main"));
    }

    PythonInterfaceModule::~PythonInterfaceModule()
    {
        delete FSInitialize;
    }

    void Initialize()
    {
        FSInitialize->BeginArgs(1);
        FSInitialize->AddArgObject(config);
        FSInitialize->Call();
    }

    static void Append(PythonInterfaceModule* newModule)
    {
        if (head == NULL)
        {
            head = newModule;
        }
        else
        {
            PythonInterfaceModule* module = head;
            while (true)
            {
                if (module->next == NULL)
                {
                    module->next = newModule;
                    break;
                }
                module = module->next;
            }
        }
    }

    static void PurgeAll()
    {
        PythonInterfaceModule* module = head;
        while (module != NULL)
        {
            PythonInterfaceModule* next = module->next;
            delete module;
            module = next;
        }
    }

    string name;
    PythonInterfaceModule* next;
    static PythonInterfaceModule* head;

  private:
    MyPyModule module;
    MyPyObject config;
    MyPyFunctionPtr* FSInitialize;
};

PythonInterfaceModule* PythonInterfaceModule::head = NULL;

PythonInterface::PythonInterface()
  : methRegisterSOAPObject(NULL),
    methSOAPPoll(NULL)
{
}

PythonInterface::~PythonInterface()
{
    delete methRegisterSOAPObject;
    delete methSOAPPoll;
    PythonInterfaceModule::PurgeAll();
    PythonInterfaceObject::PurgeAll();
}

void PythonInterface::AddModule(const string& name, PyObject* config)
{
    PythonInterfaceModule::Append(new PythonInterfaceModule(name, config));
    moduleCount++;
}

void PythonInterface::AddHandler(PyObject* object, int tickSeconds)
{
    PythonInterfaceObject::Append(new PythonInterfaceObject(object, tickSeconds));
}

void PythonInterface::AddCommandHandler(PyObject* object, int nRow)
{
    PythonInterfaceObject::Append(new PythonInterfaceObject(object, 0, nRow));
}

void PythonInterface::SetSOAPHandler(PyObject* object)
{
    soapServer = object;
    delete methRegisterSOAPObject;
    delete methSOAPPoll;
    methRegisterSOAPObject = new MyPyMethod(soapServer, "RegisterSOAPObject", true);
    methSOAPPoll       = new MyPyMethod(soapServer, "SOAPPoll",       true);
}

void PythonInterface::RegisterSOAPObject(PyObject* object, const string& nameSpace)
{
    if (methRegisterSOAPObject != NULL)
    {
        methRegisterSOAPObject->BeginArgs(2);
        methRegisterSOAPObject->AddArgObject(object);
        methRegisterSOAPObject->AddArgString(nameSpace);
        methRegisterSOAPObject->Call();
    }
}

void PythonInterface::SOAPPoll()
{
    if (methSOAPPoll != NULL)
    {
        methSOAPPoll->BeginArgs(0);
        methSOAPPoll->Call();
    }
}

void PythonInterface::Initialize()
{
    // Initialize modules
    for (PythonInterfaceModule* module = PythonInterfaceModule::head; module != NULL; module = module->next)
        module->Initialize();

    // Initialize objects added by module initializations
    for (PythonInterfaceObject* object = PythonInterfaceObject::head; object != NULL; object = object->next)
    {
        object->Initialize();
    }
}

void PythonInterface::WindowCreate(Window window, Window parent)
{
    for (PythonInterfaceObject* object = PythonInterfaceObject::head;
         object != NULL;
         object = object->next)
    {
        try
        {
            object->WindowCreate(window, parent);
        }
        catch(MyPyException e)
        {
            printf("WindowCreate failed: \"%s\"\n", e.text.c_str());
        }
    }
}

void PythonInterface::WindowDestroy(Window window)
{
    for (PythonInterfaceObject* object = PythonInterfaceObject::head;
         object != NULL;
         object = object->next)
    {
        try
        {
            object->WindowDestroy(window);
        }
        catch(MyPyException e)
        {
            printf("WindowDestroy failed: \"%s\"\n", e.text.c_str());
        }
    }
}

void PythonInterface::WindowAdd(Window window, int workspace)
{
    for (PythonInterfaceObject* object = PythonInterfaceObject::head;
         object != NULL;
         object = object->next)
    {
        try
        {
            object->WindowAdd(window, workspace);
        }
        catch(MyPyException e)
        {
            printf("WindowAdd failed: \"%s\"\n", e.text.c_str());
        }
    }
}

void PythonInterface::WindowRemove(Window window)
{
    for (PythonInterfaceObject* object = PythonInterfaceObject::head;
         object != NULL;
         object = object->next)
    {
        try
        {
            object->WindowRemove(window);
        }
        catch(MyPyException e)
        {
            printf("WindowRemove failed: \"%s\"\n", e.text.c_str());
        }
    }
}

void PythonInterface::WindowChangeAttributes(Window window)
{
    for (PythonInterfaceObject* object = PythonInterfaceObject::head;
         object != NULL;
         object = object->next)
    {
        try
        {
            object->WindowChangeAttributes(window);
        }
        catch(MyPyException e)
        {
            printf("WindowChangeAttributes failed: \"%s\"\n", e.text.c_str());
        }
    }
}

void PythonInterface::WindowFocus(Window window)
{
    for (PythonInterfaceObject* object = PythonInterfaceObject::head;
         object != NULL;
         object = object->next)
    {
        try
        {
            object->WindowFocus(window);
        }
        catch(MyPyException e)
        {
            printf("WindowFocus failed: \"%s\"\n", e.text.c_str());
        }
    }
}

void PythonInterface::WindowRaise(Window window)
{
    for (PythonInterfaceObject* object = PythonInterfaceObject::head;
         object != NULL;
         object = object->next)
    {
        try
        {
            object->WindowRaise(window);
        }
        catch(MyPyException e)
        {
            printf("WindowRaise failed: \"%s\"\n", e.text.c_str());
        }
    }
}

void PythonInterface::WindowLower(Window window)
{
    for (PythonInterfaceObject* object = PythonInterfaceObject::head;
         object != NULL;
         object = object->next)
    {
        try
        {
            object->WindowLower(window);
        }
        catch(MyPyException e)
        {
            printf("WindowLower failed: \"%s\"\n", e.text.c_str());
        }
    }
}

void PythonInterface::WindowConfigure(Window window)
{
    for (PythonInterfaceObject* object = PythonInterfaceObject::head;
         object != NULL;
         object = object->next)
    {
        try
        {
            object->WindowConfigure(window);
        }
        catch(MyPyException e)
        {
            printf("WindowConfigure failed: \"%s\"\n", e.text.c_str());
        }
    }
}

void PythonInterface::WindowChangeParent(Window window, Window parent)
{
    for (PythonInterfaceObject* object = PythonInterfaceObject::head;
         object != NULL;
         object = object->next)
    {
        try
        {
            object->WindowChangeParent(window, parent);
        }
        catch(MyPyException e)
        {
            printf("WindowChangeParent failed: \"%s\"\n", e.text.c_str());
        }
    }
}

void PythonInterface::WindowMap(Window window)
{
    for (PythonInterfaceObject* object = PythonInterfaceObject::head;
         object != NULL;
         object = object->next)
    {
        try
        {
            object->WindowMap(window);
        }
        catch(MyPyException e)
        {
            printf("WindowMap failed: \"%s\"\n", e.text.c_str());
        }
    }
}

void PythonInterface::WindowUnmap(Window window)
{
    for (PythonInterfaceObject* object = PythonInterfaceObject::head;
         object != NULL;
         object = object->next)
    {
        try
        {
            object->WindowUnmap(window);
        }
        catch(MyPyException e)
        {
            printf("WindowUnmap failed: \"%s\"\n", e.text.c_str());
        }
    }
}

void PythonInterface::WorkspaceChangeCount(int nWorkspaces)
{
    for (PythonInterfaceObject* object = PythonInterfaceObject::head;
         object != NULL;
         object = object->next)
    {
        try
        {
            object->WorkspaceChangeCount(nWorkspaces);
        }
        catch(MyPyException e)
        {
            printf("WorkspaceChangeCount failed: \"%s\"\n", e.text.c_str());
        }
    }
}


void PythonInterface::WorkspaceIn(int workspace)
{
    for (PythonInterfaceObject* object = PythonInterfaceObject::head;
         object != NULL;
         object = object->next)
    {
        try
        {
            object->WorkspaceIn(workspace);
        }
        catch(MyPyException e)
        {
            printf("WorkspaceIn failed: \"%s\"\n", e.text.c_str());
        }
    }
}

void PythonInterface::WorkspaceOut(int workspace)
{
    for (PythonInterfaceObject* object = PythonInterfaceObject::head;
         object != NULL;
         object = object->next)
    {
        try
        {
            object->WorkspaceOut(workspace);
        }
        catch(MyPyException e)
        {
            printf("WorkspaceOut failed: \"%s\"\n", e.text.c_str());
        }
    }
}

void PythonInterface::Command(int row, const string& command, int button, const string& data)
{
    // Note that the object will filter on the row # when invoked.
    // So only one object will actually invoke the handler.
    for (PythonInterfaceObject* object = PythonInterfaceObject::head;
         object != NULL;
         object = object->next)
    {
        try
        {
            object->Command(row, command, button, data);
        }
        catch(MyPyException e)
        {
            printf("Command \"%s\" for row %d failed: \"%s\"\n", command.c_str(), row, e.text.c_str());
        }
    }
}

void PythonInterface::Tick()
{
    for (PythonInterfaceObject* object = PythonInterfaceObject::head;
         object != NULL;
         object = object->next)
    {
        try
        {
            object->Tick();
        }
        catch(MyPyException e)
        {
            printf("WorkspaceOut failed: \"%s\"\n", e.text.c_str());
        }
    }

    // Make sure the soap calls keep flowing
    SOAPPoll();
}

size_t PythonInterface::GetModuleCount()
{
    return moduleCount;
}
