#ifndef FBXFILESDK_KFBXMODULES_KFBXPLUGINCONTAINER_H
#define FBXFILESDK_KFBXMODULES_KFBXPLUGINCONTAINER_H

/**************************************************************************************

 Copyright � 2001 - 2006 Autodesk, Inc. and/or its licensors.
 All Rights Reserved.

 The coded instructions, statements, computer programs, and/or related material
 (collectively the "Data") in these files contain unpublished information
 proprietary to Autodesk, Inc. and/or its licensors, which is protected by
 Canada and United States of America federal copyright law and by international
 treaties.

 The Data may not be disclosed or distributed to third parties, in whole or in
 part, without the prior written consent of Autodesk, Inc. ("Autodesk").

 THE DATA IS PROVIDED "AS IS" AND WITHOUT WARRANTY.
 ALL WARRANTIES ARE EXPRESSLY EXCLUDED AND DISCLAIMED. AUTODESK MAKES NO
 WARRANTY OF ANY KIND WITH RESPECT TO THE DATA, EXPRESS, IMPLIED OR ARISING
 BY CUSTOM OR TRADE USAGE, AND DISCLAIMS ANY IMPLIED WARRANTIES OF TITLE,
 NON-INFRINGEMENT, MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE OR USE.
 WITHOUT LIMITING THE FOREGOING, AUTODESK DOES NOT WARRANT THAT THE OPERATION
 OF THE DATA WILL BE UNINTERRUPTED OR ERROR FREE.

 IN NO EVENT SHALL AUTODESK, ITS AFFILIATES, PARENT COMPANIES, LICENSORS
 OR SUPPLIERS ("AUTODESK GROUP") BE LIABLE FOR ANY LOSSES, DAMAGES OR EXPENSES
 OF ANY KIND (INCLUDING WITHOUT LIMITATION PUNITIVE OR MULTIPLE DAMAGES OR OTHER
 SPECIAL, DIRECT, INDIRECT, EXEMPLARY, INCIDENTAL, LOSS OF PROFITS, REVENUE
 OR DATA, COST OF COVER OR CONSEQUENTIAL LOSSES OR DAMAGES OF ANY KIND),
 HOWEVER CAUSED, AND REGARDLESS OF THE THEORY OF LIABILITY, WHETHER DERIVED
 FROM CONTRACT, TORT (INCLUDING, BUT NOT LIMITED TO, NEGLIGENCE), OR OTHERWISE,
 ARISING OUT OF OR RELATING TO THE DATA OR ITS USE OR ANY OTHER PERFORMANCE,
 WHETHER OR NOT AUTODESK HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH LOSS
 OR DAMAGE.

**************************************************************************************/
#include <fbxfilesdk/components/kbaselib/kaydaradef_h.h>
// Local includes 
#include <fbxfilesdk/kfbxmodules/kfbxplugin.h>

// FBX includes
#include <fbxfilesdk/kfbxevents/kfbxemitter.h>

// FBX begin namespace
#include <fbxfilesdk/fbxfilesdk_nsbegin.h>

/////////////////////////////////////////////////////////////////////////////////////////
// TODO: todonamespace
//       Every FBX module should have a namespace but for consistency, 
//       you have, for this version of fbx, to use the fbx object without namespace
//namespace kfbxmodules
//{
    /** \class KFbxPluginContainer
    *
    * \brief Used to register plug-ins.
    * 
    */
    class KFBX_DLL KFbxPluginContainer : public kfbxevents::KFbxEmitter
    {
    public:
        typedef KIntrusiveList<KFbxPlugin> PluginList;

        /**
        *\name Public interface
        */
        //@{
        /** Plug-ins has the responsibility to register them selves to their loading strategy. It is done by executing a registration via this method
        * \param pPlugin  Reference to plug-in to be registered
        */
        void Register(KFbxPlugin& pPlugin);

        /** Unregister a previously registered plugin.
        * \param pPlugin  Reference to plug-in to be unregistered
        */
        void Unregister(KFbxPlugin& pPlugin);

        /** Constant accessor to the list of plug-ins owned by the strategy
        * \return List of plug-ins
        */
        inline const PluginList& GetPlugins()const;

        /** Non-Constant accessor to the list of plug-ins owned by the strategy
        * \return List of plug-ins
        */
        inline PluginList& GetPlugins();

        //@}

    protected:
        // Plug-in management
        PluginList mPlugins;

        virtual ~KFbxPluginContainer();
    };

    //////////////////////////////////////////////////////////////////////////////////////
    inline const KFbxPluginContainer::PluginList& KFbxPluginContainer::GetPlugins()const
    {
        return mPlugins;
    }
    //////////////////////////////////////////////////////////////////////////////////////
    inline KFbxPluginContainer::PluginList& KFbxPluginContainer::GetPlugins()
    {
        return mPlugins;
    }
//}

// FBX end namespace
#include <fbxfilesdk/fbxfilesdk_nsend.h>

#endif // FBXFILESDK_KFBXMODULES_KFBXPLUGINCONTAINER_H


