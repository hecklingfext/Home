/*!  \file kfbxnodeattribute.h
 */

#ifndef FBXFILESDK_KFBXPLUGINS_KFBXNODEATTRIBUTE_H
#define FBXFILESDK_KFBXPLUGINS_KFBXNODEATTRIBUTE_H

/**************************************************************************************

 Copyright ?2001 - 2008 Autodesk, Inc. and/or its licensors.
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
#include <fbxfilesdk/components/kbaselib/kaydara.h>
#include <fbxfilesdk/components/kbaselib/klib/karrayul.h>
#include <fbxfilesdk/components/kbaselib/klib/kstringlist.h>

#include <fbxfilesdk/kfbxplugins/kfbxtakenodecontainer.h>

#include <fbxfilesdk/fbxfilesdk_nsbegin.h>

class KFbxNode;
class KFbxSdkManager;

/**	\brief This class is the base class to all types of node attributes.
  * \nosubgrouping
  *	A node attribute is the content of a node. A \c NULL node attribute is set 
  * by calling function KFbxNode::SetNodeAttribute() with a \c NULL pointer.
  */
class KFBX_DLL KFbxNodeAttribute : public KFbxTakeNodeContainer
{
	KFBXOBJECT_DECLARE(KFbxNodeAttribute,KFbxTakeNodeContainer);

public:
	/**
	  * Property Names
	  */
	static const char*			sColor;

	/**
	  * Property Default Values
	  */
	static const fbxDouble3		sDefaultColor;

	//////////////////////////////////////////////////////////////////////////
	//
	// Properties
	//
	//////////////////////////////////////////////////////////////////////////
	
	/** This property handles the color.
	  *
	  * Default value is (0.8, 0.8, 0.8)
	  */
	KFbxTypedProperty<fbxDouble3>		Color;



	/** \enum EAttributeType Node attribute types.
	  * - \e eUNIDENTIFIED
	  * - \e eNULL
	  * - \e eMARKER
	  * - \e eSKELETON
	  * - \e eMESH
	  * - \e eNURB
	  * - \e ePATCH
	  * - \e eCAMERA
	  * - \e eCAMERA_SWITCHER
	  * - \e eLIGHT
            * - \e eOPTICAL_REFERENCE
	  * - \e eOPTICAL_MARKER
	  * - \e eNURBS_CURVE
	  * - \e eTRIM_NURBS_SURFACE
	  * - \e eBOUNDARY
            * - \e eNURBS_SURFACE
	  * - \e eSHAPE
	  * - \e eLODGROUP
	  */
    typedef enum 
    {   
		eUNIDENTIFIED,
		eNULL,
		eMARKER,
	    eSKELETON, 
	    eMESH, 
	    eNURB, 
	    ePATCH, 
	    eCAMERA, 
		eCAMERA_SWITCHER,
	    eLIGHT,
		eOPTICAL_REFERENCE,
		eOPTICAL_MARKER,
		eNURBS_CURVE,
		eTRIM_NURBS_SURFACE,
		eBOUNDARY,
		eNURBS_SURFACE,
        eSHAPE,
		eLODGROUP
    } EAttributeType;

	/** Return the type of node attribute.
	  * This class is pure virtual.
	  */
	virtual EAttributeType GetAttributeType() const { return eUNIDENTIFIED; }

    /** Return the node this attribute is set to.
	  * \return     Pointer to the node, or \c NULL if the current attribute is not set to a node.
	  */
	KFbxNode* GetNode() const;

	/**
	  * \name Properties
	  */
	//@{


	/** Set color.
	  * \param pColor     The color used to draw the node attribute.
	  * \remarks          This property can be used by the node attribute for its wireframe representation
	  *                   or to define its color if there are no materials connected. Only
	  *                   the node attribute of type eMARKER use this property. This value persists in the FBX file
	  *                   so third party plug-ins may use it for their specific needs.
	  *
	  * \remarks Deprecated. Use the Color property.
	  *
	  */
	K_DEPRECATED void SetColor(KFbxColor const& pColor);

	/** Get marker color.
	  * \return The node attribute color property. The default value, if no call to SetColor has
	  * been made, is RGB(0.8, 0.8, 0.8).
	  * \remark This property can be used by the node attribute for its wireframe representation
	  * or to define its color in the case that no materials are connected to it. For now, only
	  * the node attribute of type eMARKER use this property. This value is persisten in the FBX file
	  * so third party plug-ins can use it for their specific needs.
	  *
	  * \remarks Deprecated. Use the Color property.
	  *
	  */
	K_DEPRECATED KFbxColor GetColor() const;

	//@}

    virtual KFbxObject* Clone(KFbxObject* pContainer, KFbxObject::ECloneType pCloneType) const;

///////////////////////////////////////////////////////////////////////////////
//
//  WARNING!
//
//	Anything beyond these lines may not be documented accurately and is 
// 	subject to change without notice.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef DOXYGEN_SHOULD_SKIP_THIS

public:

protected:
	
	KFbxNodeAttribute(KFbxSdkManager& pManager, char const* pName);
	virtual ~KFbxNodeAttribute();

	virtual bool					ConstructProperties(bool pForceSet);

	virtual KFbxTakeNodeContainer*	GetTakeNodeContainer();

	//! Once it's set, the node should not change.
	virtual void SetNode(KFbxNode& pNode);
	virtual void UnsetNode();

	virtual	bool ConnecNotify (KFbxConnectEvent const &pEvent);

	/**
	  *	Used to retrive the KProperty list from an attribute.
	  */
	virtual KString		GetTypeName() const;
	virtual KStringList	GetTypeFlags() const;

	//! Assignment operator.
    KFbxNodeAttribute& operator=(KFbxNodeAttribute const& pAttribute);

	KFbxNode*	mParentNode;

	friend class KFbxNode;
	friend class KFbxScene;
    
#endif // #ifndef DOXYGEN_SHOULD_SKIP_THIS

};

typedef KFbxNodeAttribute* HKFbxNodeAttribute;

#include <fbxfilesdk/fbxfilesdk_nsend.h>

#endif // FBXFILESDK_KFBXPLUGINS_KFBXNODEATTRIBUTE_H


