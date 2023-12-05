import pandas as pd
import glob
import os
import matplotlib.pyplot as plt
import matplotlib.dates as mdates
import matplotlib.ticker as ticker
from matplotlib.dates import DateFormatter
import time
from scipy import integrate
import ftplib
from matplotlib.offsetbox import OffsetImage


#######Checking MOSFETS ##################################
path = 'dataraw/'


all_files = glob.glob(os.path.join(path, "*.csv"))
all_files=sorted(all_files) 


df = pd.concat((pd.read_csv(f, header=None, sep='\t', on_bad_lines='skip',names = ["time", "type", "notvalid1", "integer4725", "notvalid2","volt4725","notvalid3","voltage","notvalid4","current","notvalid5","Rds"]) for f in all_files))



df["time"] =  pd.to_datetime(df["time"], errors='coerce')
df = df.dropna(subset=["time"])
df = df.set_index(df["time"])


#df.to_csv('Output.csv', index = False)




#df = df.drop(df[df['type'] == 'start'].index)
df = df[~df.apply(lambda row: row.astype(str).str.contains('inf').any(), axis=1)]
df = df[~df.apply(lambda row: row.astype(str).str.contains('ovf').any(), axis=1)]

df["Rds"] =  1.0 / df["Rds"].astype("float")


df = df[["time","type","integer4725","volt4725","voltage","current","Rds"]].copy()




# start_time = pd.to_datetime('2023-06-21 19:44:00')
# end_time   = pd.to_datetime('2023-06-21 20:21:00')
# df = df.loc[(df.index >= start_time) & (df.index <= end_time)]



# ################# PLOTS ############################################



fig = plt.figure(figsize=(10, 10))
plt.rcParams["font.family"] = "Lato"


####################################################################
#Create subplots in Python
####################################################################
################# SUBPLOT1 #########################################
plt.subplot(2,2,1)

plt.title("Transfer curve: ")

#plt.xlabel("time")
plt.ylabel("I$_{D}$ (mA)")
plt.xlabel("V$_{GS}$ (mV)")

plt.minorticks_on()



# plt.gca().xaxis.set_major_formatter(mdates.DateFormatter("%m/%d %H:%M"))
# plt.setp(plt.gca().get_xticklabels(), ha="right", rotation=45)

#plt.plot(df["volt4725"],df["current"], label='current')#,color = '#8000ff')
#plt.plot(dfJV["voltage"],dfJV["power"], label='power')#,color = '#8000ff')

#print(df)

grouped = df.groupby('type')
# #print(grouped)

for group_name, group_data in grouped:
    plt.plot(group_data['volt4725'], group_data['current'], label=group_name)


plt.legend(loc='best')

################# SUBPLOT2 #########################################
plt.subplot(2,2,2)

plt.title("Transconductance")

#plt.xlabel("time")
plt.ylabel("Transconductance 1/$R_{DS}$ (ohm$^{-1}$ )")
plt.xlabel("V$_{GS}$ (mV)")

plt.minorticks_on()



# plt.gca().xaxis.set_major_formatter(mdates.DateFormatter("%m/%d %H:%M"))
# plt.setp(plt.gca().get_xticklabels(), ha="right", rotation=45)

plt.yscale("log") 
#plt.plot(df["volt4725"],df["Rds"], label='current')#,color = '#8000ff')
#plt.plot(dfJV["voltage"],dfJV["power"], label='power')#,color = '#8000ff')

#print(df)

grouped = df.groupby('type')
# #print(grouped)

for group_name, group_data in grouped:
    plt.plot(group_data['volt4725'], group_data['Rds'], label=group_name)


plt.legend(loc='best')

################# SUBPLOT3 #########################################
plt.subplot(2,2,3)

plt.title("Drain plot: I$_{D}$ vs V$_{DS}$ ")

#plt.xlabel("time")
plt.ylabel("I$_{D}$  (mA)")
plt.xlabel("V$_{DS}$ (mV)")

plt.minorticks_on()



# plt.gca().xaxis.set_major_formatter(mdates.DateFormatter("%m/%d %H:%M"))
# plt.setp(plt.gca().get_xticklabels(), ha="right", rotation=45)

#plt.plot(df["voltage"],df["current"], label='current')#,color = '#8000ff')
#plt.plot(dfJV["voltage"],dfJV["power"], label='power')#,color = '#8000ff')

#print(df)

grouped = df.groupby('type')
# #print(grouped)

for group_name, group_data in grouped:
    plt.plot(group_data['voltage'], group_data['current'], label=group_name)

# for group_name, group_data in grouped:
    # plt.plot(group_data['volt4725'], group_data['voltage'], label=group_name)

plt.legend(loc='best')

################# SUBPLOT4 #########################################
plt.subplot(2,2,4)

plt.title("V$_{GS}$ vs V$_{DS}$ ")

#plt.xlabel("time")
plt.ylabel("V$_{DS}$  (mV)")
plt.xlabel("V$_{GS}$ (mV)")

plt.minorticks_on()



# plt.gca().xaxis.set_major_formatter(mdates.DateFormatter("%m/%d %H:%M"))
# plt.setp(plt.gca().get_xticklabels(), ha="right", rotation=45)

#plt.plot(df["voltage"],df["current"], label='current')#,color = '#8000ff')
#plt.plot(dfJV["voltage"],dfJV["power"], label='power')#,color = '#8000ff')

#print(df)

grouped = df.groupby('type')
# #print(grouped)

# for group_name, group_data in grouped:
    # plt.plot(group_data['voltage'], group_data['current'], label=group_name)

for group_name, group_data in grouped:
    plt.plot(group_data['volt4725'], group_data['voltage'], label=group_name)

plt.legend(loc='best')




plt.tight_layout()
#plt.savefig('test.png')



plt.show()




