#ifndef FBXFILESDK_FBXPROCESSORS_KFBXPROCESSOR_H
#define FBXFILESDK_FBXPROCESSORS_KFBXPROCESSOR_H

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

#include <fbxfilesdk/fbxprocessors/fbxprocessors_def.h>

#include <fbxfilesdk/fbxcore/fbxcollection/kfbxcollection.h>

namespace FBXPROCESSORS_NAMESPACE {

	/** This class contains objects
	  * \nosubgrouping
	  * ??????????
	  * \par
	  * This class also provides access to global settings and take information.
	  *
	  */
	class KFBX_DLL KFbxProcessor : public KFbxObject
	{
		KFBXOBJECT_DECLARE(KFbxProcessor,KFbxObject);

		/**
		* \name Processor management
		*/
		//@{
		public:
			bool					ProcessCollection(KFbxCollection *pCollection=0);
			bool					ProcessObject	 (KFbxObject *pCollection=0);
		//@}

		/**
		* \name Overrrideable internal function	*/
		//@{
		protected:
			virtual bool				internal_ProcessCollectionBegin (KFbxCollection *pCollection);
			virtual bool				internal_ProcessCollectionEnd 	(KFbxCollection *pCollection);
			virtual bool				internal_ProcessObject  		(KFbxObject*	 pObject);
			virtual bool				internal_ProcessCollection		(KFbxCollection* pCollection);
		//@}

		///////////////////////////////////////////////////////////////////////////////
		//  WARNING!
		//	Anything beyond these lines may not be Collectioned accurately and is 
		// 	subject to change without notice.
		///////////////////////////////////////////////////////////////////////////////
		#ifndef DOXYGEN_SHOULD_SKIP_THIS
			// Constructor / Destructor
			protected:
				virtual KFbxObject* Clone(KFbxObject* pContainer, KFbxObject::ECloneType pCloneType) const;
				KFbxProcessor(KFbxSdkManager& pManager, char const* pName);
				~KFbxProcessor();
				virtual void	Construct	(const KFbxProcessor* pFrom);
				virtual void	Destruct	(bool pRecursive, bool pDependents);
				bool			ConstructProperties(bool pForceSet);
		#endif // #ifndef DOXYGEN_SHOULD_SKIP_THIS 

	};

}

#endif // FBXFILESDK_FBXPROCESSORS_KFBXPROCESSOR_H


