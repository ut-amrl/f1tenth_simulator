/* Auto-generated by genmsg_cpp for file /home/jaholtz/code/cobot_simulator/cobot_msgs/msg/AckermanDriveMsg.msg */
#ifndef COBOT_MSGS_MESSAGE_ACKERMANDRIVEMSG_H
#define COBOT_MSGS_MESSAGE_ACKERMANDRIVEMSG_H
#include <string>
#include <vector>
#include <map>
#include <ostream>
#include "ros/serialization.h"
#include "ros/builtin_message_traits.h"
#include "ros/message_operations.h"
#include "ros/time.h"

#include "ros/macros.h"

#include "ros/assert.h"

#include "std_msgs/Header.h"

namespace cobot_msgs
{
template <class ContainerAllocator>
struct AckermanDriveMsg_ {
  typedef AckermanDriveMsg_<ContainerAllocator> Type;

  AckermanDriveMsg_()
  : header()
  , v(0.0)
  , R(0.0)
  {
  }

  AckermanDriveMsg_(const ContainerAllocator& _alloc)
  : header(_alloc)
  , v(0.0)
  , R(0.0)
  {
  }

  typedef  ::std_msgs::Header_<ContainerAllocator>  _header_type;
   ::std_msgs::Header_<ContainerAllocator>  header;

  typedef float _v_type;
  float v;

  typedef float _R_type;
  float R;


  typedef boost::shared_ptr< ::cobot_msgs::AckermanDriveMsg_<ContainerAllocator> > Ptr;
  typedef boost::shared_ptr< ::cobot_msgs::AckermanDriveMsg_<ContainerAllocator>  const> ConstPtr;
}; // struct AckermanDriveMsg
typedef  ::cobot_msgs::AckermanDriveMsg_<std::allocator<void> > AckermanDriveMsg;

typedef boost::shared_ptr< ::cobot_msgs::AckermanDriveMsg> AckermanDriveMsgPtr;
typedef boost::shared_ptr< ::cobot_msgs::AckermanDriveMsg const> AckermanDriveMsgConstPtr;


template<typename ContainerAllocator>
std::ostream& operator<<(std::ostream& s, const  ::cobot_msgs::AckermanDriveMsg_<ContainerAllocator> & v)
{
  ros::message_operations::Printer< ::cobot_msgs::AckermanDriveMsg_<ContainerAllocator> >::stream(s, "", v);
  return s;}

} // namespace cobot_msgs

namespace ros
{
namespace message_traits
{
template<class ContainerAllocator> struct IsMessage< ::cobot_msgs::AckermanDriveMsg_<ContainerAllocator> > : public TrueType {};
template<class ContainerAllocator> struct IsMessage< ::cobot_msgs::AckermanDriveMsg_<ContainerAllocator>  const> : public TrueType {};
template<class ContainerAllocator>
struct MD5Sum< ::cobot_msgs::AckermanDriveMsg_<ContainerAllocator> > {
  static const char* value() 
  {
    return "b46e25bb9c7b4a4f6d687cffd13da65c";
  }

  static const char* value(const  ::cobot_msgs::AckermanDriveMsg_<ContainerAllocator> &) { return value(); } 
  static const uint64_t static_value1 = 0xb46e25bb9c7b4a4fULL;
  static const uint64_t static_value2 = 0x6d687cffd13da65cULL;
};

template<class ContainerAllocator>
struct DataType< ::cobot_msgs::AckermanDriveMsg_<ContainerAllocator> > {
  static const char* value() 
  {
    return "cobot_msgs/AckermanDriveMsg";
  }

  static const char* value(const  ::cobot_msgs::AckermanDriveMsg_<ContainerAllocator> &) { return value(); } 
};

template<class ContainerAllocator>
struct Definition< ::cobot_msgs::AckermanDriveMsg_<ContainerAllocator> > {
  static const char* value() 
  {
    return "Header header\n\
\n\
# linear velocity command in the forward direction [m/s]\n\
float32 v\n\
\n\
# Inverse Turning Radius [m]\n\
float32 R\n\
\n\
================================================================================\n\
MSG: std_msgs/Header\n\
# Standard metadata for higher-level stamped data types.\n\
# This is generally used to communicate timestamped data \n\
# in a particular coordinate frame.\n\
# \n\
# sequence ID: consecutively increasing ID \n\
uint32 seq\n\
#Two-integer timestamp that is expressed as:\n\
# * stamp.sec: seconds (stamp_secs) since epoch (in Python the variable is called 'secs')\n\
# * stamp.nsec: nanoseconds since stamp_secs (in Python the variable is called 'nsecs')\n\
# time-handling sugar is provided by the client library\n\
time stamp\n\
#Frame this data is associated with\n\
# 0: no frame\n\
# 1: global frame\n\
string frame_id\n\
\n\
";
  }

  static const char* value(const  ::cobot_msgs::AckermanDriveMsg_<ContainerAllocator> &) { return value(); } 
};

template<class ContainerAllocator> struct HasHeader< ::cobot_msgs::AckermanDriveMsg_<ContainerAllocator> > : public TrueType {};
template<class ContainerAllocator> struct HasHeader< const ::cobot_msgs::AckermanDriveMsg_<ContainerAllocator> > : public TrueType {};
} // namespace message_traits
} // namespace ros

namespace ros
{
namespace serialization
{

template<class ContainerAllocator> struct Serializer< ::cobot_msgs::AckermanDriveMsg_<ContainerAllocator> >
{
  template<typename Stream, typename T> inline static void allInOne(Stream& stream, T m)
  {
    stream.next(m.header);
    stream.next(m.v);
    stream.next(m.R);
  }

  ROS_DECLARE_ALLINONE_SERIALIZER
}; // struct AckermanDriveMsg_
} // namespace serialization
} // namespace ros

namespace ros
{
namespace message_operations
{

template<class ContainerAllocator>
struct Printer< ::cobot_msgs::AckermanDriveMsg_<ContainerAllocator> >
{
  template<typename Stream> static void stream(Stream& s, const std::string& indent, const  ::cobot_msgs::AckermanDriveMsg_<ContainerAllocator> & v) 
  {
    s << indent << "header: ";
s << std::endl;
    Printer< ::std_msgs::Header_<ContainerAllocator> >::stream(s, indent + "  ", v.header);
    s << indent << "v: ";
    Printer<float>::stream(s, indent + "  ", v.v);
    s << indent << "R: ";
    Printer<float>::stream(s, indent + "  ", v.R);
  }
};


} // namespace message_operations
} // namespace ros

#endif // COBOT_MSGS_MESSAGE_ACKERMANDRIVEMSG_H

