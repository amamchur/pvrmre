/******************************************************************************

 @File         PVRTPFXParserAPI.h

 @Title        OGLES2/PVRTPFXParserAPI.h

 @Version      

 @Copyright    Copyright (c) Imagination Technologies Limited.

 @Platform     Windows + Linux

 @Description  Declaration of PFX file parser

******************************************************************************/

#ifndef _PVRTPFXPARSERAPI_H_
#define _PVRTPFXPARSERAPI_H_

#include "../PVRTError.h"

/****************************************************************************
** Structures
****************************************************************************/

/*! Application supplies an array of these so PVRTPFX can translate strings to numbers */
struct SPVRTPFXUniformSemantic
{
	const char		*p;	// String containing semantic
	unsigned int	n;	// Application-defined semantic value
};

/*! PVRTPFX returns an array of these to indicate GL locations & semantics to the application */
struct SPVRTPFXUniform
{
	unsigned int	nLocation;	// GL location of the Uniform
	unsigned int	nSemantic;	// Application-defined semantic value
	unsigned int	nIdx;		// Index; for example two semantics might be LIGHTPOSITION0 and LIGHTPOSITION1
	CPVRTString		sValueName;		// The name of the variable referenced in shader code
};

/*! An array of these is gained from PVRTPFX so the application can fill in the texture handles*/
struct SPVRTPFXTexture
{
	CPVRTStringHash		Name;		// texture name
	GLuint				ui;		// Loaded texture handle
	GLuint				unit;	// The bound texture unit
	unsigned int		flags;	// Texture type i.e 2D, Cubemap
};

class PVRTPFXEffectDelegate
{
public:
	virtual EPVRTError PVRTPFXOnLoadTexture(const CPVRTStringHash& TextureName, GLuint& uiHandle, unsigned int& uiFlags) = 0;
	virtual ~PVRTPFXEffectDelegate() { }
};

/*!**************************************************************************
@Class CPVRTPFXEffect
@Brief PFX effect
****************************************************************************/
class CPVRTPFXEffect
{
public:
	/*!***************************************************************************
	@Function			CPVRTPFXEffect Blank Constructor
	@Description		Sets the context to NULL and initialises the member variables to zero.
	*****************************************************************************/
	CPVRTPFXEffect();

	/*!***************************************************************************
	@Function			CPVRTPFXEffect Constructor
	@Description		Sets the context and initialises the member variables to zero.
	*****************************************************************************/
	CPVRTPFXEffect(SPVRTContext &sContext);

	/*!***************************************************************************
	@Function			CPVRTPFXEffect Destructor
	@Description		Calls Destroy().
	*****************************************************************************/
	~CPVRTPFXEffect();

	/*!***************************************************************************
	@Function			Load
	@Input				src					PFX Parser Object
	@Input				pszEffect			Effect name
	@Input				pszFileName			Effect file name
	@Input				pDelegate			A delegate which will receive callbacks
	@Output				uiUnknownUniforms	Number of unknown uniforms found
	@Output				pReturnError		Error string
	@Returns			PVR_SUCCESS if load succeeded
	@Description		Loads the specified effect from the CPVRTPFXParser object.
						Compiles and links the shaders. Initialises texture data.
	*****************************************************************************/
	EPVRTError Load(CPVRTPFXParser &src, const char * const pszEffect, const char * const pszFileName, 
					PVRTPFXEffectDelegate* pDelegate, unsigned int& uiUnknownUniforms, CPVRTString *pReturnError);

	/*!***************************************************************************
	@Function			Destroy
	@Description		Deletes the gl program object and texture data.
	*****************************************************************************/
	void Destroy();

	/*!***************************************************************************
	@Function			Activate
	@Input				i32RenderTextureId		The ID of the render target of the current task
	@Input				ui32ReplacementTexture	The ID of the texture that should be used instead
	@Returns			EPVRTError				PVR_SUCCESS if activate succeeded
	@Description		Selects the gl program object and binds the textures.
						If the render target texture for the current render pass is required
						in this effect (and therefore cannot be sampled),
						load the replacement texture instead.
	*****************************************************************************/
	EPVRTError Activate(const int i32RenderTextureId=-1, const unsigned int ui32ReplacementTexture=0);

	/*!***************************************************************************
	@Function			GetTextureArray
	@Returns			SPVRTPFXTexture*		pointer to the texture data array
	@Description		Gets the texture data array.
	*****************************************************************************/
	const CPVRTArray<SPVRTPFXTexture>& GetTextureArray() const;

	/*!***************************************************************************
	@Function		GetUniformArray
	@Return			const CPVRTArray<SPVRTPFXUniform>&	
	@Description	Returns a list of known semantics.
	*****************************************************************************/
	const CPVRTArray<SPVRTPFXUniform>& GetUniformArray() const;

	/*!***************************************************************************
	@Function		GetSemanticArray
	@Return			const CPVRTArray<SPVRTPFXUniformSemantic>&	
	@Description	Gets the array of registered semantics which will be used to
	match PFX code.
	*****************************************************************************/
	const CPVRTArray<SPVRTPFXUniformSemantic>& GetSemanticArray() const;

	/*!***************************************************************************
	@Function			SetTexture
	@Input				nIdx				texture number
	@Input				ui					opengl texture handle
	@Input				u32flags			texture flags
	@Description		Sets the textrue and applys the filtering.
	*****************************************************************************/
	void SetTexture(const unsigned int nIdx, const GLuint ui, const unsigned int u32flags=0);

	/*!***************************************************************************
	@Function			SetDefaultSemanticValue
	@Input				pszName				name of uniform
	@Input				psDefaultValue      pointer to default value
	@Description		Sets the dafault value for the uniform semantic.
	*****************************************************************************/
	void SetDefaultUniformValue(const char *const pszName, const SPVRTSemanticDefaultData *psDefaultValue);

	/*!***************************************************************************
	@Function		RegisterUniformSemantic
	@Input			psUniforms			Array of semantics to register
	@Input			uiNumUniforms		Number provided
	@Output			pReturnError		Human-readable error if any
	@Return			PVR_SUCCESS on success	
	@Description	Registers a user-provided uniform semantic.
	*****************************************************************************/
	EPVRTError RegisterUniformSemantic(const SPVRTPFXUniformSemantic* const psUniforms, unsigned int uiNumUniforms, CPVRTString* pReturnError);

	/*!***************************************************************************
	@Function		RemoveUniformSemantic
	@Input			uiSemanticID
	@Output			pReturnError
	@Return			PVR_SUCCESS on success	
	@Description	Removes a given semantic ID from the 'known' semantic list and 
					re-parses the effect to update the uniform table.
	*****************************************************************************/
	EPVRTError RemoveUniformSemantic(unsigned int uiSemanticID, CPVRTString* pReturnError);

	/*!***************************************************************************
	 @Function			SetContext
	 @Input				pContext			context pointer
	 @Description		Sets the context for this effect.
	 *****************************************************************************/
	void SetContext(SPVRTContext * const pContext);
	
	/*!***************************************************************************
	@Function		GetProgramHandle
	@Return			unsigned int	
	@Description	Returns the OGL program handle.
	*****************************************************************************/
	unsigned int GetProgramHandle() const;

	/*!***************************************************************************
	@Function		GetEffectIndex
	@Return			unsigned int	
	@Description	Gets the active effect index within the PFX file.
	*****************************************************************************/
	unsigned int GetEffectIndex() const;

private:
	/*!***************************************************************************
	@Function		LoadShadersForEffect
	@Input			pszFileName
	@Output			pReturnError
	@Return			EPVRTError	
	@Description	Loads all of the GLSL shaders for an effect.
	*****************************************************************************/
	EPVRTError LoadShadersForEffect(CPVRTPFXParser &src, const char * const pszFileName, CPVRTString *pReturnError);

	/*!***************************************************************************
	@Function		LoadTexturesForEffect
	@Output			pReturnError
	@Return			EPVRTError	
	@Description	Loads all of the textures for this effect.
	*****************************************************************************/
	EPVRTError LoadTexturesForEffect(PVRTPFXEffectDelegate* pDelegate, CPVRTString *pReturnError);

	/*!***************************************************************************
	@Function		BuildUniformTable
	@Output			uiUnknownSemantics
	@Output			pReturnError
	@Return			EPVRTError	
	@Description	Builds the uniform table from a list of known semantics.
	*****************************************************************************/
	EPVRTError RebuildUniformTable(unsigned int& uiUnknownSemantics, CPVRTString* pReturnError);

protected:
	bool									m_bLoaded;
	SPVRTContext*							m_psContext;
	CPVRTPFXParser*							m_pParser;
	unsigned int							m_nEffect;

	GLuint									m_uiProgram;		// Loaded program

	CPVRTArray<SPVRTPFXTexture>				m_Textures;			// Array of loaded textures
	CPVRTArray<SPVRTPFXUniform>				m_Uniforms;			// Array of found uniforms

	CPVRTArray<SPVRTPFXUniformSemantic>		m_Semantics;		// An array of registered semantics.
};

/****************************************************************************
** Auxiliary functions
****************************************************************************/
inline bool operator==(const SPVRTPFXUniformSemantic& lhs, const SPVRTPFXUniformSemantic& rhs)
{
	return (lhs.n == rhs.n);
}

#endif /* _PVRTPFXPARSERAPI_H_ */

/*****************************************************************************
 End of file (PVRTPFXParserAPI.h)
*****************************************************************************/

