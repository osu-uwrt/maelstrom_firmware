#ifndef _ROS_riptide_msgs_BatStamped_h
#define _ROS_riptide_msgs_BatStamped_h

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "ros/msg.h"
#include "std_msgs/Header.h"
#include "riptide_msgs/Bat.h"

namespace riptide_msgs
{

  class BatStamped : public ros::Msg
  {
    public:
      typedef std_msgs::Header _header_type;
      _header_type header;
      typedef riptide_msgs::Bat _port_type;
      _port_type port;
      typedef riptide_msgs::Bat _stbd_type;
      _stbd_type stbd;

    BatStamped():
      header(),
      port(),
      stbd()
    {
    }

    virtual int serialize(unsigned char *outbuffer) const
    {
      int offset = 0;
      offset += this->header.serialize(outbuffer + offset);
      offset += this->port.serialize(outbuffer + offset);
      offset += this->stbd.serialize(outbuffer + offset);
      return offset;
    }

    virtual int deserialize(unsigned char *inbuffer)
    {
      int offset = 0;
      offset += this->header.deserialize(inbuffer + offset);
      offset += this->port.deserialize(inbuffer + offset);
      offset += this->stbd.deserialize(inbuffer + offset);
     return offset;
    }

    const char * getType(){ return "riptide_msgs/BatStamped"; };
    const char * getMD5(){ return "00b3661bfda801ef1ed520e7d4bfb3ef"; };

  };

}
#endif