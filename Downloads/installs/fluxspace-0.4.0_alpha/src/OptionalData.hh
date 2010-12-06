//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
// OptionalData.hh
//
//  Fluxspace is a workspace and slit manager for fluxbox
//
//  Copyright (c) 2002 Steve Cooper, stevencooper@isomedia.com
//  Copyright (c) 1998-2000 by John Kennis, jkennis@chello.nl
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
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

#ifndef __OPTIONALDATA_HH
#define __OPTIONALDATA_HH

// T1 is the incoming type, e.g. "const char*"
// T2 is the storage type, e.g. "std::string"
// T2 must support copy semantics and conversion from T1.
template<class T1, class T2> class OptionalData
{
  public:
    OptionalData<T1, T2>()
      : isSet(false)
    {
    }

    OptionalData<T1, T2>(const OptionalData<T1, T2>& src)
    {
        Copy(src);
    }

    const OptionalData<T1, T2>& operator=(const OptionalData<T1, T2>& src)
    {
        Copy(src);
        return *this;
    }

    void Copy(const OptionalData<T1, T2>& src)
    {
        isSet = src.isSet;
        value = src.value;
    }

    void Set(T1 value_)
    {
        value = value_;
        isSet = true;
    }

    T2 Get(T1 defValue)
    {
        return (isSet ? value : defValue);
    }

    void Clear()
    {
        isSet = false;
    }

    bool isSet;
    T2   value;
};

typedef OptionalData<int, int>                     OptionalInteger;
typedef OptionalData<unsigned int, unsigned int>   OptionalUnsignedInteger;
typedef OptionalData<long, long>                   OptionalLong;
typedef OptionalData<unsigned long, unsigned long> OptionalUnsignedLong;
typedef OptionalData<bool, bool>                   OptionalBoolean;
typedef OptionalUnsignedLong                       OptionalColor;
typedef OptionalData<const char*, std::string>     OptionalString;

#endif // __OPTIONALDATA_HH
