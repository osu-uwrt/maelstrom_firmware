#ifndef _ROS_riptide_msgs_Thrust_h
#define _ROS_riptide_msgs_Thrust_h

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "ros/msg.h"

namespace riptide_msgs
{

  class Thrust : public ros::Msg
  {
    public:
      typedef float _surge_port_hi_type;
      _surge_port_hi_type surge_port_hi;
      typedef float _surge_stbd_hi_type;
      _surge_stbd_hi_type surge_stbd_hi;
      typedef float _surge_port_lo_type;
      _surge_port_lo_type surge_port_lo;
      typedef float _surge_stbd_lo_type;
      _surge_stbd_lo_type surge_stbd_lo;
      typedef float _sway_fwd_type;
      _sway_fwd_type sway_fwd;
      typedef float _sway_aft_type;
      _sway_aft_type sway_aft;
      typedef float _heave_port_fwd_type;
      _heave_port_fwd_type heave_port_fwd;
      typedef float _heave_stbd_fwd_type;
      _heave_stbd_fwd_type heave_stbd_fwd;
      typedef float _heave_port_aft_type;
      _heave_port_aft_type heave_port_aft;
      typedef float _heave_stbd_aft_type;
      _heave_stbd_aft_type heave_stbd_aft;

    Thrust():
      surge_port_hi(0),
      surge_stbd_hi(0),
      surge_port_lo(0),
      surge_stbd_lo(0),
      sway_fwd(0),
      sway_aft(0),
      heave_port_fwd(0),
      heave_stbd_fwd(0),
      heave_port_aft(0),
      heave_stbd_aft(0)
    {
    }

    virtual int serialize(unsigned char *outbuffer) const
    {
      int offset = 0;
      union {
        float real;
        uint32_t base;
      } u_surge_port_hi;
      u_surge_port_hi.real = this->surge_port_hi;
      *(outbuffer + offset + 0) = (u_surge_port_hi.base >> (8 * 0)) & 0xFF;
      *(outbuffer + offset + 1) = (u_surge_port_hi.base >> (8 * 1)) & 0xFF;
      *(outbuffer + offset + 2) = (u_surge_port_hi.base >> (8 * 2)) & 0xFF;
      *(outbuffer + offset + 3) = (u_surge_port_hi.base >> (8 * 3)) & 0xFF;
      offset += sizeof(this->surge_port_hi);
      union {
        float real;
        uint32_t base;
      } u_surge_stbd_hi;
      u_surge_stbd_hi.real = this->surge_stbd_hi;
      *(outbuffer + offset + 0) = (u_surge_stbd_hi.base >> (8 * 0)) & 0xFF;
      *(outbuffer + offset + 1) = (u_surge_stbd_hi.base >> (8 * 1)) & 0xFF;
      *(outbuffer + offset + 2) = (u_surge_stbd_hi.base >> (8 * 2)) & 0xFF;
      *(outbuffer + offset + 3) = (u_surge_stbd_hi.base >> (8 * 3)) & 0xFF;
      offset += sizeof(this->surge_stbd_hi);
      union {
        float real;
        uint32_t base;
      } u_surge_port_lo;
      u_surge_port_lo.real = this->surge_port_lo;
      *(outbuffer + offset + 0) = (u_surge_port_lo.base >> (8 * 0)) & 0xFF;
      *(outbuffer + offset + 1) = (u_surge_port_lo.base >> (8 * 1)) & 0xFF;
      *(outbuffer + offset + 2) = (u_surge_port_lo.base >> (8 * 2)) & 0xFF;
      *(outbuffer + offset + 3) = (u_surge_port_lo.base >> (8 * 3)) & 0xFF;
      offset += sizeof(this->surge_port_lo);
      union {
        float real;
        uint32_t base;
      } u_surge_stbd_lo;
      u_surge_stbd_lo.real = this->surge_stbd_lo;
      *(outbuffer + offset + 0) = (u_surge_stbd_lo.base >> (8 * 0)) & 0xFF;
      *(outbuffer + offset + 1) = (u_surge_stbd_lo.base >> (8 * 1)) & 0xFF;
      *(outbuffer + offset + 2) = (u_surge_stbd_lo.base >> (8 * 2)) & 0xFF;
      *(outbuffer + offset + 3) = (u_surge_stbd_lo.base >> (8 * 3)) & 0xFF;
      offset += sizeof(this->surge_stbd_lo);
      union {
        float real;
        uint32_t base;
      } u_sway_fwd;
      u_sway_fwd.real = this->sway_fwd;
      *(outbuffer + offset + 0) = (u_sway_fwd.base >> (8 * 0)) & 0xFF;
      *(outbuffer + offset + 1) = (u_sway_fwd.base >> (8 * 1)) & 0xFF;
      *(outbuffer + offset + 2) = (u_sway_fwd.base >> (8 * 2)) & 0xFF;
      *(outbuffer + offset + 3) = (u_sway_fwd.base >> (8 * 3)) & 0xFF;
      offset += sizeof(this->sway_fwd);
      union {
        float real;
        uint32_t base;
      } u_sway_aft;
      u_sway_aft.real = this->sway_aft;
      *(outbuffer + offset + 0) = (u_sway_aft.base >> (8 * 0)) & 0xFF;
      *(outbuffer + offset + 1) = (u_sway_aft.base >> (8 * 1)) & 0xFF;
      *(outbuffer + offset + 2) = (u_sway_aft.base >> (8 * 2)) & 0xFF;
      *(outbuffer + offset + 3) = (u_sway_aft.base >> (8 * 3)) & 0xFF;
      offset += sizeof(this->sway_aft);
      union {
        float real;
        uint32_t base;
      } u_heave_port_fwd;
      u_heave_port_fwd.real = this->heave_port_fwd;
      *(outbuffer + offset + 0) = (u_heave_port_fwd.base >> (8 * 0)) & 0xFF;
      *(outbuffer + offset + 1) = (u_heave_port_fwd.base >> (8 * 1)) & 0xFF;
      *(outbuffer + offset + 2) = (u_heave_port_fwd.base >> (8 * 2)) & 0xFF;
      *(outbuffer + offset + 3) = (u_heave_port_fwd.base >> (8 * 3)) & 0xFF;
      offset += sizeof(this->heave_port_fwd);
      union {
        float real;
        uint32_t base;
      } u_heave_stbd_fwd;
      u_heave_stbd_fwd.real = this->heave_stbd_fwd;
      *(outbuffer + offset + 0) = (u_heave_stbd_fwd.base >> (8 * 0)) & 0xFF;
      *(outbuffer + offset + 1) = (u_heave_stbd_fwd.base >> (8 * 1)) & 0xFF;
      *(outbuffer + offset + 2) = (u_heave_stbd_fwd.base >> (8 * 2)) & 0xFF;
      *(outbuffer + offset + 3) = (u_heave_stbd_fwd.base >> (8 * 3)) & 0xFF;
      offset += sizeof(this->heave_stbd_fwd);
      union {
        float real;
        uint32_t base;
      } u_heave_port_aft;
      u_heave_port_aft.real = this->heave_port_aft;
      *(outbuffer + offset + 0) = (u_heave_port_aft.base >> (8 * 0)) & 0xFF;
      *(outbuffer + offset + 1) = (u_heave_port_aft.base >> (8 * 1)) & 0xFF;
      *(outbuffer + offset + 2) = (u_heave_port_aft.base >> (8 * 2)) & 0xFF;
      *(outbuffer + offset + 3) = (u_heave_port_aft.base >> (8 * 3)) & 0xFF;
      offset += sizeof(this->heave_port_aft);
      union {
        float real;
        uint32_t base;
      } u_heave_stbd_aft;
      u_heave_stbd_aft.real = this->heave_stbd_aft;
      *(outbuffer + offset + 0) = (u_heave_stbd_aft.base >> (8 * 0)) & 0xFF;
      *(outbuffer + offset + 1) = (u_heave_stbd_aft.base >> (8 * 1)) & 0xFF;
      *(outbuffer + offset + 2) = (u_heave_stbd_aft.base >> (8 * 2)) & 0xFF;
      *(outbuffer + offset + 3) = (u_heave_stbd_aft.base >> (8 * 3)) & 0xFF;
      offset += sizeof(this->heave_stbd_aft);
      return offset;
    }

    virtual int deserialize(unsigned char *inbuffer)
    {
      int offset = 0;
      union {
        float real;
        uint32_t base;
      } u_surge_port_hi;
      u_surge_port_hi.base = 0;
      u_surge_port_hi.base |= ((uint32_t) (*(inbuffer + offset + 0))) << (8 * 0);
      u_surge_port_hi.base |= ((uint32_t) (*(inbuffer + offset + 1))) << (8 * 1);
      u_surge_port_hi.base |= ((uint32_t) (*(inbuffer + offset + 2))) << (8 * 2);
      u_surge_port_hi.base |= ((uint32_t) (*(inbuffer + offset + 3))) << (8 * 3);
      this->surge_port_hi = u_surge_port_hi.real;
      offset += sizeof(this->surge_port_hi);
      union {
        float real;
        uint32_t base;
      } u_surge_stbd_hi;
      u_surge_stbd_hi.base = 0;
      u_surge_stbd_hi.base |= ((uint32_t) (*(inbuffer + offset + 0))) << (8 * 0);
      u_surge_stbd_hi.base |= ((uint32_t) (*(inbuffer + offset + 1))) << (8 * 1);
      u_surge_stbd_hi.base |= ((uint32_t) (*(inbuffer + offset + 2))) << (8 * 2);
      u_surge_stbd_hi.base |= ((uint32_t) (*(inbuffer + offset + 3))) << (8 * 3);
      this->surge_stbd_hi = u_surge_stbd_hi.real;
      offset += sizeof(this->surge_stbd_hi);
      union {
        float real;
        uint32_t base;
      } u_surge_port_lo;
      u_surge_port_lo.base = 0;
      u_surge_port_lo.base |= ((uint32_t) (*(inbuffer + offset + 0))) << (8 * 0);
      u_surge_port_lo.base |= ((uint32_t) (*(inbuffer + offset + 1))) << (8 * 1);
      u_surge_port_lo.base |= ((uint32_t) (*(inbuffer + offset + 2))) << (8 * 2);
      u_surge_port_lo.base |= ((uint32_t) (*(inbuffer + offset + 3))) << (8 * 3);
      this->surge_port_lo = u_surge_port_lo.real;
      offset += sizeof(this->surge_port_lo);
      union {
        float real;
        uint32_t base;
      } u_surge_stbd_lo;
      u_surge_stbd_lo.base = 0;
      u_surge_stbd_lo.base |= ((uint32_t) (*(inbuffer + offset + 0))) << (8 * 0);
      u_surge_stbd_lo.base |= ((uint32_t) (*(inbuffer + offset + 1))) << (8 * 1);
      u_surge_stbd_lo.base |= ((uint32_t) (*(inbuffer + offset + 2))) << (8 * 2);
      u_surge_stbd_lo.base |= ((uint32_t) (*(inbuffer + offset + 3))) << (8 * 3);
      this->surge_stbd_lo = u_surge_stbd_lo.real;
      offset += sizeof(this->surge_stbd_lo);
      union {
        float real;
        uint32_t base;
      } u_sway_fwd;
      u_sway_fwd.base = 0;
      u_sway_fwd.base |= ((uint32_t) (*(inbuffer + offset + 0))) << (8 * 0);
      u_sway_fwd.base |= ((uint32_t) (*(inbuffer + offset + 1))) << (8 * 1);
      u_sway_fwd.base |= ((uint32_t) (*(inbuffer + offset + 2))) << (8 * 2);
      u_sway_fwd.base |= ((uint32_t) (*(inbuffer + offset + 3))) << (8 * 3);
      this->sway_fwd = u_sway_fwd.real;
      offset += sizeof(this->sway_fwd);
      union {
        float real;
        uint32_t base;
      } u_sway_aft;
      u_sway_aft.base = 0;
      u_sway_aft.base |= ((uint32_t) (*(inbuffer + offset + 0))) << (8 * 0);
      u_sway_aft.base |= ((uint32_t) (*(inbuffer + offset + 1))) << (8 * 1);
      u_sway_aft.base |= ((uint32_t) (*(inbuffer + offset + 2))) << (8 * 2);
      u_sway_aft.base |= ((uint32_t) (*(inbuffer + offset + 3))) << (8 * 3);
      this->sway_aft = u_sway_aft.real;
      offset += sizeof(this->sway_aft);
      union {
        float real;
        uint32_t base;
      } u_heave_port_fwd;
      u_heave_port_fwd.base = 0;
      u_heave_port_fwd.base |= ((uint32_t) (*(inbuffer + offset + 0))) << (8 * 0);
      u_heave_port_fwd.base |= ((uint32_t) (*(inbuffer + offset + 1))) << (8 * 1);
      u_heave_port_fwd.base |= ((uint32_t) (*(inbuffer + offset + 2))) << (8 * 2);
      u_heave_port_fwd.base |= ((uint32_t) (*(inbuffer + offset + 3))) << (8 * 3);
      this->heave_port_fwd = u_heave_port_fwd.real;
      offset += sizeof(this->heave_port_fwd);
      union {
        float real;
        uint32_t base;
      } u_heave_stbd_fwd;
      u_heave_stbd_fwd.base = 0;
      u_heave_stbd_fwd.base |= ((uint32_t) (*(inbuffer + offset + 0))) << (8 * 0);
      u_heave_stbd_fwd.base |= ((uint32_t) (*(inbuffer + offset + 1))) << (8 * 1);
      u_heave_stbd_fwd.base |= ((uint32_t) (*(inbuffer + offset + 2))) << (8 * 2);
      u_heave_stbd_fwd.base |= ((uint32_t) (*(inbuffer + offset + 3))) << (8 * 3);
      this->heave_stbd_fwd = u_heave_stbd_fwd.real;
      offset += sizeof(this->heave_stbd_fwd);
      union {
        float real;
        uint32_t base;
      } u_heave_port_aft;
      u_heave_port_aft.base = 0;
      u_heave_port_aft.base |= ((uint32_t) (*(inbuffer + offset + 0))) << (8 * 0);
      u_heave_port_aft.base |= ((uint32_t) (*(inbuffer + offset + 1))) << (8 * 1);
      u_heave_port_aft.base |= ((uint32_t) (*(inbuffer + offset + 2))) << (8 * 2);
      u_heave_port_aft.base |= ((uint32_t) (*(inbuffer + offset + 3))) << (8 * 3);
      this->heave_port_aft = u_heave_port_aft.real;
      offset += sizeof(this->heave_port_aft);
      union {
        float real;
        uint32_t base;
      } u_heave_stbd_aft;
      u_heave_stbd_aft.base = 0;
      u_heave_stbd_aft.base |= ((uint32_t) (*(inbuffer + offset + 0))) << (8 * 0);
      u_heave_stbd_aft.base |= ((uint32_t) (*(inbuffer + offset + 1))) << (8 * 1);
      u_heave_stbd_aft.base |= ((uint32_t) (*(inbuffer + offset + 2))) << (8 * 2);
      u_heave_stbd_aft.base |= ((uint32_t) (*(inbuffer + offset + 3))) << (8 * 3);
      this->heave_stbd_aft = u_heave_stbd_aft.real;
      offset += sizeof(this->heave_stbd_aft);
     return offset;
    }

    const char * getType(){ return "riptide_msgs/Thrust"; };
    const char * getMD5(){ return "e6dea66de100b0f3680efd9d3f1a3305"; };

  };

}
#endif