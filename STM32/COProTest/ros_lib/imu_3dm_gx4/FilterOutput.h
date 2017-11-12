#ifndef _ROS_imu_3dm_gx4_FilterOutput_h
#define _ROS_imu_3dm_gx4_FilterOutput_h

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "ros/msg.h"
#include "std_msgs/Header.h"
#include "geometry_msgs/Quaternion.h"
#include "geometry_msgs/Vector3.h"

namespace imu_3dm_gx4
{

  class FilterOutput : public ros::Msg
  {
    public:
      typedef std_msgs::Header _header_type;
      _header_type header;
      typedef uint16_t _quat_status_type;
      _quat_status_type quat_status;
      typedef uint16_t _bias_status_type;
      _bias_status_type bias_status;
      typedef geometry_msgs::Quaternion _orientation_type;
      _orientation_type orientation;
      float orientation_covariance[9];
      typedef geometry_msgs::Vector3 _bias_type;
      _bias_type bias;
      float bias_covariance[9];
      typedef uint16_t _bias_covariance_status_type;
      _bias_covariance_status_type bias_covariance_status;
      typedef uint16_t _orientation_covariance_status_type;
      _orientation_covariance_status_type orientation_covariance_status;
      enum { STATUS_INVALID =  0 };
      enum { STATUS_VALID =  1 };
      enum { STATUS_VALID_REFERENCED =  2 };

    FilterOutput():
      header(),
      quat_status(0),
      bias_status(0),
      orientation(),
      orientation_covariance(),
      bias(),
      bias_covariance(),
      bias_covariance_status(0),
      orientation_covariance_status(0)
    {
    }

    virtual int serialize(unsigned char *outbuffer) const
    {
      int offset = 0;
      offset += this->header.serialize(outbuffer + offset);
      *(outbuffer + offset + 0) = (this->quat_status >> (8 * 0)) & 0xFF;
      *(outbuffer + offset + 1) = (this->quat_status >> (8 * 1)) & 0xFF;
      offset += sizeof(this->quat_status);
      *(outbuffer + offset + 0) = (this->bias_status >> (8 * 0)) & 0xFF;
      *(outbuffer + offset + 1) = (this->bias_status >> (8 * 1)) & 0xFF;
      offset += sizeof(this->bias_status);
      offset += this->orientation.serialize(outbuffer + offset);
      for( uint32_t i = 0; i < 9; i++){
      offset += serializeAvrFloat64(outbuffer + offset, this->orientation_covariance[i]);
      }
      offset += this->bias.serialize(outbuffer + offset);
      for( uint32_t i = 0; i < 9; i++){
      offset += serializeAvrFloat64(outbuffer + offset, this->bias_covariance[i]);
      }
      *(outbuffer + offset + 0) = (this->bias_covariance_status >> (8 * 0)) & 0xFF;
      *(outbuffer + offset + 1) = (this->bias_covariance_status >> (8 * 1)) & 0xFF;
      offset += sizeof(this->bias_covariance_status);
      *(outbuffer + offset + 0) = (this->orientation_covariance_status >> (8 * 0)) & 0xFF;
      *(outbuffer + offset + 1) = (this->orientation_covariance_status >> (8 * 1)) & 0xFF;
      offset += sizeof(this->orientation_covariance_status);
      return offset;
    }

    virtual int deserialize(unsigned char *inbuffer)
    {
      int offset = 0;
      offset += this->header.deserialize(inbuffer + offset);
      this->quat_status =  ((uint16_t) (*(inbuffer + offset)));
      this->quat_status |= ((uint16_t) (*(inbuffer + offset + 1))) << (8 * 1);
      offset += sizeof(this->quat_status);
      this->bias_status =  ((uint16_t) (*(inbuffer + offset)));
      this->bias_status |= ((uint16_t) (*(inbuffer + offset + 1))) << (8 * 1);
      offset += sizeof(this->bias_status);
      offset += this->orientation.deserialize(inbuffer + offset);
      for( uint32_t i = 0; i < 9; i++){
      offset += deserializeAvrFloat64(inbuffer + offset, &(this->orientation_covariance[i]));
      }
      offset += this->bias.deserialize(inbuffer + offset);
      for( uint32_t i = 0; i < 9; i++){
      offset += deserializeAvrFloat64(inbuffer + offset, &(this->bias_covariance[i]));
      }
      this->bias_covariance_status =  ((uint16_t) (*(inbuffer + offset)));
      this->bias_covariance_status |= ((uint16_t) (*(inbuffer + offset + 1))) << (8 * 1);
      offset += sizeof(this->bias_covariance_status);
      this->orientation_covariance_status =  ((uint16_t) (*(inbuffer + offset)));
      this->orientation_covariance_status |= ((uint16_t) (*(inbuffer + offset + 1))) << (8 * 1);
      offset += sizeof(this->orientation_covariance_status);
     return offset;
    }

    const char * getType(){ return "imu_3dm_gx4/FilterOutput"; };
    const char * getMD5(){ return "40af8b09da9b33d02fc6b6288f52b159"; };

  };

}
#endif