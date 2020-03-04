#!/bin/bash

rm -f /tmp/ttyReicast
reicast -config config:Debug.VirtualSerialPortFile=/tmp/ttyReicast -config config:Debug.VirtualSerialPort=yes nodisk &
sleep 10
dc-tool -t /tmp/ttyReicast -x "$2"
