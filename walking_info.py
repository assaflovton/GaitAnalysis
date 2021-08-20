import asyncio
import struct
from bleak import BleakClient
import csv
import time

# address = input("Device Address (e.g. 12:34:56:78:9A:BC): ")
TIME_UUID = "00000001-0000-1000-8000-00805f9b34fb"
VELX_UUID = "00000011-0000-1000-8000-00805f9b34fb"
VELY_UUID = "00000012-0000-1000-8000-00805f9b34fb"
VELZ_UUID = "00000013-0000-1000-8000-00805f9b34fb"

ACCX_UUID = "00000021-0000-1000-8000-00805f9b34fb"
ACCY_UUID = "00000022-0000-1000-8000-00805f9b34fb"
ACCZ_UUID = "00000023-0000-1000-8000-00805f9b34fb"

ANG_VELX_UUID = "00000031-0000-1000-8000-00805f9b34fb"
ANG_VELY_UUID = "00000032-0000-1000-8000-00805f9b34fb"
ANG_VELZ_UUID = "00000033-0000-1000-8000-00805f9b34fb"

ANG_ACCX_UUID = "00000041-0000-1000-8000-00805f9b34fb"
ANG_ACCY_UUID = "00000042-0000-1000-8000-00805f9b34fb"
ANG_ACCZ_UUID = "00000043-0000-1000-8000-00805f9b34fb"

# address_big = "C8:B2:F5:AF:16:74"
address = "8F:E7:07:F7:D7:70"
data = []


async def run(address):
    async with BleakClient(address) as client:
        print("Connected....Timer started")
        timeout = time.time() + 20 * 1  # 5 minutes from now
        while 1:
            curr_time = time.time()
            if time.time() > timeout:
                break
            accX = await client.read_gatt_char(ACCX_UUID)
            accY = await client.read_gatt_char(ACCY_UUID)
            accZ = await client.read_gatt_char(ACCZ_UUID)

            ang_velX = await client.read_gatt_char(ANG_VELX_UUID)
            ang_velY = await client.read_gatt_char(ANG_VELY_UUID)
            ang_velZ = await client.read_gatt_char(ANG_VELZ_UUID)

            data.append({'time': curr_time,
                         'accX': accX,
                         'accY': accY,
                         'accZ': accZ,
                         'ang_velX': ang_velX,
                         'ang_velY': ang_velY,
                         'ang_velZ': ang_velZ})
            """
            print("accX: {0} m/s^2".format(struct.unpack('f', accX)))
            print("accY: {0} m/s^2".format(struct.unpack('f', accX)))
            print("accZ: {0} m/s^2".format(struct.unpack('f', accZ)))

            print("anf_velX: {0} m/s^2".format(struct.unpack('f', ang_velX)))
            print("anf_velY: {0} m/s^2".format(struct.unpack('f', ang_velY)))
            print("anf_velZ: {0} m/s^2".format(struct.unpack('f', ang_velZ)))
            """


loop = asyncio.get_event_loop()
loop.run_until_complete(run(address))

print("finished recording")
# Write all data in the end of data collecting
fieldnames = ['time', 'accX', 'accY', 'accZ', 'ang_velX', 'ang_velY', 'ang_velZ']
res = []
for i in data:
    a_dict = {}
    for key, value in i.items():
        if key != "time":
            a_dict[key] = struct.unpack('f', value)
        else:
            a_dict[key] = value

    res.append(a_dict)
with open('data.csv', 'w', encoding='UTF8', newline='') as f:
    writer = csv.DictWriter(f, fieldnames=fieldnames)
    writer.writeheader()
    writer.writerows(res)