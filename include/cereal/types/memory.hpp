/*
  Copyright (c) 2013, Randolph Voorhies, Shane Grant
  All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:
      * Redistributions of source code must retain the above copyright
        notice, this list of conditions and the following disclaimer.
      * Redistributions in binary form must reproduce the above copyright
        notice, this list of conditions and the following disclaimer in the
        documentation and/or other materials provided with the distribution.
      * Neither the name of cereal nor the
        names of its contributors may be used to endorse or promote products
        derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#ifndef CEREAL_TYPES_SHARED_PTR_HPP_
#define CEREAL_TYPES_SHARED_PTR_HPP_

#include <cereal/cereal.hpp>
#include <memory>

namespace cereal
{
  //! Saving std::shared_ptr
  template <class Archive, class T> inline
  void save( Archive & ar, std::shared_ptr<T> const & ptr )
  {
    uint32_t id = ar.registerSharedPointer( ptr.get() );
    ar( id );

    if( id & msb_32bit )
    {
      ar( *ptr );
    }
  }

  //! Loading std::shared_ptr, case when user load and allocate
  template <class Archive, class T> inline
  typename std::enable_if<traits::has_load_and_allocate<T, BinaryInputArchive>(), void>::type
  load( Archive & ar, std::shared_ptr<T> & ptr )
  {
    uint32_t id;

    ar( id );

    if( id & msb_32bit )
    {
      ptr.reset( detail::Load<T, BinaryInputArchive>::load_andor_allocate( ar ) );
      ar.registerSharedPointer(id, ptr);
    }
    else
    {
      ptr = std::static_pointer_cast<T>(ar.getSharedPointer(id));
    }
  }

  //! Loading std::shared_ptr, case when no user load and allocate
  template <class Archive, class T> inline
  typename std::enable_if<!traits::has_load_and_allocate<T, BinaryInputArchive>(), void>::type
  load( Archive & ar, std::shared_ptr<T> & ptr )
  {
    uint32_t id;

    ar( id );

    if( id & msb_32bit )
    {
      ptr.reset( detail::Load<T, BinaryInputArchive>::load_andor_allocate( ar ) );
      ar( *ptr );
      ar.registerSharedPointer(id, ptr);
    }
    else
    {
      ptr = std::static_pointer_cast<T>(ar.getSharedPointer(id));
    }
  }

  //! Saving std::weak_ptr
  template <class T> inline
  void save( BinaryOutputArchive & ar, std::weak_ptr<T> const & ptr )
  {
    auto sptr = ptr.lock();
    ar( sptr );
  }

  //! Loading std::weak_ptr
  template <class T> inline
  void load( BinaryInputArchive & ar, std::weak_ptr<T> & ptr )
  {
    std::shared_ptr<T> sptr;
    ar( sptr );
    ptr = sptr;
  }

  //! Saving std::unique_ptr
  template <class T, class D> inline
  void save( BinaryOutputArchive & ar, std::unique_ptr<T, D> const & ptr )
  {
    ar( *ptr );
  }

  //! Loading std::unique_ptr, case when user provides load_and_allocate
  template <class Archive, class T, class D> inline
  typename std::enable_if<traits::has_load_and_allocate<T, BinaryInputArchive>(), void>::type
  load( BinaryInputArchive & ar, std::unique_ptr<T, D> & ptr )
  {
    ptr.reset( detail::Load<T, BinaryInputArchive>::load_andor_allocate( ar ) );
  }

  //! Loading std::unique_ptr, case when no load_and_allocate
  template <class Archive, class T, class D> inline
  typename std::enable_if<!traits::has_load_and_allocate<T, BinaryInputArchive>(), void>::type
  load( Archive & ar, std::unique_ptr<T, D> & ptr )
  {
    ptr.reset( detail::Load<T, BinaryInputArchive>::load_andor_allocate( ar ) );
    ar( *ptr );
  }

} // namespace cereal

#endif // CEREAL_TYPES_SHARED_PTR_HPP_