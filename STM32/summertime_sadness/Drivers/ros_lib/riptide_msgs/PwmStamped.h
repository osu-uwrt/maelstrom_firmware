#ifndef _ROS_riptide_msgs_PwmStamped_h
#define _ROS_riptide_msgs_PwmStamped_h

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "ros/msg.h"
#include "std_msgs/Header.h"
#include "riptide_msgs/Pwm.h"

namespace riptide_msgs
{

  class PwmStamped : public ros::Msg
  {
    public:
      typedef std_msgs::Header _header_type;
      _header_type header;
      typedef riptide_msgs::Pwm _pwm_type;
      _pwm_type pwm;

    PwmStamped():
      header(),
      pwm()
    {
    }

    virtual int serialize(unsigned char *outbuffer) const
    {
      int offset = 0;
      offset += this->header.serialize(outbuffer + offset);
      offset += this->pwm.serialize(outbuffer + offset);
      return offset;
    }

    virtual int deserialize(unsigned char *inbuffer)
    {
      int offset = 0;
      offset += this->header.deserialize(inbuffer + offset);
      offset += this->pwm.deserialize(inbuffer + offset);
     return offset;
    }

    const char * getType(){ return "riptide_msgs/PwmStamped"; };
    const char * getMD5(){ return "f9e5559e7373650dd0fb7382c0532912"; };

  };

}
#endif