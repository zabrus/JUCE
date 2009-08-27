/*
  ==============================================================================

   This file is part of the JUCE library - "Jules' Utility Class Extensions"
   Copyright 2004-9 by Raw Material Software Ltd.

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

#ifndef __JUCE_BLOWFISH_JUCEHEADER__
#define __JUCE_BLOWFISH_JUCEHEADER__


//==============================================================================
/**
    BlowFish encryption class.

*/
class JUCE_API  BlowFish
{
public:
    //==============================================================================
    /** Creates an object that can encode/decode based on the specified key.

        The key data can be up to 72 bytes long.
    */
    BlowFish (const uint8* keyData, int keyBytes);

    /** Creates a copy of another blowfish object. */
    BlowFish (const BlowFish& other);

    /** Copies another blowfish object. */
    const BlowFish& operator= (const BlowFish& other);

    /** Destructor. */
    ~BlowFish();

    //==============================================================================
    /** Encrypts a pair of 32-bit integers. */
    void encrypt (uint32& data1, uint32& data2) const;

    /** Decrypts a pair of 32-bit integers. */
    void decrypt (uint32& data1, uint32& data2) const;


    //==============================================================================
    juce_UseDebuggingNewOperator

private:
    uint32 p[18];
    uint32* s[4];

    uint32 F (uint32 x) const;
};


#endif   // __JUCE_BLOWFISH_JUCEHEADER__