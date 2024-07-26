import pandas as pd
import glob
import os
import matplotlib.pyplot as plt

#######Checking MOSFETS ##################################
path = 'dataraw/'

all_files = glob.glob(os.path.join(path, "*.csv"))
all_files=sorted(all_files) 


df = pd.concat((pd.read_csv(f, header=None, sep='\t', on_bad_lines='skip',names = ["time", "type", "notvalid1", "integer4725", "notvalid2","volt4725","notvalid3","voltage","notvalid4","current","notvalid5","Rds"]) for f in all_files))



df["time"] =  pd.to_datetime(df["time"], errors='coerce')
df = df.dropna(subset=["time"])
df = df.set_index(df["time"])


df["Rds"] =  1.0 / df["Rds"].astype("float")

df = df[["time","type","integer4725","volt4725","voltage","current","Rds"]]


# ################# PLOTS ############################################

fig = plt.figure(figsize=(10, 10))


####################################################################
#Create subplots in Python
####################################################################
################# SUBPLOT1 #########################################
plt.subplot(2,3,1)

plt.title("Transfer curve: ")

plt.ylabel("I$_{D}$ (mA)")
plt.xlabel("V$_{GS}$ (mV)")

plt.minorticks_on()

grouped = df.groupby('type')

for group_name, group_data in grouped:
    plt.plot(group_data['volt4725'], group_data['current'], label=group_name)

plt.legend(loc='best')

################# SUBPLOT2 #########################################
plt.subplot(2,3,2)

plt.title("Transconductance")

#plt.xlabel("time")
plt.ylabel("Transconductance 1/$R_{DS}$ (ohm$^{-1}$ )")
plt.xlabel("V$_{GS}$ (mV)")

plt.minorticks_on()

plt.yscale("log") 

grouped = df.groupby('type')

for group_name, group_data in grouped:
    plt.plot(group_data['volt4725'], group_data['Rds'], label=group_name)

plt.legend(loc='best')

################# SUBPLOT3 #########################################
plt.subplot(2,3,3)

plt.title("Drain plot: I$_{D}$ vs V$_{DS}$ ")

plt.ylabel("I$_{D}$  (mA)")
plt.xlabel("V$_{DS}$ (mV)")

plt.minorticks_on()

grouped = df.groupby('type')

for group_name, group_data in grouped:
    plt.plot(group_data['voltage'], group_data['current'], label=group_name)

plt.legend(loc='best')

################# SUBPLOT4 #########################################
plt.subplot(2,3,4)

plt.title("I$_{D}$ vs DAC$_{GS}$")

plt.ylabel("I$_{D}$ (mA)")
plt.xlabel("DAC Integer$_{GS}$ (-)")

plt.minorticks_on()

grouped = df.groupby('type')

for group_name, group_data in grouped:
    plt.plot(group_data['integer4725'], group_data['current'], label=group_name)


plt.legend(loc='best')

################# SUBPLOT5 #########################################
plt.subplot(2,3,5)

plt.title("Power$_{DS}$ vs DAC$_{GS}$")

#plt.xlabel("time")
plt.ylabel("Power$_{D}$ (mW)")
plt.xlabel("DAC Integer$_{GS}$ (-)")

plt.minorticks_on()



# plt.gca().xaxis.set_major_formatter(mdates.DateFormatter("%m/%d %H:%M"))
# plt.setp(plt.gca().get_xticklabels(), ha="right", rotation=45)

#plt.plot(df["voltage"],df["current"], label='current')#,color = '#8000ff')
#plt.plot(dfJV["voltage"],dfJV["power"], label='power')#,color = '#8000ff')

#print(df)

grouped = df.groupby('type')
# #print(grouped)

for group_name, group_data in grouped:
    plt.plot(group_data['integer4725'], group_data['current']*group_data['voltage']/1000, label=group_name)


plt.legend(loc='best')

################# SUBPLOT6 #########################################
plt.subplot(2,3,6)

plt.title("Power$_{DS}$ vs V$_{DS}$")

plt.ylabel("Power$_{D}$ (mW)")
plt.xlabel("V$_{DS}$ (mV)")

plt.minorticks_on()


grouped = df.groupby('type')

for group_name, group_data in grouped:
    plt.plot(group_data['voltage'], group_data['current']*group_data['voltage']/1000, label=group_name)


plt.legend(loc='best')
########################################################################
plt.tight_layout()
#plt.savefig('test.png')


plt.show()




