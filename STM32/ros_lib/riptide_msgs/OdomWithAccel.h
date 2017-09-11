#ifndef _ROS_riptide_msgs_OdomWithAccel_h
#define _ROS_riptide_msgs_OdomWithAccel_h

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "ros/msg.h"
#include "std_msgs/Header.h"
#include "geometry_msgs/Pose.h"
#include "geometry_msgs/Twist.h"
#include "geometry_msgs/Accel.h"

namespace riptide_msgs
{

  class OdomWithAccel : public ros::Msg
  {
    public:
      typedef std_msgs::Header _header_type;
      _header_type header;
      typedef geometry_msgs::Pose _pose_type;
      _pose_type pose;
      typedef geometry_msgs::Twist _twist_type;
      _twist_type twist;
      typedef geometry_msgs::Accel _accel_type;
      _accel_type accel;

    OdomWithAccel():
      header(),
      pose(),
      twist(),
      accel()
    {
    }

    virtual int serialize(unsigned char *outbuffer) const
    {
      int offset = 0;
      offset += this->header.serialize(outbuffer + offset);
      offset += this->pose.serialize(outbuffer + offset);
      offset += this->twist.serialize(outbuffer + offset);
      offset += this->accel.serialize(outbuffer + offset);
      return offset;
    }

    virtual int deserialize(unsigned char *inbuffer)
    {
      int offset = 0;
      offset += this->header.deserialize(inbuffer + offset);
      offset += this->pose.deserialize(inbuffer + offset);
      offset += this->twist.deserialize(inbuffer + offset);
      offset += this->accel.deserialize(inbuffer + offset);
     return offset;
    }

    const char * getType(){ return "riptide_msgs/OdomWithAccel"; };
    const char * getMD5(){ return "f66648d1c57928da53a1cb39d1189cb5"; };

  };

}
#endif