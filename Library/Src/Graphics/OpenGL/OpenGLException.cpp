/***************************************************************************************************
 * File:         $URL: https://svn.bolitho.us/Library/Trunk/Src/Graphics/OpenGL/OpenGLException.cpp $
 * Author:       $Author: BOLITHO\matthew $
 * Revision:     $Rev: 1631 $
 * Last Updated: $Date: 2008-09-15 07:52:23 -0700 (Mon, 15 Sep 2008) $
 * 
 * 
 * Copyright (c) 2004-2007, Matthew G Bolitho
 * All rights reserved.
 *
 *
 * Microsoft Reference License (Ms-RL)
 *   
 * This license governs use of the accompanying software. If you use the software, you accept 
 * this license. If you do not accept the license, do not use the software.
 *
 * 1. Definitions
 *  - The terms "reproduce," "reproduction" and "distribution" have the same meaning here as under 
 *    U.S. copyright law.
 *  - "You" means the licensee of the software.
 *  - "Your company" means the company you worked for when you downloaded the software.
 *  - "Reference use" means use of the software within your company as a reference, in read only 
 *    form, for the sole purposes of debugging your products, maintaining your products, or 
 *    enhancing the interoperability of your products with the software, and specifically excludes 
 *    the right to distribute the software outside of your company.
 *  - "Licensed patents" means any Licensor patent claims which read directly on the software as 
 *    distributed by the Licensor under this license.
 * 
 * 2. Grant of Rights
 *  (A) Copyright Grant- Subject to the terms of this license, the Licensor grants you a non-transferable, 
 *      non-exclusive, worldwide, royalty-free copyright license to reproduce the software for reference use.
 *  (B) Patent Grant- Subject to the terms of this license, the Licensor grants you a non-transferable,
 *      non-exclusive, worldwide, royalty-free patent license under licensed patents for reference use.
 * 
 * 3. Limitations
 *  (A) No Trademark License - This license does not grant you any rights to use the Licensor's name
 *      logo, or trademarks.
 *  (B) If you begin patent litigation against the Licensor over patents that you think may apply 
 *      to the software (including a cross-claim or counterclaim in a lawsuit), your license to the 
 *      software ends automatically.
 *  (C) The software is licensed "as-is." You bear the risk of using it. The Licensor gives no express 
 *      warranties, guarantees or conditions. You may have additional consumer rights under your local 
 *      laws which this license cannot change. To the extent permitted under your local laws, the 
 *      Licensor excludes the implied warranties of merchantability, fitness for a particular purpose 
 *      and non-infringement.
 *
 ***************************************************************************************************/




#include "CodeLibrary.hpp"

#if defined(USE_OPENGL)

#include "Graphics/OpenGL/OpenGLException.hpp"
#include "GL/gl.h"

using namespace Bolitho;
using namespace Bolitho::Graphics;
using namespace Bolitho::Graphics::OpenGL;

//----------------------------------------------------------------------------------------------------------------------------------------------------
ImplementRuntimeType(Bolitho::Graphics::OpenGL,OpenGLException,Exception);
//----------------------------------------------------------------------------------------------------------------------------------------------------
OpenGLException::OpenGLException(CONST Object* Source, LPCTSTR Description) : Exception(Source)
{
  m_Description = Description;
}
//----------------------------------------------------------------------------------------------------------------------------------------------------
OpenGLException::OpenGLException(CONST Object* Source, INT ErrorCode) : Exception(Source)
{
  switch (ErrorCode)
  {
  case GL_INVALID_ENUM:
    m_Description = TEXT("An unacceptable value is specified for an enumerated argument."); 
    break;
  case GL_INVALID_VALUE:
    m_Description = TEXT("A numeric argument is out of range."); 
    break;
  case GL_INVALID_OPERATION:
    m_Description = TEXT("The specified operation is not allowed in the current state.");
    break;
  case GL_STACK_OVERFLOW:
    m_Description = TEXT("This command would cause a stack overflow.");
    break;
  case GL_STACK_UNDERFLOW:
    m_Description = TEXT("This command would cause a stack underflow.");
    break;
  case GL_OUT_OF_MEMORY:
    m_Description = TEXT("There is not enough memory left to execute the command.");
    break;
  default:
    m_Description = String::Format(TEXT("Unknown OpenGL Error %d"), ErrorCode);
    break;
  };
}
//----------------------------------------------------------------------------------------------------------------------------------------------------




#endif