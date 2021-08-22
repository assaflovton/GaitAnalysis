
import csv
import matplotlib.pyplot as plt
import os

script_dir = os.path.dirname(__file__)
results_dir = os.path.join(script_dir, 'ResultsAnalyzed/')

# create folder if does not exists already
if not os.path.isdir(results_dir):
    os.makedirs(results_dir)

# open every id_data.csv file and generate plots, all plots saved into ResultsAnalyzed folder
for patient_id in range(35, 40):  # choose the id's to perform analysis on

    sample_file_name = str(patient_id) + "_data"
    data_file_name = str(patient_id) + "_data.csv"
    ang_vel = []
    ang_vel_x = []
    ang_vel_y = []
    ang_vel_z = []
    acc = []
    acc_x = []
    acc_y = []
    acc_z = []
    time = []

    with open(data_file_name) as csv_file:
        data = csv.reader(csv_file, delimiter=',')
        for i in range(35):  # skip 5 first seconds of the samples to reduce noise due to start of recording
            next(data)
        for row in data:  # get data from csv file
            time.append(float(row[0]))
            acc_x.append(float(row[1]))
            acc_y.append(float(row[2]))
            acc_z.append(float(row[3]))
            ang_vel_x.append(float(row[4]))
            ang_vel_y.append(float(row[5]))
            ang_vel_z.append(float(row[6]))
            acc.append((float(row[1]) ** 2 + float(row[2]) ** 2 + float(row[3]) ** 2) ** 0.5)  # size of acc
            ang_vel.append((float(row[4]) ** 2 + float(row[5]) ** 2
                            + float(row[6]) ** 2) ** 0.5)  # size of angular velocity

    # ----------------------------------------generate plots-------------------------------------------------

    # create size of acc graph
    plt.plot(time, acc)
    plt.xlabel('time [milliseconds]')
    plt.ylabel('acceleration [m/s^2]')
    plt.title('patient id: '+str(patient_id)+' - acceleration as a function of time')
    sample_file_name = str(patient_id) + "_fig_acc"
    plt.savefig(results_dir + sample_file_name)
    # plt.show()
    plt.clf()

    # create acc_x graph
    plt.plot(time, acc_x)
    plt.xlabel('time [milliseconds]')
    plt.ylabel('acceleration x axis [m/s^2]')
    plt.title('patient id: ' + str(patient_id) + ' - acceleration x axis as a function of time')
    sample_file_name = str(patient_id) + "_fig_acc_x"
    plt.savefig(results_dir + sample_file_name)
    # plt.show()
    plt.clf()

    # create acc_y graph
    plt.plot(time, acc_y)
    plt.xlabel('time [milliseconds]')
    plt.ylabel('acceleration y axis [m/s^2]')
    plt.title('patient id: ' + str(patient_id) + ' - acceleration y axis as a function of time')
    sample_file_name = str(patient_id) + "_fig_acc_y"
    plt.savefig(results_dir + sample_file_name)
    # plt.show()
    plt.clf()

    # create acc_z graph
    plt.plot(time, acc_z)
    plt.xlabel('time [milliseconds]')
    plt.ylabel('acceleration z axis [m/s^2]')
    plt.title('patient id: ' + str(patient_id) + ' - acceleration z axis as a function of time')
    sample_file_name = str(patient_id) + "_fig_acc_z"
    plt.savefig(results_dir + sample_file_name)
    # plt.show()
    plt.clf()

    # create size of ang_vel graph
    plt.plot(time, ang_vel)
    plt.xlabel('time [milliseconds]')
    plt.ylabel('angular velocity [deg/s]')
    plt.title('patient id: '+str(patient_id)+' - angular velocity as a function of time')
    sample_file_name = str(patient_id) + "_fig_ang"
    plt.savefig(results_dir + sample_file_name)
    # plt.show()
    plt.clf()

    # create size of ang_vel_x graph
    plt.plot(time, ang_vel_x)
    plt.xlabel('time [milliseconds]')
    plt.ylabel('angular velocity in x axis [deg/s]')
    plt.title('patient id: ' + str(patient_id) + ' - angular velocity x axis as a function of time')
    sample_file_name = str(patient_id) + "_fig_ang_x"
    plt.savefig(results_dir + sample_file_name)
    # plt.show()
    plt.clf()

    # create size of ang_vel_y graph
    plt.plot(time, ang_vel_y)
    plt.xlabel('time [milliseconds]')
    plt.ylabel('angular velocity in y axis [deg/s]')
    plt.title('patient id: ' + str(patient_id) + ' - angular velocity y axis as a function of time')
    sample_file_name = str(patient_id) + "_fig_ang_y"
    plt.savefig(results_dir + sample_file_name)
    # plt.show()
    plt.clf()

    # create size of ang_vel_z graph
    plt.plot(time, ang_vel_z)
    plt.xlabel('time [milliseconds]')
    plt.ylabel('angular velocity in z axis [deg/s]')
    plt.title('patient id: ' + str(patient_id) + ' - angular velocity z axis as a function of time')
    sample_file_name = str(patient_id) + "_fig_ang_z"
    plt.savefig(results_dir + sample_file_name)
    # plt.show()
    plt.clf()
