#!/usr/bin/env python3
#
# discover.py - search for Blutooth Low Energy devices and report their
# hardware address and friendly name.
#
# NOTE: Must run with sudo or have permission to access Bluetooth device.
#
import asyncio
from bleak import discover

async def run():
    devices = await discover()
    for dev in devices:
        print(dev)

loop = asyncio.get_event_loop()
loop.run_until_complete(run())
