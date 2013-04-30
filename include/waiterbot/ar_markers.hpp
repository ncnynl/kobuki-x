/*
 * ar_markers.hpp
 *
 *  Created on: Apr 6, 2013
 *      Author: jorge
 */

#ifndef AR_MARKERS_HPP_
#define AR_MARKERS_HPP_

#include <boost/function.hpp>

#include <tf/tf.h>
#include <tf/transform_listener.h>
#include <tf/transform_broadcaster.h>

#include <ros/ros.h>

#include <geometry_msgs/PoseWithCovarianceStamped.h>
#include <ar_track_alvar/AlvarMarkers.h>

#include "waiterbot/common.hpp"
namespace waiterbot
{

class ARMarkers
{
public:
  ARMarkers();
  virtual ~ARMarkers();

  bool init();

  void setRobotPoseCB(boost::function<void (const geometry_msgs::PoseWithCovarianceStamped::ConstPtr&)> cb)
  {
    robot_pose_cb_ = cb;
  };

  void baseSpottedCB(boost::function<void (const geometry_msgs::PoseStamped::ConstPtr&, uint32_t)> cb)
  {
    base_spotted_cb_ = cb;
  };

  const geometry_msgs::PoseStamped& getDockingBasePose() {

//    tf::Transform marker_gb(tf::createQuaternionFromYaw(tf::getYaw(docking_marker_.pose.pose.orientation) - M_PI/2.0),// tk::pitch(docking_marker_.pose)),
//                            tf::Vector3(docking_marker_.pose.pose.position.x, docking_marker_.pose.pose.position.y, 0.0));
//
//    // Half turn and translate to put goal at some distance in front of the marker
//    tf::Transform in_front(tf::createQuaternionFromYaw(M_PI),
//                           tf::Vector3(1.1/1.5, 0.0, 0.0));
//    tf::StampedTransform st1(marker_gb, ros::Time::now(), "map", "GOAL 1");
//    tf::StampedTransform st2(marker_gb*in_front, ros::Time::now(), "map", "GOAL 2");
//
//    for (int i = 0; i < 20; i++)
//    {
//    tf_brcaster_.sendTransform(st1);
//    tf_brcaster_.sendTransform(st2);
//    ros::Duration(0.5).sleep();
//    }
//

    return docking_marker_.pose; };

  bool spotted(double younger_than,
               const ar_track_alvar::AlvarMarkers& including, const ar_track_alvar::AlvarMarkers& excluding,
               ar_track_alvar::AlvarMarkers& spotted_markers);
  bool closest(const ar_track_alvar::AlvarMarkers& including, const ar_track_alvar::AlvarMarkers& excluding,
               ar_track_alvar::AlvarMarker& closest_marker);

  /**
   * Return spotted markers satisfying the constraints specified by the parameters
   * @param younger_than    Elapsed time between now and markers timestamp must be below this limit.
   * @param min_confidence
   * @param exclude_globals
   * @param spotted_markers
   * @return
   */
  bool spotted(double younger_than, int min_confidence, bool exclude_globals,
               ar_track_alvar::AlvarMarkers& spotted_markers);
  bool closest(double younger_than, int min_confidence, bool exclude_globals,
               ar_track_alvar::AlvarMarker& closest_marker);

  bool spotDockMarker(uint32_t base_marker_id);

  static bool enableTracker();
  static bool disableTracker();

private:
  std::string global_frame_;
  std::string odom_frame_;
  std::string base_frame_;

  std::vector<uint32_t> times_spotted_;

  tf::TransformListener    tf_listener_;
  tf::TransformBroadcaster tf_brcaster_;
  double                   tf_brc_freq_;  /**< Allows enabling tf broadcasting; mostly for debug */

  ar_track_alvar::AlvarMarker  docking_marker_;  /**< AR markers described in the semantic map */
  ar_track_alvar::AlvarMarkers global_markers_;  /**< AR markers described in the semantic map */
  ar_track_alvar::AlvarMarkers spotted_markers_;

  ros::Subscriber tracked_markers_sub_;
  ros::Subscriber global_markers_sub_;

  boost::function<void (const geometry_msgs::PoseWithCovarianceStamped::ConstPtr&)>  robot_pose_cb_;
  boost::function<void (const geometry_msgs::PoseStamped::ConstPtr&, uint32_t)>    base_spotted_cb_;

  void broadcastMarkersTF();
  void globalMarkersCB(const ar_track_alvar::AlvarMarkers::ConstPtr& msg);
  void arPoseMarkersCB(const ar_track_alvar::AlvarMarkers::ConstPtr& msg);

  bool included(const uint32_t id, const ar_track_alvar::AlvarMarkers& v,
                ar_track_alvar::AlvarMarker* e = NULL)
  {
    for (unsigned int i = 0; i < v.markers.size(); i++)
    {
      if (id == v.markers[i].id)
      {
        if (e != NULL)
          *e = v.markers[i];

        return true;
      }
    }

    return false;
  }

  bool excluded(const uint32_t id, const ar_track_alvar::AlvarMarkers& v)
  {
    for (unsigned int i = 0; i < v.markers.size(); i++)
    {
      if (id == v.markers[i].id)
        return false;
    }

    return true;
  }
};

} /* namespace waiterbot */

#endif /* AR_MARKERS_HPP_ */
