/* (c) Copyright 2011-2014 Felipe Magno de Almeida
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef GHTV_QT_JAVA_XLET_CLASS_LOADER_HPP
#define GHTV_QT_JAVA_XLET_CLASS_LOADER_HPP

#include <jvb/adapt_class.hpp>

namespace ghtv { namespace qt { namespace java {

struct xlet_class_loader
{
  xlet_class_loader(JNIEnv* env)
  {
  }

  // javabind::object load_class(JNIEnv* env, javabind::string name, bool resolve)
  // {
  //   env->EnsureLocalCapacity(100);
  //   std::cout <<  "load_class " << name << std::endl;

  //   assert(!env->ExceptionCheck());

  //   javabind::env e(env);

  //   javabind::string class_string
  //     (env->NewStringUTF("java/io/FileInputStream.class"), env);
  //   javabind::string class_name
  //     (env->NewStringUTF("java/io/FileInputStream"), env);
  //   javabind::string source_name
  //     (env->NewStringUTF("FileInputStream.java"), env);

  //   assert(!env->ExceptionCheck());

  //   javabind::class_ resource_class = e.find_class("sun/misc/Resource");
  //   javabind::class_ ucp_class = e.find_class("sun/misc/URLClassPath");
  //   javabind::class_ url_class_loader_class = e.find_class("java/net/URLClassLoader");
  //   javabind::class_ class_loader_class = e.find_class("java/lang/ClassLoader");
  //   javabind::class_ byte_buffer_class = e.find_class("java/nio/ByteBuffer");

  //   assert(!env->ExceptionCheck());
    
  //   javabind::field<javabind::object> ucp_field
  //     = url_class_loader_class.find_field<javabind::object>("ucp", "Lsun/misc/URLClassPath;");

  //   assert(!env->ExceptionCheck());

  //   javabind::object ucp = ucp_field.get(self(env));
  //   assert(!!ucp);

  //   assert(!env->ExceptionCheck());

  //   javabind::method<javabind::object(javabind::string, bool)>
  //     get_resource = ucp_class.method<javabind::object(javabind::string, bool)>
  //     ("getResource", "sun/misc/Resource");

  //   assert(!env->ExceptionCheck());

  //   javabind::object res = get_resource(ucp, class_string, false);
  //   assert(!!res);

  //   assert(!env->ExceptionCheck());

  //   javabind::method<javabind::object()> get_byte_buffer = resource_class
  //     .method<javabind::object()>("getByteBuffer", "java/nio/ByteBuffer");

  //   assert(!env->ExceptionCheck());

  //   javabind::object bb;
  //   try
  //   {
  //     bb = get_byte_buffer(res);
  //   }
  //   catch(std::exception& e)
  //   {
  //     std::cout << "Exception was thrown while getByteBuffer" << std::endl;
  //     env->ExceptionDescribe();
  //     return javabind::object(0, env);
  //   }

  //   assert(!env->ExceptionCheck());

  //   javabind::object class_;
  //   if(bb)
  //   {
  //   javabind::method<javabind::int_()> position = byte_buffer_class.method
  //     <javabind::int_()>("position");
  //   assert(!env->ExceptionCheck());
  //   javabind::method<javabind::int_()> remaining = byte_buffer_class.method
  //     <javabind::int_()>("remaining");
  //   assert(!env->ExceptionCheck());
  //   javabind::int_ pos = position(bb)
  //     , remain = remaining(bb);
  //   assert(!env->ExceptionCheck());

  //   javabind::method<javabind::object(javabind::string, javabind::object
  //                                     , javabind::int_, javabind::int_
  //                                     , javabind::object, javabind::string)>
  //   define_class = class_loader_class
  //     .method<javabind::object(javabind::string, javabind::object
  //                              , javabind::int_, javabind::int_
  //                              , javabind::object, javabind::string)>
  //   ("defineClass2", javabind::descriptors("java/lang/Class", "java/nio/ByteBuffer"
  //                                          , "java/security/ProtectionDomain"));
  //   assert(!env->ExceptionCheck());

  //   class_ = define_class(self(env), class_name, bb, pos, remain
  //                         , javabind::object(0, env), source_name);
  //   assert(!env->ExceptionCheck());
  //   assert(!!class_);
  //   }
  //   else
  //   {
  //     assert(!env->ExceptionCheck());
  //     javabind::method<javabind::array<javabind::byte>()> get_bytes = resource_class
  //       .method<javabind::array<javabind::byte>()>("getBytes");
  //     assert(!env->ExceptionCheck());
  //     javabind::array<javabind::byte> bytes = get_bytes(res);
  //     assert(!!bytes);
  //     assert(!env->ExceptionCheck());

  //     std::size_t length = bytes.length();
  //     assert(length != 0);
  //     assert(!env->ExceptionCheck());

  //     javabind::method<javabind::array<javabind::object>()>
  //       get_code_signers = resource_class.method<javabind::array<javabind::object>()>
  //       ("getCodeSigners", "java/security/CodeSigner");
  //     assert(!env->ExceptionCheck());
  //     javabind::array<javabind::object> signers = get_code_signers(res);
  //     assert(!env->ExceptionCheck());

  //     // javabind::class_ code_signer_class = e.find_class("java/security/CodeSigner");
  //     // javabind::array<javabind::object> signers
  //     //   (env->NewObjectArray(1, code_signer_class.raw(), 0), env);
  //     // assert(!!signers);
  //     // assert(!env->ExceptionCheck());

  //     javabind::method<javabind::object()> get_url = resource_class
  //       .method<javabind::object()>("getCodeSourceURL", "java/net/URL");
  //     assert(!env->ExceptionCheck());
  //     javabind::object url = get_url(res);
  //     assert(!env->ExceptionCheck());
  //     assert(!!url);
      
  //     std::cout << "url: " << url << std::endl;
      
  //     javabind::class_ code_source_class = e.find_class("java/security/CodeSource");
  //     assert(!env->ExceptionCheck());
  //     javabind::object code_source = code_source_class.constructor
  //       <javabind::object(javabind::object, javabind::array<javabind::object>)>
  //       (javabind::descriptors("java/net/URL", "java/security/CodeSigner"))
  //       (code_source_class, url, /*javabind::object(0, env)*/signers);
  //     assert(!env->ExceptionCheck());
  //     assert(!!code_source);

  //     javabind::class_ permissions_class = e.find_class("java/security/Permissions");
  //     javabind::object permissions = permissions_class.constructor<javabind::object()>
  //       ()(permissions_class);
  //     assert(!!permissions);

  //     javabind::class_ protection_domain_class = e.find_class("java/security/ProtectionDomain");
  //     javabind::object protection_domain = protection_domain_class
  //       .constructor<javabind::object(javabind::object, javabind::object
  //                                     , javabind::object
  //                                     , javabind::array<javabind::object>)>
  //       (javabind::descriptors("java/security/CodeSource", "java/security/PermissionCollection"
  //                              , "java/lang/ClassLoader", "java/security/Principal"))
  //       (protection_domain_class, code_source, permissions, self(env)
  //        , javabind::array<javabind::object>() /*principals*/);
  //     assert(!!protection_domain);

  //     javabind::method<javabind::object(javabind::string, javabind::array<javabind::byte>
  //                                       , javabind::int_, javabind::int_
  //                                       , javabind::object, javabind::string)>
  //       define_class = class_loader_class
  //       .method<javabind::object(javabind::string, javabind::array<javabind::byte>
  //                                , javabind::int_, javabind::int_
  //                                , javabind::object, javabind::string)>
  //     ("defineClass1", javabind::descriptors("java/lang/Class", "java/security/ProtectionDomain"));
  //     class_ = define_class(self(env), class_name, bytes, 0, length
  //                           , protection_domain, source_name);
  //     assert(!!class_);

  //   }

  //   std::cout << "returning" << std::endl;
  //   return class_;
  // }
};

} } }

#endif
