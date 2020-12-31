#!/usr/bin/python3

# QMesh
# Copyright (C) 2019 Daniel R. Fay

# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

import sys, os
import pika
import qmesh_pb2
import qmesh_common


# Write out to file
out_file = open("boot_log.json", 'w')

def dbg_process(ch, method, properties, body):
    ser_msg = qmesh_pb2.SerialMsg()
    ser_msg.ParseFromString(body)
    if(ser_msg.type == ser_msg.STATUS):
        qmesh_common.print_status_msg(ser_msg.status)
        if(ser_msg.status.status == ser_msg.status.MANAGEMENT):
            qmesh_common.channel.stop_consuming()
			
def log_process(ch, method, properties, body):
    ser_msg = qmesh_pb2.SerialMsg()
    ser_msg.ParseFromString(body)
    if(ser_msg.type == ser_msg.STATUS):
        qmesh_common.print_status_msg(ser_msg.status)
        if(ser_msg.status.status == ser_msg.status.MANAGEMENT):
            qmesh_common.channel.stop_consuming()
    elif(ser_msg.type == ser_msg.REPLY_BOOT_LOG):
        if(ser_msg.log_msg.valid == False):
            print("Finished reading in boot log entries")
            sys.exit(0)
        else:
            log_msg_str = qmesh_common.print_bootlog_msg(ser_msg.log_msg)
            out_file.write(log_msg_str)
            out_file.flush()
            ch.stop_consuming()

qmesh_common.setup_outgoing_rabbitmq()
qmesh_common.reboot_board()	
qmesh_common.channel.start_consuming()
qmesh_common.stay_in_management()

while True:
    ser_msg = qmesh_pb2.SerialMsg()
    ser_msg.type = qmesh_pb2.SerialMsg.READ_BOOT_LOG
    qmesh_common.publish_msg()
    qmesh_common.channel.basic_consume(queue=qmesh_common.queue_name, auto_ack=True, \
        on_message_callback=qmesh_common.log_process)	
    qmesh_common.channel.start_consuming()
