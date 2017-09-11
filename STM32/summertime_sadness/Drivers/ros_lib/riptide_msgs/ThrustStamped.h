#ifndef _ROS_riptide_msgs_ThrustStamped_h
#define _ROS_riptide_msgs_ThrustStamped_h

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "ros/msg.h"
#include "std_msgs/Header.h"
#include "riptide_msgs/Thrust.h"

namespace riptide_msgs
{

  class ThrustStamped : public ros::Msg
  {
    public:
      typedef std_msgs::Header _header_type;
      _header_type header;
      typedef riptide_msgs::Thrust _force_type;
      _force_type force;

    ThrustStamped():
      header(),
      force()
    {
    }

    virtual int serialize(unsigned char *outbuffer) const
    {
      int offset = 0;
      offset += this->header.serialize(outbuffer + offset);
      offset += this->force.serialize(outbuffer + offset);
      return offset;
    }

    virtual int deserialize(unsigned char *inbuffer)
    {
      int offset = 0;
      offset += this->header.deserialize(inbuffer + offset);
      offset += this->force.deserialize(inbuffer + offset);
     return offset;
    }

    const char * getType(){ return "riptide_msgs/ThrustStamped"; };
    const char * getMD5(){ return "45ff11326423ac08ffb32beb2a739476"; };

  };

}
#endif