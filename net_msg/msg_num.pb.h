// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: msg_num.proto

#ifndef PROTOBUF_INCLUDED_msg_5fnum_2eproto
#define PROTOBUF_INCLUDED_msg_5fnum_2eproto

#include <string>

#include <google/protobuf/stubs/common.h>

#if GOOGLE_PROTOBUF_VERSION < 3006001
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please update
#error your headers.
#endif
#if 3006001 < GOOGLE_PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/arena.h>
#include <google/protobuf/arenastring.h>
#include <google/protobuf/generated_message_table_driven.h>
#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/inlined_string_field.h>
#include <google/protobuf/metadata.h>
#include <google/protobuf/repeated_field.h>  // IWYU pragma: export
#include <google/protobuf/extension_set.h>  // IWYU pragma: export
#include <google/protobuf/generated_enum_reflection.h>
// @@protoc_insertion_point(includes)
#define PROTOBUF_INTERNAL_EXPORT_protobuf_msg_5fnum_2eproto 

namespace protobuf_msg_5fnum_2eproto {
// Internal implementation detail -- do not use these members.
struct TableStruct {
  static const ::google::protobuf::internal::ParseTableField entries[];
  static const ::google::protobuf::internal::AuxillaryParseTableField aux[];
  static const ::google::protobuf::internal::ParseTable schema[1];
  static const ::google::protobuf::internal::FieldMetadata field_metadata[];
  static const ::google::protobuf::internal::SerializationTable serialization_table[];
  static const ::google::protobuf::uint32 offsets[];
};
void AddDescriptors();
}  // namespace protobuf_msg_5fnum_2eproto
namespace game {
}  // namespace game
namespace game {

enum MsgNum {
  eMsg_Begin = 0,
  eMsg_ReqLogin = 1,
  eMsg_ResLogin = 2,
  eMsg_ReqHeartBeat = 3,
  eMsg_ResHeartBeat = 4,
  eMsg_End = 999,
  MsgNum_INT_MIN_SENTINEL_DO_NOT_USE_ = ::google::protobuf::kint32min,
  MsgNum_INT_MAX_SENTINEL_DO_NOT_USE_ = ::google::protobuf::kint32max
};
bool MsgNum_IsValid(int value);
const MsgNum MsgNum_MIN = eMsg_Begin;
const MsgNum MsgNum_MAX = eMsg_End;
const int MsgNum_ARRAYSIZE = MsgNum_MAX + 1;

const ::google::protobuf::EnumDescriptor* MsgNum_descriptor();
inline const ::std::string& MsgNum_Name(MsgNum value) {
  return ::google::protobuf::internal::NameOfEnum(
    MsgNum_descriptor(), value);
}
inline bool MsgNum_Parse(
    const ::std::string& name, MsgNum* value) {
  return ::google::protobuf::internal::ParseNamedEnum<MsgNum>(
    MsgNum_descriptor(), name, value);
}
// ===================================================================


// ===================================================================


// ===================================================================

#ifdef __GNUC__
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif  // __GNUC__
#ifdef __GNUC__
  #pragma GCC diagnostic pop
#endif  // __GNUC__

// @@protoc_insertion_point(namespace_scope)

}  // namespace game

namespace google {
namespace protobuf {

template <> struct is_proto_enum< ::game::MsgNum> : ::std::true_type {};
template <>
inline const EnumDescriptor* GetEnumDescriptor< ::game::MsgNum>() {
  return ::game::MsgNum_descriptor();
}

}  // namespace protobuf
}  // namespace google

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_INCLUDED_msg_5fnum_2eproto
