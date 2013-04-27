/******************************************************************************

 @File         PVRTPFXSemantics.h

 @Title        PFX Semantics

 @Version      

 @Copyright    Copyright (c) Imagination Technologies Limited.

 @Platform     All

 @Description  A list of supported PFX semantics.

******************************************************************************/
#ifndef PVRTPFXSEMANTICS_H
#define PVRTPFXSEMANTICS_H

struct SPVRTPFXUniformSemantic;
/****************************************************************************
** Semantic Enumerations
****************************************************************************/
enum EPVRTPFXUniformSemantic
{
	ePVRTPFX_UsPOSITION,
	ePVRTPFX_UsNORMAL,
	ePVRTPFX_UsTANGENT,
	ePVRTPFX_UsBINORMAL,
	ePVRTPFX_UsUV,
	ePVRTPFX_UsVERTEXCOLOR,
	ePVRTPFX_UsBONEINDEX,
	ePVRTPFX_UsBONEWEIGHT,

	ePVRTPFX_UsWORLD,
	ePVRTPFX_UsWORLDI,
	ePVRTPFX_UsWORLDIT,
	ePVRTPFX_UsVIEW,
	ePVRTPFX_UsVIEWI,
	ePVRTPFX_UsVIEWIT,
	ePVRTPFX_UsPROJECTION,
	ePVRTPFX_UsPROJECTIONI,
	ePVRTPFX_UsPROJECTIONIT,
	ePVRTPFX_UsWORLDVIEW,
	ePVRTPFX_UsWORLDVIEWI,
	ePVRTPFX_UsWORLDVIEWIT,
	ePVRTPFX_UsWORLDVIEWPROJECTION,
	ePVRTPFX_UsWORLDVIEWPROJECTIONI,
	ePVRTPFX_UsWORLDVIEWPROJECTIONIT,
	ePVRTPFX_UsVIEWPROJECTION,
	ePVRTPFX_UsVIEWPROJECTIONI,
	ePVRTPFX_UsVIEWPROJECTIONIT,
	ePVRTPFX_UsOBJECT,
	ePVRTPFX_UsOBJECTI,
	ePVRTPFX_UsOBJECTIT,
	ePVRTPFX_UsUNPACKMATRIX,

	ePVRTPFX_UsBONECOUNT,
	ePVRTPFX_UsBONEMATRIXARRAY,
	ePVRTPFX_UsBONEMATRIXARRAYIT,

	ePVRTPFX_UsMATERIALOPACITY,
	ePVRTPFX_UsMATERIALSHININESS,
	ePVRTPFX_UsMATERIALCOLORAMBIENT,
	ePVRTPFX_UsMATERIALCOLORDIFFUSE,
	ePVRTPFX_UsMATERIALCOLORSPECULAR,

	ePVRTPFX_UsLIGHTCOLOR,
	ePVRTPFX_UsLIGHTPOSMODEL,
	ePVRTPFX_UsLIGHTPOSWORLD,
	ePVRTPFX_UsLIGHTPOSEYE,
	ePVRTPFX_UsLIGHTDIRMODEL,
	ePVRTPFX_UsLIGHTDIRWORLD,
	ePVRTPFX_UsLIGHTDIREYE,
	ePVRTPFX_UsLIGHTATTENUATION,
	ePVRTPFX_UsLIGHTFALLOFF,

	ePVRTPFX_UsEYEPOSMODEL,
	ePVRTPFX_UsEYEPOSWORLD,
	ePVRTPFX_UsTEXTURE,
	ePVRTPFX_UsANIMATION,

	ePVRTPFX_UsVIEWPORTPIXELSIZE,
	ePVRTPFX_UsVIEWPORTCLIPPING,
	ePVRTPFX_UsTIME,
	ePVRTPFX_UsTIMECOS,
	ePVRTPFX_UsTIMESIN,
	ePVRTPFX_UsTIMETAN,
	ePVRTPFX_UsTIME2PI,
	ePVRTPFX_UsTIME2PICOS,
	ePVRTPFX_UsTIME2PISIN,
	ePVRTPFX_UsTIME2PITAN,
	ePVRTPFX_UsRANDOM,

	ePVRTPFX_NumSemantics,
};

const SPVRTPFXUniformSemantic* PVRTPFXSemanticsGetSemanticList();

#endif /* PVRTPFXSEMANTICS_H */

/*****************************************************************************
 End of file (PVRTPFXSemantics.h)
*****************************************************************************/

