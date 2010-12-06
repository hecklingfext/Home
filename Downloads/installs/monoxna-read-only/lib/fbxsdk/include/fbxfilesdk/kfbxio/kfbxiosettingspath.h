/**************************************************************************************

 Copyright � 2001 - 2008 Autodesk, Inc. and/or its licensors.
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

// these define are used for file preset

#define KS_BS                           KString("\\")

#define KS_IMPORT                       KString("\\import")
#define KS_EXPORT                       KString("\\export")
#define KS_FBXPRESETS                   KString("\\fbxpresets")

#define KS_3DSMAX                       KString("\\3dsmax")
#define KS_MAYA                         KString("\\maya")

#define KS_VERSION                      KString(FBXPLUGINS_VERSION_STRING)

#define IMP_PRESET_EXT					"fbximportpreset"
#define EXP_PRESET_EXT					"fbxexportpreset"

#define IMP_M_E_FILENAME				"Autodesk Media & Entertainment."IMP_PRESET_EXT
#define EXP_M_E_FILENAME				"Autodesk Media & Entertainment."EXP_PRESET_EXT

#define IMP_ARCH_FILENAME               "Autodesk Architectural (Revit)."IMP_PRESET_EXT
#define EXP_ARCH_FILENAME               "Autodesk Architectural (Revit)."EXP_PRESET_EXT

#define IMP_USERDEFINED_FILENAME		"User defined."IMP_PRESET_EXT
#define EXP_USERDEFINED_FILENAME		"User defined."EXP_PRESET_EXT

#define IMP_DIR							KS_FBXPRESETS + KS_BS + KS_VERSION + KS_IMPORT
#define EXP_DIR							KS_FBXPRESETS + KS_BS + KS_VERSION + KS_EXPORT

#define PRESET_VERSION_DIR              KS_FBXPRESETS + KS_BS + KS_VERSION

#define VERSIONPATH_3DS                 KS_3DSMAX     + KS_FBXPRESETS + KS_BS + KS_VERSION

#define MYDOCIMPORTPATH_3DS             KS_3DSMAX     + KS_FBXPRESETS + KS_BS + KS_VERSION + KS_IMPORT
#define MYDOCEXPORTPATH_3DS             KS_3DSMAX     + KS_FBXPRESETS + KS_BS + KS_VERSION + KS_EXPORT

#define MYDOCIMPORTPATH_Maya_Win        KS_MAYA       + KS_FBXPRESETS + KS_BS + KS_VERSION + KS_IMPORT
#define MYDOCEXPORTPATH_Maya_Win        KS_MAYA       + KS_FBXPRESETS + KS_BS + KS_VERSION + KS_EXPORT

#define FN_LOCALIZATION_PREFIX          "localization_"
#define FN_LOCALIZATION_EXT             ".xlf"

#define FN_LANGUAGE                     "lang.dat"
#define LANGUAGE_PATH                   KS_FBXPRESETS + KS_BS + KS_VERSION + KS_BS + FN_LANGUAGE

// these defines are used for hierarchical propreties names
#define IOSROOT							"IOSRoot"

#define IOSN_EXPORT			    		"Export"
#define IOSN_IMPORT			    		"Import"

#define	IOSN_PLUGIN_GRP					"PlugInGrp"
#define IOSN_PLUGIN_VERSION				"PlugInVersion"
#define IOSN_BUILDNUMBER                "BuildNumber"
#define IOSN_PLUGIN_UI_WIDTH			"PlugInUIWidth"
#define IOSN_PLUGIN_UI_HEIGHT   		"PlugInUIHeight"
#define IOSN_PRESET_SELECTED            "PresetSelected"

#define IOSN_PRESETS_GRP        		"PresetsGrp"
#define IOSN_STATISTICS_GRP     		"StatisticsGrp"
#define IOSN_UNITS_GRP          		"UnitsGrp"
#define IOSN_INCLUDE_GRP        		"IncludeGrp"
#define IOSN_ADV_OPT_GRP        		"AdvOptGrp"
#define IOSN_AXISCONV_GRP       		"AxisConvGrp"
#define IOSN_BAKE               		"Bake"

#define IOSN_UP_AXIS					"UpAxis"
#define IOSN_UP_AXIS_MAX				"UpAxisMax"
#define IOSN_ZUPROTATION_MAX            "ZUProtation_max"
#define IOSN_AXISCONVERSION             "AxisConversion"
#define IOSN_AXIS_SELECTOR              "AxisSelector" 
#define IOSN_AXIS_CON_METHOD_SELECTOR   "AxisConMethodSelector"

#define IOSN_PRESETS	        		"Presets"
#define IOSN_STATISTICS	        		"Statistics"
#define IOSN_UNITS_SCALE           		"UnitsScale"
#define IOSN_TOTAL_UNITS_SCALE_TB  		"TotalUnitsScale"

#define IOSN_SCALECONVERSION            "ScaleConversion"
#define IOSN_MASTERSCALE                "MasterScale"
#define IOSN_MASTERUNITSTR              "MasterUnitStr" // must be removed DT
#define IOSN_UNITSELECTOR				"UnitsSelector"

#define IOSN_ANIMATION          		"Animation"
#define IOSN_GEOMETRY           		"Geometry"
#define IOSN_DEFORMATION				"Deformation"
#define IOSN_MARKERS					"Markers"
#define IOSN_CHARACTER					"Character"
#define IOSN_SETLOCKEDATTRIB			"LockedAttribute"

#define IOSN_MRCUSTOMATTRIBUTES 		"MRCustomAttributes"
#define IOSN_MESHPRIMITIVE      		"MeshPrimitive"
#define IOSN_MESHTRIANGLE       		"MeshTriangle"
#define IOSN_MESHPOLY           		"MeshPoly"
#define IOSN_NURB               		"Nurb"
#define IOSN_PATCH              		"Patch"
#define IOSN_BIP2FBX            		"Bip2Fbx"
#define IOSN_ASCIIFBX                   "AsciiFbx"

#define IOSN_TAKE						"Take"

#define IOSN_GEOMETRYMESHPRIMITIVEAS    "GeometryMeshPrimitiveAs"
#define IOSN_GEOMETRYMESHTRIANGLEAS     "GeometryMeshTriangleAs"
#define IOSN_GEOMETRYMESHPOLYAS         "GeometryMeshPolyAs"
#define IOSN_GEOMETRYNURBSAS            "GeometryNurbsAs"

#define IOSN_GEOMETRYNURBSSURFACEAS     "GeometryNurbsSurfaceAs"
#define IOSN_GEOMETRYPATCHAS            "GeometryPatchAs"

#define IOSN_FORCENORMALSBYCONTROLPOINT "ForceNormalsByControlPoint"
#define IOSN_ANIMATIONONLY              "AnimationOnly"
#define IOSN_SELECTIONONLY				"SelectionOnly"

#define IOSN_BONE						"Bone"
#define IOSN_BONEWIDTHHEIGHTLOCK 		"BoneWidthHeightLock"
#define IOSN_BONEASDUMMY         		"BoneAsDummy"
#define IOSN_BONEMAX4BONEWIDTH   		"Max4BoneWidth"
#define IOSN_BONEMAX4BONEHEIGHT  		"Max4BoneHeight"
#define IOSN_BONEMAX4BONETAPER   		"Max4BoneTaper"

#define IOSN_CURVE_FILTER	    		"CurveFilter"
#define IOSN_CONSTRAINT		    		"Constraint"
#define IOSN_UI				    		"UI"
#define IOSN_SHOW_UI_MODE               "ShowUIMode"
#define IOSN_SHOW_WARNINGS_MANAGER      "ShowWarningsManager"

#define IOSN_MERGE_MODE	    		    "MergeMode"
#define IOSN_SAMPLINGPANEL              "SamplingPanel"

#define IOSN_FILE_FORMAT	    		"FileFormat"
#define IOSN_FBX                		"Fbx"
#define IOSN_DXF                		"Dxf"
#define IOSN_OBJ                		"Obj"
#define IOSN_3DS                		"Max_3ds"  // can't start by a number for xml node name
#define IOSN_COLLADA            		"Collada"

#define IOSN_MOTION_BASE				"Motion_Base"  // for commond Motion Readers/Writers stream options

#define IOSN_BIOVISION_BVH       		"Biovision_BVH"
#define IOSN_MOTIONANALYSIS_HTR  		"MotionAnalysis_HTR"
#define IOSN_MOTIONANALYSIS_TRC  		"MotionAnalysis_TRC"
#define IOSN_ACCLAIM_ASF         		"Acclaim_ASF"
#define IOSN_ACCLAIM_AMC         		"Acclaim_AMC"
#define IOSN_VICON_C3D           		"Vicon_C3D"
#define IOSN_ADAPTIVEOPTICS_AOA  		"AdaptiveOptics_AOA"
#define IOSN_SUPERFLUO_MCD       		"Superfluo_MCD"
#define IOSN_PUPPETEER_VPM       		"Puppeteer_VPM"
#define IOSN_WAVEFRONT_MOV       		"Wavefront_MOV"
#define IOSN_ASCENSION_BRD       		"Ascension_BRD"

#define IOSN_SKINS				 		"Skins"
#define IOSN_POINTCACHE          		"PointCache"
#define IOSN_QUATERNION			 		"Quaternion"

#define IOSN_SHAPE               		 "Shape"
#define IOSN_LIGHT						 "Light"
#define IOSN_LIGHTATTENUATION            "LightAttenuation"
#define IOSN_CAMERA						 "Camera"
#define IOSN_VIEW_CUBE				     "ViewCube"

#define IOSN_BINDPOSE					 "BindPose"

#define IOSN_EMBEDTEXTURE       		"EmbedTexture"
#define IOSN_CONVERTTOTIFF      		"Convert_2Tiff"
#define IOSN_SMOOTHING          		"Smoothing"

#define IOSN_BAKEANIMATIONLAYERS		"BakeAnimationLayers"
#define IOSN_BAKECOMPLEXANIMATION		"BakeComplexAnimation"

#define IOSN_BAKEFRAMESTART				"BakeFrameStart"		 
#define IOSN_BAKEFRAMEEND				"BakeFrameEnd"		 
#define IOSN_BAKEFRAMESTEP				"BakeFrameStep"		 
#define IOSN_BAKEFRAMESTARTNORESET		"BakeFrameStartNoReset"
#define IOSN_BAKEFRAMEENDNORESET		"BakeFrameEndNoReset"	 
#define IOSN_BAKEFRAMESTEPNORESET		"BakeFrameStepNoReset" 

#define IOSN_USEMATRIXFROMPOSE      	"UseMatrixFromPose"
#define IOSN_NULLSTOPIVOT           	"NullsToPivot"
#define IOSN_PIVOTTONULLS               "PivotToNulls"



#define IOSN_GEOMNORMALPERPOLY		    "GeomNormalPerPoly"
#define IOSN_MAXBONEASBONE				"MaxBoneAsBone"
#define IOSN_MAXNURBSSTEP				"MaxNurbsStep"
#define IOSN_PROTECTDRIVENKEYS          "ProtectDrivenKeys"
#define IOSN_DEFORMNULLSASJOINTS        "DeformNullsAsJoints"

// Note this will use IOSN_SAMPLINGRATE 
#define IOSN_SAMPLINGRATESELECTOR       "SamplingRateSelector"

#define IOSN_SAMPLINGRATE               "CurveFilterSamplingRate"
#define IOSN_APPLYCSTKEYRED             "CurveFilterApplyCstKeyRed"
#define IOSN_CSTKEYREDTPREC				"CurveFilterCstKeyRedTPrec"
#define IOSN_CSTKEYREDRPREC				"CurveFilterCstKeyRedRPrec"
#define IOSN_CSTKEYREDSPREC             "CurveFilterCstKeyRedSPrec"
#define IOSN_CSTKEYREDOPREC             "CurveFilterCstKeyRedOPrec"
#define IOSN_APPLYKEYREDUCE             "CurveFilterApplyKeyReduce"
#define IOSN_KEYREDUCEPREC              "CurveFilterKeyReducePrec" 
#define IOSN_APPLYKEYSONFRM             "CurveFilterApplyKeysOnFrm"
#define IOSN_APPLYKEYSYNC               "CurveFilterApplyKeySync"  
#define IOSN_APPLYUNROLL                "CurveFilterApplyUnroll"   
#define IOSN_UNROLLPREC                 "CurveFilterUnrollPrec"    
#define IOSN_UNROLLPATH                 "CurveFilterUnrollPath"   

#define IOSN_AUTOTANGENTSONLY           "AutoTangentsOnly"

#define IOSN_HARDEDGES                  "HardEdges"
#define IOSN_EXP_HARDEDGES              "expHardEdges"
#define IOSN_BLINDDATA                  "BlindData"
#define IOSN_INSTANCES                  "Instances"
#define IOSN_CONTAINEROBJECTS           "ContainerObjects"
#define IOSN_BYPASSRRSINHERITANCE       "BypassRrsInheritance"
#define IOSN_FORCEWEIGHTNORMALIZE       "ForceWeightNormalize"
#define IOSN_OVERRIDENORMALSLOCK        "OverrideNormalsLock"
#define IOSN_SHAPEANIMATION             "ShapeAnimation"
#define IOSN_SMOOTHKEYASUSER            "SmoothKeyAsUser"

#define IOSN_SCALEFACTOR				"ScaleFactor"
#define IOSN_AXISCONVERSIONMETHOD		"AxisConversionMethod"
#define IOSN_UPAXIS						"UpAxis"
#define IOSN_USETMPFILEPERIPHERAL		"UseTmpFilePeripheral"
#define IOSN_SELECTIONSETNAMEASPOINTCACHE "SelectionSetNameAsPointCache"

#define IOSN_KEEPFRAMERATE               "KeepFrameRate"
#define IOSN_ATTENUATIONASINTENSITYCURVE "AttenuationAsIntensityCurve"

#define IOSN_TIMELINE                    "TimeLine"

#define IOSN_BUTTON_WEB_UPDATE           "WebUpdateButton"
#define IOSN_BUTTON_EDIT                 "EditButton"
#define IOSN_BUTTON_OK                   "OKButton"
#define IOSN_BUTTON_CANCEL               "CancelButton"
#define IOSN_MENU_EDIT_PRESET            "EditPresetMenu"
#define IOSN_MENU_SAVE_PRESET            "SavePresetMenu"

#define IOSN_UIL                         "UILIndex"
#define IOSN_PLUGIN_PRODUCT_FAMILY       "PluginProductFamily"

#define IOSN_PLUGIN_UI_XPOS              "PlugInUIXpos"
#define IOSN_PLUGIN_UI_YPOS              "PlugInUIYpos"

#define IOSN_COLLADA_FRAME_COUNT         "FrameCount"
#define IOSN_COLLADA_START               "Start"
#define IOSN_COLLADA_TAKE_NAME           "TakeName"

#define IOSN_COLLADA_TRIANGULATE         "Triangulate"
#define IOSN_COLLADA_SINGLEMATRIX        "SingleMatrix"
#define IOSN_COLLADA_FRAME_RATE          "FrameRate"

#define IOSN_DXF_TRIANGULATE             "Triangulate"
#define IOSN_DXF_DEFORMATION             "Deformation"

#define IOSN_DXF_WELD_VERTICES           "WeldVertices"
#define IOSN_DXF_OBJECT_DERIVATION       "ObjectDerivation"
#define IOSN_DXF_REFERENCE_NODE          "ReferenceNode"


//---------------------------
// import defined path

#define IMP_PRESETS                     IOSN_IMPORT"|"IOSN_PRESETS_GRP"|"IOSN_PRESETS
#define IMP_STATISTICS                  IOSN_IMPORT"|"IOSN_STATISTICS_GRP"|"IOSN_STATISTICS


#define IMP_PLUGIN_GRP                  IOSN_IMPORT"|"IOSN_PLUGIN_GRP
#define IMP_INCLUDE_GRP         		IOSN_IMPORT"|"IOSN_INCLUDE_GRP
#define IMP_ADV_OPT_GRP         		IOSN_IMPORT"|"IOSN_ADV_OPT_GRP

#define IMP_GEOMETRY            		IMP_INCLUDE_GRP"|"IOSN_GEOMETRY
#define IMP_ANIMATION           		IMP_INCLUDE_GRP"|"IOSN_ANIMATION
#define IMP_SETLOCKEDATTRIB				IMP_INCLUDE_GRP"|"IOSN_SETLOCKEDATTRIB
#define IMP_MERGE_MODE            		IMP_INCLUDE_GRP"|"IOSN_MERGE_MODE

#define IMP_LIGHT						IMP_INCLUDE_GRP"|"IOSN_LIGHT
#define IMP_CAMERA						IMP_INCLUDE_GRP"|"IOSN_CAMERA
#define IMP_VIEW_CUBE                   IMP_INCLUDE_GRP"|"IOSN_VIEW_CUBE

#define IMP_CURVEFILTERS        		IMP_ANIMATION"|"IOSN_CURVE_FILTER
#define IMP_BAKE                		IMP_ANIMATION"|"IOSN_BAKE
#define IMP_CONSTRAINT          		IMP_ANIMATION"|"IOSN_CONSTRAINT
#define IMP_DEFORMATION         		IMP_ANIMATION"|"IOSN_DEFORMATION
#define IMP_QUATERNION          		IMP_ANIMATION"|"IOSN_QUATERNION
#define IMP_MARKERS             		IMP_ANIMATION"|"IOSN_MARKERS
#define IMP_BAKEANIMATIONLAYERS 		IMP_ANIMATION"|"IOSN_BAKEANIMATIONLAYERS
#define IMP_BONE						IMP_ANIMATION"|"IOSN_BONE
#define IMP_SHAPEANIMATION              IMP_ANIMATION"|"IOSN_SHAPEANIMATION
#define IMP_KEEPFRAMERATE               IMP_ANIMATION"|"IOSN_KEEPFRAMERATE
#define IMP_TIMELINE                    IMP_ANIMATION"|"IOSN_TIMELINE

#define IMP_SAMPLINGPANEL               IMP_ANIMATION"|"IOSN_SAMPLINGPANEL
#define IMP_SAMPLINGRATESELECTOR        IMP_SAMPLINGPANEL"|"IOSN_SAMPLINGRATESELECTOR
#define IMP_SAMPLINGRATE				IMP_SAMPLINGPANEL"|"IOSN_SAMPLINGRATE

#define IMP_PROTECTDRIVENKEYS           IMP_ANIMATION"|"IOSN_PROTECTDRIVENKEYS
#define IMP_DEFORMNULLSASJOINTS         IMP_ANIMATION"|"IOSN_DEFORMNULLSASJOINTS
#define IMP_NULLSTOPIVOT        		IMP_ANIMATION"|"IOSN_NULLSTOPIVOT
#define IMP_POINTCACHE       			IMP_ANIMATION"|"IOSN_POINTCACHE
#define IMP_CHARACTER            		IMP_ANIMATION"|"IOSN_CHARACTER
#define IMP_TAKE                        IMP_ANIMATION"|"IOSN_TAKE
#define IMP_ATTENUATIONASINTENSITYCURVE IMP_ANIMATION"|"IOSN_ATTENUATIONASINTENSITYCURVE

#define IMP_UNITS_GRP           		IMP_ADV_OPT_GRP"|"IOSN_UNITS_GRP
#define IMP_AXISCONV_GRP        		IMP_ADV_OPT_GRP"|"IOSN_AXISCONV_GRP
#define IMP_UI                  		IMP_ADV_OPT_GRP"|"IOSN_UI 
#define IMP_FILEFORMAT          		IMP_ADV_OPT_GRP"|"IOSN_FILE_FORMAT

#define IMP_SCALECONVERSION             IMP_UNITS_GRP"|"IOSN_SCALECONVERSION
#define IMP_UNITS_TB                    IMP_UNITS_GRP"|"IOSN_UNITS_TB
#define IMP_MASTERSCALE                 IMP_UNITS_GRP"|"IOSN_MASTERSCALE
#define IMP_UNITS_SCALE                 IMP_UNITS_GRP"|"IOSN_UNITS_SCALE
#define IMP_MASTERUNITSTR               IMP_UNITS_GRP"|"IOSN_MASTERUNITSTR // must be removed DT
#define IMP_UNITSELECTOR                IMP_UNITS_GRP"|"IOSN_UNITSELECTOR
#define IMP_TOTAL_UNITS_SCALE_TB        IMP_UNITS_GRP"|"IOSN_TOTAL_UNITS_SCALE_TB

#define IMP_SHOW_UI_MODE                IMP_UI"|"IOSN_SHOW_UI_MODE
#define IMP_SHOW_WARNINGS_MANAGER       IMP_UI"|"IOSN_SHOW_WARNINGS_MANAGER

#define IMP_FBX                 		IMP_FILEFORMAT"|"IOSN_FBX
#define IMP_DXF                 		IMP_ADV_OPT_GRP"|"IOSN_DXF
#define IMP_OBJ                 		IMP_FILEFORMAT"|"IOSN_OBJ
#define IMP_3DS                 		IMP_FILEFORMAT"|"IOSN_3DS

#define IMP_MOTION_BASE         		IMP_FILEFORMAT"|"IOSN_MOTION_BASE
#define IMP_BIOVISION_BVH       		IMP_FILEFORMAT"|"IOSN_BIOVISION_BVH
#define IMP_MOTIONANALYSIS_HTR  		IMP_FILEFORMAT"|"IOSN_MOTIONANALYSIS_HTR
#define IMP_MOTIONANALYSIS_TRC  		IMP_FILEFORMAT"|"IOSN_MOTIONANALYSIS_TRC
#define IMP_ACCLAIM_ASF         		IMP_FILEFORMAT"|"IOSN_ACCLAIM_ASF
#define IMP_ACCLAIM_AMC         		IMP_FILEFORMAT"|"IOSN_ACCLAIM_AMC
#define IMP_VICON_C3D           		IMP_FILEFORMAT"|"IOSN_VICON_C3D
#define IMP_ADAPTIVEOPTICS_AOA  		IMP_FILEFORMAT"|"IOSN_ADAPTIVEOPTICS_AOA
#define IMP_SUPERFLUO_MCD       		IMP_FILEFORMAT"|"IOSN_SUPERFLUO_MCD
#define IMP_PUPPETEER_VPM       		IMP_FILEFORMAT"|"IOSN_PUPPETEER_VPM
#define IMP_WAVEFRONT_MOV       		IMP_FILEFORMAT"|"IOSN_WAVEFRONT_MOV
#define IMP_ASCENSION_BRD       		IMP_FILEFORMAT"|"IOSN_ASCENSION_BRD

#define IMP_SMOOTHING           		IMP_GEOMETRY"|"IOSN_SMOOTHING

#define IMP_HARDEDGES                   IMP_GEOMETRY"|"IOSN_HARDEDGES
#define IMP_BLINDDATA                   IMP_GEOMETRY"|"IOSN_BLINDDATA
#define IMP_OVERRIDENORMALSLOCK         IMP_GEOMETRY"|"IOSN_OVERRIDENORMALSLOCK

#define IMP_BONE_WIDTHHEIGHTLOCK 		IMP_BONE"|"IOSN_BONEWIDTHHEIGHTLOCK
#define IMP_BONEASDUMMY          		IMP_BONE"|"IOSN_BONEASDUMMY
#define IMP_BONEMAX4BONEWIDTH    		IMP_BONE"|"IOSN_BONEMAX4BONEWIDTH   
#define IMP_BONEMAX4BONEHEIGHT   		IMP_BONE"|"IOSN_BONEMAX4BONEHEIGHT  
#define IMP_BONEMAX4BONETAPER    		IMP_BONE"|"IOSN_BONEMAX4BONETAPER

#define IMP_SHAPE                		IMP_DEFORMATION"|"IOSN_SHAPE
#define IMP_SKINS			     		IMP_DEFORMATION"|"IOSN_SKINS
#define IMP_USEMATRIXFROMPOSE    		IMP_DEFORMATION"|"IOSN_USEMATRIXFROMPOSE
#define IMP_FORCEWEIGHTNORMALIZE        IMP_DEFORMATION"|"IOSN_FORCEWEIGHTNORMALIZE



#define IMP_APPLYCSTKEYRED              IMP_CURVEFILTERS"|"IOSN_APPLYCSTKEYRED
#define IMP_CSTKEYREDTPREC				IMP_APPLYCSTKEYRED"|"IOSN_CSTKEYREDTPREC	
#define IMP_CSTKEYREDRPREC				IMP_APPLYCSTKEYRED"|"IOSN_CSTKEYREDRPREC	
#define IMP_CSTKEYREDSPREC				IMP_APPLYCSTKEYRED"|"IOSN_CSTKEYREDSPREC  
#define IMP_CSTKEYREDOPREC				IMP_APPLYCSTKEYRED"|"IOSN_CSTKEYREDOPREC
#define IMP_AUTOTANGENTSONLY            IMP_APPLYCSTKEYRED"|"IOSN_AUTOTANGENTSONLY

#define IMP_APPLYKEYREDUCE				IMP_CURVEFILTERS"|"IOSN_APPLYKEYREDUCE  
#define IMP_KEYREDUCEPREC				IMP_APPLYKEYREDUCE"|"IOSN_KEYREDUCEPREC   
#define IMP_APPLYKEYSONFRM				IMP_APPLYKEYREDUCE"|"IOSN_APPLYKEYSONFRM  
#define IMP_APPLYKEYSYNC				IMP_APPLYKEYREDUCE"|"IOSN_APPLYKEYSYNC  

#define IMP_APPLYUNROLL					IMP_CURVEFILTERS"|"IOSN_APPLYUNROLL     
#define IMP_UNROLLPREC					IMP_APPLYUNROLL"|"IOSN_UNROLLPREC      
#define IMP_UNROLLPATH					IMP_APPLYUNROLL"|"IOSN_UNROLLPATH      

#define IMP_UP_AXIS					    IMP_AXISCONV_GRP"|"IOSN_UP_AXIS
#define IMP_UP_AXIS_MAX					IMP_AXISCONV_GRP"|"IOSN_UP_AXIS_MAX
#define IMP_ZUPROTATION_MAX				IMP_AXISCONV_GRP"|"IOSN_ZUPROTATION_MAX
#define IMP_AXISCONVERSION              IMP_AXISCONV_GRP"|"IOSN_AXISCONVERSION


#define IMP_BUILDNUMBER                 IMP_PLUGIN_GRP"|"IOSN_BUILDNUMBER
#define IMP_PLUGIN_UI_WIDTH             IMP_PLUGIN_GRP"|"IOSN_PLUGIN_UI_WIDTH
#define IMP_PLUGIN_UI_HEIGHT            IMP_PLUGIN_GRP"|"IOSN_PLUGIN_UI_HEIGHT
#define IMP_PRESET_SELECTED             IMP_PLUGIN_GRP"|"IOSN_PRESET_SELECTED

#define IMP_UIL                         IMP_PLUGIN_GRP"|"IOSN_UIL
#define IMP_PLUGIN_PRODUCT_FAMILY       IMP_PLUGIN_GRP"|"IOSN_PLUGIN_PRODUCT_FAMILY

#define IMP_PLUGIN_UI_XPOS              IMP_PLUGIN_GRP"|"IOSN_PLUGIN_UI_XPOS     
#define IMP_PLUGIN_UI_YPOS              IMP_PLUGIN_GRP"|"IOSN_PLUGIN_UI_YPOS     

#define IMP_BUTTON_WEB_UPDATE           IMP_PLUGIN_GRP"|"IOSN_BUTTON_WEB_UPDATE
#define IMP_BUTTON_EDIT                 IMP_PLUGIN_GRP"|"IOSN_BUTTON_EDIT      
#define IMP_BUTTON_OK                   IMP_PLUGIN_GRP"|"IOSN_BUTTON_OK        
#define IMP_BUTTON_CANCEL               IMP_PLUGIN_GRP"|"IOSN_BUTTON_CANCEL    
#define IMP_MENU_EDIT_PRESET            IMP_PLUGIN_GRP"|"IOSN_MENU_EDIT_PRESET 
#define IMP_MENU_SAVE_PRESET            IMP_PLUGIN_GRP"|"IOSN_MENU_SAVE_PRESET 


#define IMP_DXF_WELD_VERTICES           IMP_DXF"|"IOSN_DXF_WELD_VERTICES
#define IMP_DXF_OBJECT_DERIVATION       IMP_DXF"|"IOSN_DXF_OBJECT_DERIVATION
#define IMP_DXF_REFERENCE_NODE          IMP_DXF"|"IOSN_DXF_REFERENCE_NODE


// end of import defined path
//---------------------------

//---------------------------
// export defined path

#define EXP_ADV_OPT_GRP         		IOSN_EXPORT"|"IOSN_ADV_OPT_GRP
#define EXP_PRESETS_GRP            		IOSN_EXPORT"|"IOSN_PRESETS_GRP
#define EXP_STATISTICS          		IOSN_EXPORT"|"IOSN_STATISTICS_GRP"|"IOSN_STATISTICS


#define EXP_PLUGIN_GRP                  IOSN_EXPORT"|"IOSN_PLUGIN_GRP
#define EXP_INCLUDE_GRP         		IOSN_EXPORT"|"IOSN_INCLUDE_GRP

#define EXP_UNITS_GRP           		EXP_ADV_OPT_GRP"|"IOSN_UNITS_GRP
#define EXP_FILEFORMAT          		EXP_ADV_OPT_GRP"|"IOSN_FILE_FORMAT
#define EXP_AXISCONV_GRP        		EXP_ADV_OPT_GRP"|"IOSN_AXISCONV_GRP
#define EXP_UI                  		EXP_ADV_OPT_GRP"|"IOSN_UI

#define EXP_SCALEFACTOR			        EXP_AXISCONV_GRP"|"IOSN_SCALEFACTOR
#define EXP_AXISCONVERSIONMETHOD	    EXP_AXISCONV_GRP"|"IOSN_AXISCONVERSIONMETHOD
#define EXP_UPAXIS					    EXP_AXISCONV_GRP"|"IOSN_UPAXIS
#define EXP_AXIS_CON_METHOD_SELECTOR    EXP_AXISCONV_GRP"|"IOSN_AXIS_CON_METHOD_SELECTOR

#define EXP_AXIS_SELECTOR               EXP_AXISCONV_GRP"|"IOSN_AXIS_SELECTOR
#define EXP_USETMPFILEPERIPHERAL	    EXP_AXISCONV_GRP"|"IOSN_USETMPFILEPERIPHERAL
#define EXP_FORCENORMALSBYCONTROLPOINT	EXP_AXISCONV_GRP"|"IOSN_FORCENORMALSBYCONTROLPOINT

#define EXP_UNITS_SCALE            		EXP_UNITS_GRP"|"IOSN_UNITS_SCALE
#define EXP_MASTERSCALE                 EXP_UNITS_GRP "|"IOSN_MASTERSCALE
#define EXP_MASTERUNITSTR               EXP_UNITS_GRP"|"IOSN_MASTERUNITSTR
#define EXP_UNITSELECTOR                EXP_UNITS_GRP"|"IOSN_UNITSELECTOR

#define EXP_TOTAL_UNITS_SCALE_TB        EXP_UNITS_GRP"|"IOSN_TOTAL_UNITS_SCALE_TB

#define EXP_SHOW_UI_MODE                EXP_UI"|"IOSN_SHOW_UI_MODE
#define EXP_SHOW_WARNINGS_MANAGER       EXP_UI"|"IOSN_SHOW_WARNINGS_MANAGER

#define EXP_PRESETS                     EXP_PRESETS_GRP"|"IOSN_PRESETS

#define EXP_GEOMETRY            		EXP_INCLUDE_GRP"|"IOSN_GEOMETRY
#define EXP_ANIMATION           		EXP_INCLUDE_GRP"|"IOSN_ANIMATION
#define EXP_PIVOTTONULLS                EXP_INCLUDE_GRP"|"IOSN_PIVOTTONULLS

#define EXP_LIGHT						EXP_INCLUDE_GRP"|"IOSN_LIGHT	
#define EXP_LIGHTATTENUATION            EXP_INCLUDE_GRP"|"IOSN_LIGHTATTENUATION
#define EXP_CAMERA						EXP_INCLUDE_GRP"|"IOSN_CAMERA	
#define EXP_BINDPOSE					EXP_INCLUDE_GRP"|"IOSN_BINDPOSE

#define EXP_SELECTIONONLY               EXP_INCLUDE_GRP"|"IOSN_SELECTIONONLY
#define EXP_INSTANCES                   EXP_INCLUDE_GRP"|"IOSN_INSTANCES
#define EXP_CONTAINEROBJECTS			EXP_INCLUDE_GRP"|"IOSN_CONTAINEROBJECTS
#define EXP_BYPASSRRSINHERITANCE        EXP_INCLUDE_GRP"|"IOSN_BYPASSRRSINHERITANCE

#define EXP_EMBEDTEXTURE        		EXP_INCLUDE_GRP"|"IOSN_EMBEDTEXTURE
#define EXP_CONVERTTOTIFF       		EXP_EMBEDTEXTURE"|"IOSN_CONVERTTOTIFF

#define EXP_CONSTRAINT          		EXP_ANIMATION"|"IOSN_CONSTRAINT
#define EXP_CURVEFILTERS        		EXP_ANIMATION"|"IOSN_CURVE_FILTER
#define EXP_BAKE                		EXP_ANIMATION"|"IOSN_BAKE
#define EXP_DEFORMATION					EXP_ANIMATION"|"IOSN_DEFORMATION
#define EXP_BAKECOMPLEXANIMATION	    EXP_ANIMATION"|"IOSN_BAKECOMPLEXANIMATION
#define EXP_BONE						EXP_ANIMATION"|"IOSN_BONE
#define EXP_QUATERNION          		EXP_ANIMATION"|"IOSN_QUATERNION
#define EXP_SAMPLINGFRAMERATE           EXP_ANIMATION"|"IOSN_SAMPLINGFRAMERATE
#define EXP_POINTCACHE       			EXP_ANIMATION"|"IOSN_POINTCACHE
#define EXP_CHARACTER            		EXP_ANIMATION"|"IOSN_CHARACTER
#define EXP_SMOOTHKEYASUSER             EXP_ANIMATION"|"IOSN_SMOOTHKEYASUSER

#define EXP_MRCUSTOMATTRIBUTES		 	EXP_GEOMETRY"|"IOSN_MRCUSTOMATTRIBUTES
#define EXP_MESHPRIMITIVE			 	EXP_GEOMETRY"|"IOSN_MESHPRIMITIVE
#define EXP_MESHTRIANGLE			 	EXP_GEOMETRY"|"IOSN_MESHTRIANGLE 
#define EXP_MESHPOLY				 	EXP_GEOMETRY"|"IOSN_MESHPOLY
#define EXP_NURB					 	EXP_GEOMETRY"|"IOSN_NURB
#define EXP_PATCH					 	EXP_GEOMETRY"|"IOSN_PATCH
#define EXP_BIP2FBX					 	EXP_GEOMETRY"|"IOSN_BIP2FBX
#define EXP_GEOMNORMALPERPOLY			EXP_GEOMETRY"|"IOSN_GEOMNORMALPERPOLY
#define EXP_MAXBONEASBONE				EXP_GEOMETRY"|"IOSN_MAXBONEASBONE
#define EXP_MAXNURBSSTEP				EXP_GEOMETRY"|"IOSN_MAXNURBSSTEP
#define EXP_SMOOTHING           		EXP_GEOMETRY"|"IOSN_SMOOTHING
#define EXP_BLINDDATA                   EXP_GEOMETRY"|"IOSN_BLINDDATA
#define EXP_NURBSSURFACEAS              EXP_GEOMETRY"|"IOSN_GEOMETRYNURBSSURFACEAS
#define EXP_HARDEDGES                   EXP_GEOMETRY"|"IOSN_EXP_HARDEDGES
#define EXP_ANIMATIONONLY               EXP_GEOMETRY"|"IOSN_ANIMATIONONLY


#define EXP_SELECTIONSETNAMEASPOINTCACHE EXP_POINTCACHE"|"IOSN_SELECTIONSETNAMEASPOINTCACHE

#define EXP_GEOMETRYMESHPRIMITIVEAS  	EXP_GEOMETRY"|"IOSN_GEOMETRYMESHPRIMITIVEAS
#define EXP_GEOMETRYMESHTRIANGLEAS   	EXP_GEOMETRY"|"IOSN_GEOMETRYMESHTRIANGLEAS 
#define EXP_GEOMETRYMESHPOLYAS	     	EXP_GEOMETRY"|"IOSN_GEOMETRYMESHPOLYAS     
#define EXP_GEOMETRYNURBSAS	         	EXP_GEOMETRY"|"IOSN_GEOMETRYNURBSAS        
#define EXP_GEOMETRYPATCHAS          	EXP_GEOMETRY"|"IOSN_GEOMETRYPATCHAS        

#define EXP_BAKEFRAMESTART				EXP_BAKECOMPLEXANIMATION"|"IOSN_BAKEFRAMESTART			
#define EXP_BAKEFRAMEEND				EXP_BAKECOMPLEXANIMATION"|"IOSN_BAKEFRAMEEND			
#define EXP_BAKEFRAMESTEP				EXP_BAKECOMPLEXANIMATION"|"IOSN_BAKEFRAMESTEP			
#define EXP_BAKEFRAMESTARTNORESET		EXP_BAKECOMPLEXANIMATION"|"IOSN_BAKEFRAMESTARTNORESET	
#define EXP_BAKEFRAMEENDNORESET			EXP_BAKECOMPLEXANIMATION"|"IOSN_BAKEFRAMEENDNORESET	
#define EXP_BAKEFRAMESTEPNORESET		EXP_BAKECOMPLEXANIMATION"|"IOSN_BAKEFRAMESTEPNORESET	

#define EXP_FBX                 		EXP_ADV_OPT_GRP"|"IOSN_FBX
#define EXP_DXF                 		EXP_ADV_OPT_GRP"|"IOSN_DXF
#define EXP_OBJ                 		EXP_FILEFORMAT"|"IOSN_OBJ
#define EXP_3DS                 		EXP_FILEFORMAT"|"IOSN_3DS
#define EXP_COLLADA             		EXP_ADV_OPT_GRP"|"IOSN_COLLADA

#define EXP_MOTION_BASE         		EXP_FILEFORMAT"|"IOSN_MOTION_BASE
#define EXP_BIOVISION_BVH       		EXP_FILEFORMAT"|"IOSN_BIOVISION_BVH
#define EXP_MOTIONANALYSIS_HTR  		EXP_FILEFORMAT"|"IOSN_MOTIONANALYSIS_HTR
#define EXP_MOTIONANALYSIS_TRC  		EXP_FILEFORMAT"|"IOSN_MOTIONANALYSIS_TRC
#define EXP_ACCLAIM_ASF         		EXP_FILEFORMAT"|"IOSN_ACCLAIM_ASF
#define EXP_ACCLAIM_AMC         		EXP_FILEFORMAT"|"IOSN_ACCLAIM_AMC
#define EXP_VICON_C3D           		EXP_FILEFORMAT"|"IOSN_VICON_C3D
#define EXP_ADAPTIVEOPTICS_AOA  		EXP_FILEFORMAT"|"IOSN_ADAPTIVEOPTICS_AOA
#define EXP_SUPERFLUO_MCD       		EXP_FILEFORMAT"|"IOSN_SUPERFLUO_MCD
#define EXP_PUPPETEER_VPM       		EXP_FILEFORMAT"|"IOSN_PUPPETEER_VPM
#define EXP_WAVEFRONT_MOV       		EXP_FILEFORMAT"|"IOSN_WAVEFRONT_MOV
#define EXP_ASCENSION_BRD       		EXP_FILEFORMAT"|"IOSN_ASCENSION_BRD


#define EXP_ASCIIFBX                    EXP_FBX"|"IOSN_ASCIIFBX

#define EXP_SHAPE               		EXP_DEFORMATION"|"IOSN_SHAPE
#define EXP_SKINS			    		EXP_DEFORMATION"|"IOSN_SKINS

#define EXP_APPLYCSTKEYRED              EXP_CURVEFILTERS"|"IOSN_APPLYCSTKEYRED
#define EXP_SAMPLINGRATE				EXP_APPLYCSTKEYRED"|"IOSN_SAMPLINGRATE
#define EXP_CSTKEYREDTPREC				EXP_APPLYCSTKEYRED"|"IOSN_CSTKEYREDTPREC	
#define EXP_CSTKEYREDRPREC				EXP_APPLYCSTKEYRED"|"IOSN_CSTKEYREDRPREC	
#define EXP_CSTKEYREDSPREC				EXP_APPLYCSTKEYRED"|"IOSN_CSTKEYREDSPREC  
#define EXP_CSTKEYREDOPREC				EXP_APPLYCSTKEYRED"|"IOSN_CSTKEYREDOPREC  
#define EXP_AUTOTANGENTSONLY            EXP_APPLYCSTKEYRED"|"IOSN_AUTOTANGENTSONLY

#define EXP_APPLYKEYREDUCE				EXP_CURVEFILTERS"|"IOSN_APPLYKEYREDUCE  
#define EXP_KEYREDUCEPREC				EXP_APPLYKEYREDUCE"|"IOSN_KEYREDUCEPREC   
#define EXP_APPLYKEYSONFRM				EXP_APPLYKEYREDUCE"|"IOSN_APPLYKEYSONFRM  
#define EXP_APPLYKEYSYNC				EXP_APPLYKEYREDUCE"|"IOSN_APPLYKEYSYNC  

#define EXP_APPLYUNROLL					EXP_CURVEFILTERS"|"IOSN_APPLYUNROLL     
#define EXP_UNROLLPREC					EXP_APPLYUNROLL"|"IOSN_UNROLLPREC      
#define EXP_UNROLLPATH					EXP_APPLYUNROLL"|"IOSN_UNROLLPATH      

#define EXP_BUILDNUMBER                 EXP_PLUGIN_GRP"|"IOSN_BUILDNUMBER
#define EXP_PLUGIN_UI_WIDTH             EXP_PLUGIN_GRP"|"IOSN_PLUGIN_UI_WIDTH
#define EXP_PLUGIN_UI_HEIGHT            EXP_PLUGIN_GRP"|"IOSN_PLUGIN_UI_HEIGHT
#define EXP_PRESET_SELECTED             EXP_PLUGIN_GRP"|"IOSN_PRESET_SELECTED

#define EXP_UIL                         EXP_PLUGIN_GRP"|"IOSN_UIL
#define EXP_PLUGIN_PRODUCT_FAMILY       EXP_PLUGIN_GRP"|"IOSN_PLUGIN_PRODUCT_FAMILY

#define EXP_PLUGIN_UI_XPOS              EXP_PLUGIN_GRP"|"IOSN_PLUGIN_UI_XPOS     
#define EXP_PLUGIN_UI_YPOS              EXP_PLUGIN_GRP"|"IOSN_PLUGIN_UI_YPOS     

#define EXP_BUTTON_WEB_UPDATE           EXP_PLUGIN_GRP"|"IOSN_BUTTON_WEB_UPDATE
#define EXP_BUTTON_EDIT                 EXP_PLUGIN_GRP"|"IOSN_BUTTON_EDIT      
#define EXP_BUTTON_OK                   EXP_PLUGIN_GRP"|"IOSN_BUTTON_OK        
#define EXP_BUTTON_CANCEL               EXP_PLUGIN_GRP"|"IOSN_BUTTON_CANCEL    
#define EXP_MENU_EDIT_PRESET            EXP_PLUGIN_GRP"|"IOSN_MENU_EDIT_PRESET 
#define EXP_MENU_SAVE_PRESET            EXP_PLUGIN_GRP"|"IOSN_MENU_SAVE_PRESET 
    
#define EXP_COLLADA_TRIANGULATE         EXP_COLLADA"|"IOSN_COLLADA_TRIANGULATE 
#define EXP_COLLADA_SINGLEMATRIX        EXP_COLLADA"|"IOSN_COLLADA_SINGLEMATRIX
#define EXP_COLLADA_FRAME_RATE          EXP_COLLADA"|"IOSN_COLLADA_FRAME_RATE  

#define EXP_DXF_TRIANGULATE             EXP_DXF"|"IOSN_DXF_TRIANGULATE
#define EXP_DXF_DEFORMATION             EXP_DXF"|"IOSN_DXF_DEFORMATION

// end of export defined path
//---------------------------

