import csv
import matplotlib.pyplot as plt
import os

script_dir = os.path.dirname(__file__)
results_dir = os.path.join(script_dir, 'ResultsAnalyzedWithDots/')

# create folder if does not exists already
if not os.path.isdir(results_dir):
    os.makedirs(results_dir)

# open every id_data.csv file and generate plots, all plots saved into ResultsAnalyzed folder
for patient_id in range(70, 71):  # choose the id's to perform analysis on

    sample_file_name = str(patient_id) + "_data"
    data_file_name = str(patient_id) + "_data.csv"
    vels = []
    times = []

    with open(data_file_name) as csv_file:
        data = csv.reader(csv_file, delimiter=',')
        for i in range(35):  # skip 5 first seconds of the samples to reduce noise due to start of recording
            next(data)
        for row in data:  # get data from csv file
            times.append(float(row[0]))
            vels.append(float(row[1]))

    #  --------------------------------- Simulate code from arduino on the data --------------------------------------

    # ---------------------------------- PARAMETERS START -----------------------------------------

    # MS - Mid Swing state 1
    # HS - Heel Strike state 2
    # TF - Toe Off state 3

    MS_vals = []
    HS_vals = []
    TF_vals = []
    MS_times = []
    HS_times = []
    TF_times = []

    avg_time = 0
    avg_vel = 0
    rt_vals = []
    rt_times = []

    # --------------- CONSTANTS -----------------

    INITIAL_TIME_BETWEEN_STATES = 160

    INITIAL_AMPLITUDE = 0

    STEP_MAX_TIME = 3000

    RESTART_THRESH = 20000

    TIME_FLEX = 0.8

    VALUE_FLEX = 0.7

    AVG_CHANGE_RATE = 0.2

    VALID_STEP_RATIO = 0.4

    CORRELATION_RATIO = 0.7

    STATE_SWITCH_THRESH = 0.5

    # ----------- Thresholds ------------------

    step_state = 3

    TFtoMS = INITIAL_TIME_BETWEEN_STATES
    MStoHS = INITIAL_TIME_BETWEEN_STATES
    HStoTF = INITIAL_TIME_BETWEEN_STATES

    prev_HS = INITIAL_AMPLITUDE
    prev_MS = INITIAL_AMPLITUDE

    # time of last state change

    prev_state_time = 0
    prev_step_time = 0

    # time between steps

    step_thresh = STEP_MAX_TIME

    # time to restart thresholds

    restart_time = 0

    # in real time need to add window cleanup mechanism

    # ----------------------------------- PARAMETERS END -----------------------------------------

    for index, (vel, time) in enumerate(zip(vels, times)):  # in real time this will be a while loop with sleeping
        avg_time = avg_time + time
        avg_vel = avg_vel + vel
        if (index + 1) % 4 == 0:
            rt_vals.append(avg_vel / 4.0)
            rt_times.append(avg_time / 4.0)
            avg_vel = 0
            avg_time = 0
            if len(rt_vals) >= 5:

                # restart every restart_thresh to mitigate errors
                if rt_times[-3] - restart_time > RESTART_THRESH and step_state == 3:
                    restart_time = rt_times[-3]
                    prev_MS = INITIAL_AMPLITUDE
                    prev_HS = INITIAL_AMPLITUDE
                    TFtoMS = INITIAL_TIME_BETWEEN_STATES
                    MStoHS = INITIAL_TIME_BETWEEN_STATES
                    HStoTF = INITIAL_TIME_BETWEEN_STATES

                # if we passed step_thresh without a step then restart state
                # if we missed TF than restart state
                if (rt_times[-3] - prev_step_time > step_thresh) or\
                        ((rt_vals[-3] > STATE_SWITCH_THRESH * prev_MS) and step_state == 2):
                    step_state = 3

                # MS is a global max
                if rt_vals[-3] > rt_vals[-1] and rt_vals[-3] > rt_vals[-2] and \
                        rt_vals[-3] > rt_vals[-4] and rt_vals[-3] > rt_vals[-5] and \
                        rt_vals[-3] > VALUE_FLEX * prev_MS and \
                        rt_times[-3] - prev_state_time >= TIME_FLEX * TFtoMS:
                    # state is MS,  in realtime only changes state
                    MS_times.append(rt_times[-3])
                    MS_vals.append(rt_vals[-3])
                    step_state = 1
                    prev_MS = rt_vals[-3]
                    prev_step_time = rt_times[-3]
                    if TFtoMS * (1 - VALID_STEP_RATIO) < (rt_times[-3] - prev_state_time) < TFtoMS * (
                            1 + VALID_STEP_RATIO):
                        TFtoMS = ((1 - AVG_CHANGE_RATE) * TFtoMS + AVG_CHANGE_RATE * (rt_times[-3] - prev_state_time))
                    prev_state_time = rt_times[-3]


                # HS state is a local minimum after MS
                elif rt_vals[-2] > rt_vals[-3] < rt_vals[-4] and \
                        step_state == 1 and -CORRELATION_RATIO * prev_MS < rt_vals[-3] < 0 and \
                        rt_times[-3] - prev_state_time >= TIME_FLEX * MStoHS:
                    # state is MS, in realtime changes state and starts vibration
                    HS_times.append(rt_times[-3])
                    HS_vals.append(rt_vals[-3])
                    step_state = 2
                    prev_HS = rt_vals[-3]
                    if MStoHS * (1 - VALID_STEP_RATIO) < (rt_times[-3] - prev_state_time) < MStoHS * (
                            1 + VALID_STEP_RATIO):
                        MStoHS = ((1 - AVG_CHANGE_RATE) * MStoHS + AVG_CHANGE_RATE * (rt_times[-3] - prev_state_time))
                    prev_state_time = rt_times[-3]



                # TF is a global minimum after HS
                elif rt_vals[-1] > rt_vals[-3] and rt_vals[-2] > rt_vals[-3] and \
                        rt_vals[-4] > rt_vals[-3] and rt_vals[-5] > rt_vals[-3] and \
                        step_state == 2 and rt_vals[-3] < VALUE_FLEX * prev_HS and \
                        rt_times[-3] - prev_state_time >= TIME_FLEX * HStoTF:
                    TF_times.append(rt_times[-3])
                    TF_vals.append(rt_vals[-3])
                    step_state = 3
                    if HStoTF * (1 - VALID_STEP_RATIO) < (rt_times[-3] - prev_state_time) < HStoTF * (
                            1 + VALID_STEP_RATIO):
                        HStoTF = ((1 - AVG_CHANGE_RATE) * HStoTF + AVG_CHANGE_RATE * (rt_times[-3] - prev_state_time))
                    prev_state_time = rt_times[-3]

    # ----------------generate plots-------------------
    # create size of ang_vel_y graph

    fig, ax = plt.subplots(figsize=(50, 10))
    plt.grid()
    plt.plot(rt_times, rt_vals, 'k', label='Graph')
    # plt.plot(rt_times, rt_vals, 'c+', label='Original data')
    plt.plot(MS_times, MS_vals, "r+", label='MS')
    plt.plot(HS_times, HS_vals, "y+", label='HS')
    plt.plot(TF_times, TF_vals, "b+", label='TF')
    plt.xlabel('time [milliseconds]')
    plt.ylabel('angular velocity in y axis [deg/s]')
    plt.title('patient id: ' + str(patient_id) + ' - angular velocity y axis as a function of time')
    sample_file_name = str(patient_id) + "_fig_ang_y"
    plt.legend(loc='lower right')
    plt.savefig(results_dir + sample_file_name, dpi=400)
    # plt.show()
    plt.clf()
