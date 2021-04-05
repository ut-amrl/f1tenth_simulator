#!/usr/bin/env python

import rospy
import roslib
import sys

NODE_NAME = 'pedestrian_simulation'
roslib.load_manifest(NODE_NAME)

from pedsim_msgs.msg import AgentStates                # noqa: E402
from ut_multirobot_sim.msg import HumanControlCommand  # noqa: E402

human_count = int(sys.argv[1])
#  human_command_pubs = [None] + [
    #  rospy.Publisher(f'/human{n}/command', HumanControlCommand, queue_size=2 * human_count)
    #  for n in range(1, human_count + 1)]

human_command_pubs = []
for i in range(0, human_count):
    nameString = '/human' + str(i) + '/command'
    human_command_pubs.append(rospy.Publisher(nameString,
                                              HumanControlCommand,
                                              queue_size=1))

def agent_states_callback(agent_states):
    # The states themselves are in an array named "agent_states", so replace
    # the original message with that array.
    agent_states = agent_states.agent_states

    #  for i in  range(0, len(agent_states)):
        #  agent_state = agent_states[i]
        #  command_message = HumanControlCommand()
        #  command_message.header.frame_id = agent_state.header.frame_id
        #  command_message.header.stamp = rospy.Time.now()
        #  command_message.translational_velocity = agent_state.twist.linear
        #  command_message.rotational_velocity = 0.0
        #  # Subtracting one to get this to place nice with arrays and
        #  # basically every other step
        #  human_command_pubs[i].publish(command_message)

    for agent_state in agent_states:
        if agent_state.id in range(1, human_count + 1):
            command_message = HumanControlCommand()
            command_message.header.frame_id = agent_state.header.frame_id
            command_message.header.stamp = rospy.Time.now()
            command_message.translational_velocity = agent_state.twist.linear
            command_message.rotational_velocity = 0.0
            command_message.pose = agent_state.pose.position
            # Subtracting one to get this to place nice with arrays and
            # basically every other step
            human_command_pubs[agent_state.id - 1].publish(command_message)

def main():
    rospy.init_node(NODE_NAME)
    rospy.Subscriber('/pedsim_simulator/simulated_agents', AgentStates, agent_states_callback)
    rospy.spin()

if __name__ == '__main__':
    main()