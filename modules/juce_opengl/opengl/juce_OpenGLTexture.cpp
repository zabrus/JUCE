/*
  ==============================================================================

   This file is part of the JUCE library - "Jules' Utility Class Extensions"
   Copyright 2004-11 by Raw Material Software Ltd.

  ------------------------------------------------------------------------------

   JUCE can be redistributed and/or modified under the terms of the GNU General
   Public License (Version 2), as published by the Free Software Foundation.
   A copy of the license is included in the JUCE distribution, or can be found
   online at www.gnu.org/licenses.

   JUCE is distributed in the hope that it will be useful, but WITHOUT ANY
   WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
   A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

  ------------------------------------------------------------------------------

   To release a closed-source product which uses JUCE, commercial licenses are
   available: visit www.rawmaterialsoftware.com/juce for more information.

  ==============================================================================
*/

BEGIN_JUCE_NAMESPACE

#if JUCE_OPENGL_ES
 enum { internalGLTextureFormat = GL_RGBA };
#else
 enum { internalGLTextureFormat = 4 };
#endif


OpenGLTexture::OpenGLTexture()
    : textureID (0), width (0), height (0)
{
}

OpenGLTexture::~OpenGLTexture()
{
    release();
}

void OpenGLTexture::create (const int w, const int h)
{
    release();

    width  = w;
    height = h;

    jassert (BitArray (width).countNumberOfSetBits() == 1); // these dimensions must be a power-of-two
    jassert (BitArray (height).countNumberOfSetBits() == 1);

    glGenTextures (1, &textureID);
    glBindTexture (GL_TEXTURE_2D, textureID);

    glTexEnvf (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    glTexParameterf (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glPixelStorei (GL_UNPACK_ALIGNMENT, 4);
}

void OpenGLTexture::load (const Image& image)
{
    create (image.getWidth(), image.getHeight());

    Image::BitmapData srcData (image, Image::BitmapData::readOnly);

    glTexImage2D (GL_TEXTURE_2D, 0, internalGLTextureFormat, width, height, 0,
                  image.getFormat() == Image::RGB ? GL_RGB : GL_BGRA_EXT,
                  GL_UNSIGNED_BYTE, srcData.data);
}

void OpenGLTexture::load (const PixelARGB* const pixels, const int w, const int h)
{
    create (w, h);

    glTexImage2D (GL_TEXTURE_2D, 0, internalGLTextureFormat, w, h, 0,
                  GL_BGRA_EXT, GL_UNSIGNED_BYTE, pixels);
}

void OpenGLTexture::release()
{
    if (textureID != 0)
    {
        glDeleteTextures (1, &textureID);
        textureID = 0;
        width = 0;
        height = 0;
    }
}

void OpenGLTexture::bind() const
{
    glBindTexture (GL_TEXTURE_2D, textureID);
}

void OpenGLTexture::unbind() const
{
    glBindTexture (GL_TEXTURE_2D, 0);
}

void OpenGLTexture::draw2D (float x1, float y1,
                            float x2, float y2,
                            float x3, float y3,
                            float x4, float y4,
                            const Colour& colour) const
{
    bind();
    OpenGLHelpers::drawQuad2D (x1, y1, x2, y2, x3, y3, x4, y4, colour);
    unbind();
}

void OpenGLTexture::draw3D (float x1, float y1, float z1,
                            float x2, float y2, float z2,
                            float x3, float y3, float z3,
                            float x4, float y4, float z4,
                            const Colour& colour) const
{
    bind();
    OpenGLHelpers::drawQuad3D (x1, y1, z1, x2, y2, z2, x3, y3, z3, x4, y4, z4, colour);
    unbind();
}

END_JUCE_NAMESPACE