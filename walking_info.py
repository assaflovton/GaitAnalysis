import asyncio
import struct
from bleak import BleakClient
import csv
import time
import matplotlib.pyplot as plt
import os

patient_id = input("insert patient id:")
script_dir = os.path.dirname(__file__)
results_dir = os.path.join(script_dir, 'Results/')
sample_file_name = patient_id + "_data"
data_file_name = patient_id + "_data.csv"

# address = input("Device Address (e.g. 12:34:56:78:9A:BC): ")
ALL_UUID = "00000011-0000-1000-8000-00805f9b34fb"

# address_big = "C8:B2:F5:AF:16:74"
address = "8F:E7:07:F7:D7:70"

meas_list = []

# ---------------------------- Receive data from arduino----------------------------------
async def run(address):
    async with BleakClient(address) as client:
        print("Connected....Timer started")
        timeout = time.time() + 40 * 1  # 40 secs sample
        start_time = time.time()
        while 1:
            curr_time = round((time.time() - start_time) * 1000)
            if time.time() > timeout:
                break
            curr_meas = await client.read_gatt_char(ALL_UUID)
            meas_list.append((curr_time, curr_meas))


loop = asyncio.get_event_loop()
loop.run_until_complete(run(address))

print("finished recording")

# ---------------------------- Parse data collected----------------------------------

# Write all data in the end of data collecting
fieldnames = ['time', 'accX', 'accY', 'accZ', 'ang_velX', 'ang_velY', 'ang_velZ']

res = []

for t, v in meas_list:
    unpacked_meas = struct.unpack('ffffff', v)
    res.append({'time': t,
                'accX': unpacked_meas[0],
                'accY': unpacked_meas[1],
                'accZ': unpacked_meas[2],
                'ang_velX': unpacked_meas[3],
                'ang_velY': unpacked_meas[4],
                'ang_velZ': unpacked_meas[5]})

with open(data_file_name, 'w', encoding='UTF8', newline='') as f:
    writer = csv.DictWriter(f, fieldnames=fieldnames)
    writer.writeheader()
    writer.writerows(res)

# ---------------------------- Create plots and save them to Results folder ----------------------------------

if not os.path.isdir(results_dir):
    os.makedirs(results_dir)

ang_vel = []
acc = []
time = []

with open(data_file_name) as csv_file:
    data = csv.reader(csv_file, delimiter=',')
    next(data)
    for row in data:
        acc.append((float(row[1]) ** 2 + float(row[2]) ** 2 + float(row[3]) ** 2) ** 0.5)
        ang_vel.append((float(row[4]) ** 2 + float(row[5]) ** 2 + float(row[6]) ** 2) ** 0.5)
        time.append(float(row[0]))

plt.plot(time, acc)
plt.xlabel('time [milliseconds]')
plt.ylabel('acceleration [m/s^2]')
plt.title('patient id: '+patient_id+' acceleration as a function of time')
sample_file_name = patient_id + "_fig_acc"
plt.savefig(results_dir + sample_file_name)
# plt.show()
plt.clf()

plt.plot(time, ang_vel)
plt.xlabel('time [milliseconds]')
plt.ylabel('angular velocity [deg/s]')
plt.title('patient id: '+patient_id+' angular velocity as a function of time')
sample_file_name = patient_id + "_fig_ang"
plt.savefig(results_dir + sample_file_name)
# plt.show()
plt.clf()
