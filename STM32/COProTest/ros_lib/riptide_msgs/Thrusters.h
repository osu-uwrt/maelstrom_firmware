#ifndef _ROS_riptide_msgs_Thrusters_h
#define _ROS_riptide_msgs_Thrusters_h

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "ros/msg.h"
#include "riptide_msgs/Esc.h"

namespace riptide_msgs
{

  class Thrusters : public ros::Msg
  {
    public:
      typedef riptide_msgs::Esc _surge_port_hi_type;
      _surge_port_hi_type surge_port_hi;
      typedef riptide_msgs::Esc _surge_stbd_hi_type;
      _surge_stbd_hi_type surge_stbd_hi;
      typedef riptide_msgs::Esc _surge_port_lo_type;
      _surge_port_lo_type surge_port_lo;
      typedef riptide_msgs::Esc _surge_stbd_lo_type;
      _surge_stbd_lo_type surge_stbd_lo;
      typedef riptide_msgs::Esc _sway_fwd_type;
      _sway_fwd_type sway_fwd;
      typedef riptide_msgs::Esc _sway_aft_type;
      _sway_aft_type sway_aft;
      typedef riptide_msgs::Esc _heave_port_fwd_type;
      _heave_port_fwd_type heave_port_fwd;
      typedef riptide_msgs::Esc _heave_stbd_fwd_type;
      _heave_stbd_fwd_type heave_stbd_fwd;
      typedef riptide_msgs::Esc _heave_port_aft_type;
      _heave_port_aft_type heave_port_aft;
      typedef riptide_msgs::Esc _heave_stbd_aft_type;
      _heave_stbd_aft_type heave_stbd_aft;

    Thrusters():
      surge_port_hi(),
      surge_stbd_hi(),
      surge_port_lo(),
      surge_stbd_lo(),
      sway_fwd(),
      sway_aft(),
      heave_port_fwd(),
      heave_stbd_fwd(),
      heave_port_aft(),
      heave_stbd_aft()
    {
    }

    virtual int serialize(unsigned char *outbuffer) const
    {
      int offset = 0;
      offset += this->surge_port_hi.serialize(outbuffer + offset);
      offset += this->surge_stbd_hi.serialize(outbuffer + offset);
      offset += this->surge_port_lo.serialize(outbuffer + offset);
      offset += this->surge_stbd_lo.serialize(outbuffer + offset);
      offset += this->sway_fwd.serialize(outbuffer + offset);
      offset += this->sway_aft.serialize(outbuffer + offset);
      offset += this->heave_port_fwd.serialize(outbuffer + offset);
      offset += this->heave_stbd_fwd.serialize(outbuffer + offset);
      offset += this->heave_port_aft.serialize(outbuffer + offset);
      offset += this->heave_stbd_aft.serialize(outbuffer + offset);
      return offset;
    }

    virtual int deserialize(unsigned char *inbuffer)
    {
      int offset = 0;
      offset += this->surge_port_hi.deserialize(inbuffer + offset);
      offset += this->surge_stbd_hi.deserialize(inbuffer + offset);
      offset += this->surge_port_lo.deserialize(inbuffer + offset);
      offset += this->surge_stbd_lo.deserialize(inbuffer + offset);
      offset += this->sway_fwd.deserialize(inbuffer + offset);
      offset += this->sway_aft.deserialize(inbuffer + offset);
      offset += this->heave_port_fwd.deserialize(inbuffer + offset);
      offset += this->heave_stbd_fwd.deserialize(inbuffer + offset);
      offset += this->heave_port_aft.deserialize(inbuffer + offset);
      offset += this->heave_stbd_aft.deserialize(inbuffer + offset);
     return offset;
    }

    const char * getType(){ return "riptide_msgs/Thrusters"; };
    const char * getMD5(){ return "241305c365c440c0e95be78cdd3d7519"; };

  };

}
#endif