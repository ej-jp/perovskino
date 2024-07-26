import pandas as pd
import glob
import os
import matplotlib.pyplot as plt
import matplotlib.dates as mdates
import matplotlib.ticker as ticker
from matplotlib.dates import DateFormatter
import time
from scipy import integrate
from matplotlib.offsetbox import OffsetImage

def update_graph():
    while True:
        ########################################################################
        path = 'dataraw/'

        
        all_files = glob.glob(os.path.join(path, "*.csv"))
        all_files=sorted(all_files) 
        
        
        df = pd.concat((pd.read_csv(f, header=None, sep='\t',names = ["time", "id", "notvalid9","type", "notvalid1", "integer4725", "notvalid2","volt4725","notvalid3","voltage","notvalid4","current","notvalid5","power","notvalid6","integermpp4725","notvalid7","mpppower","notvalid8","temperature"]) for f in all_files))
        #on_bad_lines='skip' quitado para version pandas de crouton
        
        
        # upload from df.to csv, comma separator, remove header.
        #df = pd.concat((pd.read_csv(f, header=None, sep=',', on_bad_lines='skip',names = ["time", "type", "notvalid1", "integer4725", "notvalid2","volt4725","notvalid3","voltage","notvalid4","current","notvalid5","power","notvalid6","integermpp4725","notvalid7","mpppower","notvalid8","temperature"]) for f in all_files))
        
        df["time"] =  pd.to_datetime(df["time"], errors='coerce')
        df = df.dropna(subset=["time"])
        df = df.set_index(df["time"])
        
        
        #df.to_csv('Output.csv', index = False)
        
        
        #solar cell active area
        #area = 0.64
        area = 1.18
        # area = 1.0
        df["mpppower"] =  df["mpppower"].astype("float") / area
        df["power"] =  df["power"].astype("float") / area
        df["current"] =  df["current"].astype("float") / area
        df["resistance"] = df["voltage"].astype("float")/df["current"].astype("float") # ohm cm2
        
        
        df = df.drop(df[df['type'] == 'start'].index)
        df = df[~df.apply(lambda row: row.astype(str).str.contains('Received value').any(), axis=1)]
        
        df = df[["time","type","integer4725","volt4725","voltage","current","power","mpppower","temperature","resistance"]].copy()
        
        
        
        
        # Modify the 'type' column
        label_counter = {'JV': 0, 'MPP': 0}
        group_label = None
        
        for i in range(len(df)):
            current_label = df.iloc[i]['type']
            if current_label == group_label:
                df.at[df.index[i], 'type'] = current_label + str(label_counter[current_label])
            else:
                group_label = current_label
                label_counter[group_label] += 1
                df.at[df.index[i], 'type'] = current_label + str(label_counter[current_label])
        
        
        
        
        
        
        
        #dfMPP = df[(df.type == 'MPP1') | (df.type == 'MPP') | (df.type == 'MPP0') | (df.type == 'MPP+1') | (df.type == 'MPP-1')]
        #dfMPP = df[df['type'].str.startswith('MPP')]
        #dfJV = df[df['type'].str.startswith('JV')]
        
        
        
        
        # ################# PLOTS ############################################
        
        
        
        #fig = plt.figure(figsize=(10, 10))
        #plt.rcParams["font.family"] = "Lato"
        
        
        ####################################################################
        #Create subplots in Python
        ####################################################################
        ################# SUBPLOT1 #########################################
        plt.subplot(3,3,4)
        
        #plt.title("Instantaneous power delivery")
        plt.ylabel("Power (mW/cm$^2$)")
        plt.xlabel("Time (s)")
        
        plt.minorticks_on()
        
        
        #plt.gca().xaxis.set_major_formatter(mdates.DateFormatter("%m/%d %H:%M"))
        #plt.setp(plt.gca().get_xticklabels(), ha="right", rotation=45)
        
        
        #plt.plot(dfMPP["time"],dfMPP["mpppower"], label='mpppower')#,color = '#8000ff')
        #plt.plot(dfMPP["time"],dfMPP["power"], label='power')#,color = '#8000ff')
        #plt.plot(df["time"],df["mpppower"], label='mpppowerfull')#,color = '#8000ff')
        #plt.plot(df["time"],df["power"], label='powerfull')#,color = '#8000ff')
        
        grouped = df.groupby('type')
        for group_name, group_data in grouped:
            plt.plot(group_data['time'], group_data['power'], label=group_name)
        #plt.legend(loc='best')
        
        ################# SUBPLOT8 #########################################
        plt.subplot(3,3,8)
        
        #plt.title("Temperature")
        plt.ylabel("Temperature (ºC)")
        plt.xlabel("Time (s)")
        
        plt.minorticks_on()
        
        
        #plt.gca().xaxis.set_major_formatter(mdates.DateFormatter("%m/%d %H:%M"))
        #plt.setp(plt.gca().get_xticklabels(), ha="right", rotation=45)
        
        #plt.plot(df["time"],df["temperature"], label='temperaturefull')#,color = '#8000ff')
        
        grouped = df.groupby('type')
        for group_name, group_data in grouped:
            plt.plot(group_data['time'], group_data['temperature'], label=group_name)
        
        #plt.legend(loc='best')
        
        ################# SUBPLOT2 #########################################
        plt.subplot(3,3,2)
        
        #plt.title("IV curve")
        
        #plt.xlabel("time")
        plt.ylabel("J (mA/cm$^2$)")
        plt.xlabel("Voltage (mV)")
        
        plt.minorticks_on()
        
        
        
        # plt.gca().xaxis.set_major_formatter(mdates.DateFormatter("%m/%d %H:%M"))
        # plt.setp(plt.gca().get_xticklabels(), ha="right", rotation=45)
        
        #plt.plot(dfJV["voltage"],dfJV["current"], label='current')#,color = '#8000ff')
        #plt.plot(dfJV["voltage"],dfJV["power"], label='power')#,color = '#8000ff')
        
        #print(df)
        
        grouped = df.groupby('type')
        #print(grouped)
        
        for group_name, group_data in grouped:
            plt.plot(group_data['voltage'], group_data['current'], label=group_name)
        
        
        #plt.legend(loc='best')
        
        
        ################# SUBPLOT5 #########################################
        plt.subplot(3,3,5)
        
        #plt.title("Power vs V")
        
        #plt.xlabel("time")
        plt.ylabel("Power (mW/cm$^2$)")
        plt.xlabel("Voltage (mV)")
        
        plt.minorticks_on()
        
        
        
        # plt.gca().xaxis.set_major_formatter(mdates.DateFormatter("%m/%d %H:%M"))
        # plt.setp(plt.gca().get_xticklabels(), ha="right", rotation=45)
        
        #plt.plot(dfJV["voltage"],dfJV["current"], label='current')#,color = '#8000ff')
        #plt.plot(dfJV["voltage"],dfJV["power"], label='power')#,color = '#8000ff')
        
        #print(df)
        
        grouped = df.groupby('type')
        #print(grouped)
        
        for group_name, group_data in grouped:
            plt.plot(group_data['voltage'], group_data['power'], label=group_name)
            #plt.plot(group_data['current'], group_data['power'], label=group_name)
        
        #plt.legend(loc='best')
        
        
        ################# SUBPLOT3 #########################################
        plt.subplot(3,3,9)
        
        #plt.title("DAC int vs Voltage cell (mv)")
        
        #plt.xlabel("time")
        #plt.xlabel("DAC int (-)")
        plt.xlabel("Voltage MOSFET-gate (mV)")
        plt.ylabel("Voltage (mV)")
        
        plt.minorticks_on()
        
        
        
        # plt.gca().xaxis.set_major_formatter(mdates.DateFormatter("%m/%d %H:%M"))
        # plt.setp(plt.gca().get_xticklabels(), ha="right", rotation=45)
        
        #plt.plot(dfJV["voltage"],dfJV["current"], label='current')#,color = '#8000ff')
        #plt.plot(dfJV["voltage"],dfJV["power"], label='power')#,color = '#8000ff')
        
        #print(df)
        
        grouped = df.groupby('type')
        #print(grouped)
        
        for group_name, group_data in grouped:
            #plt.plot(group_data['integer4725'], group_data['voltage'], label=group_name)
            plt.plot(group_data['volt4725'], group_data['voltage'], label=group_name)
            
            
            
        
        
        #plt.legend(loc='best')
        
        ################# SUBPLOT7 #########################################
        plt.subplot(3,3,6)
        
        #plt.title("Transfer curve")
        
        #plt.xlabel("time")
        plt.ylabel("Power (mW/cm$^2$)")
        plt.xlabel("Voltage MOSFET-gate (mV)")
        
        plt.minorticks_on()
        
        
        
        # plt.gca().xaxis.set_major_formatter(mdates.DateFormatter("%m/%d %H:%M"))
        # plt.setp(plt.gca().get_xticklabels(), ha="right", rotation=45)
        
        #plt.plot(dfJV["voltage"],dfJV["current"], label='current')#,color = '#8000ff')
        #plt.plot(dfJV["voltage"],dfJV["power"], label='power')#,color = '#8000ff')
        
        #print(df)
        
        grouped = df.groupby('type')
        #print(grouped)
        
        for group_name, group_data in grouped:
            plt.plot(group_data['volt4725'], group_data['power'], label=group_name)
        
        #plt.legend(loc='best')
        #plt.invert_xaxis()
        
        ################## SUBPLOT7 alternative#########################################
        #plt.subplot(3,3,6)
        #
        #plt.title("__")
        #
        ##plt.xlabel("time")
        #plt.ylabel("R ()")
        #plt.xlabel("VoltageMOSFET (mV)")
        #plt.yscale('log')
        #plt.minorticks_on()
        #
        #
        #
        ## plt.gca().xaxis.set_major_formatter(mdates.DateFormatter("%m/%d %H:%M"))
        ## plt.setp(plt.gca().get_xticklabels(), ha="right", rotation=45)
        #
        ##plt.plot(dfJV["voltage"],dfJV["current"], label='current')#,color = '#8000ff')
        ##plt.plot(dfJV["voltage"],dfJV["power"], label='power')#,color = '#8000ff')
        #
        ##print(df)
        #
        #grouped = df.groupby('type')
        ##print(grouped)
        #
        #for group_name, group_data in grouped:
        #    plt.plot(group_data['volt4725'], group_data['resistance'], label=group_name)
        #
        #
        ##plt.legend(loc='best')
        ##plt.invert_xaxis()

        
        ################# SUBPLOT9 #########################################
        plt.subplot(3,3,3)
        
        #plt.title("Transfer curve")
        
        plt.ylabel("J (mA/cm$^2$)")
        plt.xlabel("Voltage MOSFET-gate (mV)")
        
        plt.minorticks_on()
        
        
        
        # plt.gca().xaxis.set_major_formatter(mdates.DateFormatter("%m/%d %H:%M"))
        # plt.setp(plt.gca().get_xticklabels(), ha="right", rotation=45)
        
        #plt.plot(dfJV["voltage"],dfJV["current"], label='current')#,color = '#8000ff')
        #plt.plot(dfJV["voltage"],dfJV["power"], label='power')#,color = '#8000ff')
        
        #print(df)
        
        grouped = df.groupby('type')
        #print(grouped)
        
        for group_name, group_data in grouped:
            plt.plot(group_data['volt4725'], group_data['current'], label=group_name)
            
        
        #plt.axhline(0, color='black', linestyle='--')
        #ax.set_ylim(0, 950)
        #plt.legend(loc='best')
        #plt.invert_xaxis()
        ################# SUBPLOT4#########################################
        plt.subplot(3,3,1)
        
        #plt.title("current")
        
        #plt.xlabel("time")
        plt.ylabel("J (mA/cm$^2$)")
        plt.xlabel("Time (s)")
        
        plt.minorticks_on()
        
        
        
        # plt.gca().xaxis.set_major_formatter(mdates.DateFormatter("%m/%d %H:%M"))
        # plt.setp(plt.gca().get_xticklabels(), ha="right", rotation=45)
        
        #plt.plot(dfMPP["time"],dfMPP["current"], label='current')#,color = '#8000ff')
        
        grouped = df.groupby('type')
        for group_name, group_data in grouped:
            plt.plot(group_data['time'], group_data['current'], label=group_name)
        
        
        #plt.legend(loc='best')
        
        ################# SUBPLOT6#########################################
        plt.subplot(3,3,7)
        
        #plt.title("Voltage")
        
        #plt.xlabel("time")
        plt.ylabel("Voltage (mV)")
        plt.xlabel("Time (s)")
        
        plt.minorticks_on()
        
        
        
        # plt.gca().xaxis.set_major_formatter(mdates.DateFormatter("%m/%d %H:%M"))
        # plt.setp(plt.gca().get_xticklabels(), ha="right", rotation=45)
        
        #plt.plot(dfMPP["time"],dfMPP["voltage"], label='voltage')#,color = '#8000ff')
        
        
        grouped = df.groupby('type')
        for group_name, group_data in grouped:
            plt.plot(group_data['time'], group_data['voltage'], label=group_name)
        
        
        
        #plt.legend(loc='best')
        
        ##################################
        
        
        
        plt.tight_layout()
        #plt.savefig('test.png')
        #plt.show()
        
        # Show the plot
        plt.draw()
        #plt.pause(1)  # Pause for 1 second
        
        filename = "pause_seconds.txt"  # Name of the file containing the number of seconds
        # Read the number of seconds from the file
        with open(filename, "r") as file:
            seconds = float(file.read().strip())
        # Pause for the specified number of seconds
        plt.pause(seconds)
        plt.clf()
        


if __name__ == '__main__':
    fig = plt.figure(figsize=(10, 10))
    plt.rcParams["font.family"] = "Lato"
    update_graph()

