import pandas as pd
import numpy as np
import glob
import os
import matplotlib.pyplot as plt
import matplotlib.dates as mdates
import matplotlib.ticker as ticker
from matplotlib.dates import DateFormatter
import time
from scipy import integrate
from matplotlib.offsetbox import OffsetImage


########################################################################
path = 'dataraw/'


all_files = glob.glob(os.path.join(path, "*.csv"))
all_files=sorted(all_files) 


df = pd.concat((pd.read_csv(f, header=None, sep='\t',on_bad_lines='skip',names = ["time", "id", "notvalid9","type", "notvalid1", "integer4725", "notvalid2","volt4725","notvalid3","voltage","notvalid4","current","notvalid5","power","notvalid6","integermpp4725","notvalid7","mpppower","notvalid8","temperature"]) for f in all_files))



df["time"] =  pd.to_datetime(df["time"], errors='coerce')
df = df.drop(df[df['type'] == 'start'].index)
df = df.dropna(subset=["time"])

#rebase procedure
# Convert the timestamp index to Unix time
df["time"] = df["time"].map(pd.Timestamp.timestamp)


# Shift the timestamps to the origin of time
df['time'] = (df['time'] - df['time'].min())

df.reset_index(drop=True, inplace=True)


# Find the rows with gaps greater than 1 in the first column
gaps = df['time'].diff() > 1
gap_indices = gaps[gaps].index


# Adjust the values to maintain a difference of 1
for index in gap_indices:
    diff = df.loc[index, 'time'] - df.loc[index-1, 'time']
    num_gaps = diff - 1.0
    df.loc[index:, 'time'] -= num_gaps
df = df.set_index(df["time"])



df = df[~df.apply(lambda row: row.astype(str).str.contains('Received').any(), axis=1)]




#solar cell active area
#area = 0.64
area = 1.18
# area = 1.0
df["mpppower"] =  df["mpppower"].astype("float") / area
df["power"] =  df["power"].astype("float") / area
df["current"] =  df["current"].astype("float") / area





df = df[["time","type","integer4725","volt4725","voltage","current","power","mpppower","temperature"]].copy()



# Pair JV and SOP stage for graph legend
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


# df.to_csv('filename.csv', index=False)

# print(df)

# specific selection
#dfMPP = df[(df.type == 'MPP1') | (df.type == 'MPP') | (df.type == 'MPP0') | (df.type == 'MPP+1') | (df.type == 'MPP-1')]
#df = df[df['type'].str.startswith('MPP4') | df['type'].str.startswith('JV4')]
#dfJV = df[df['type'].str.startswith('JV')]
#df = df[(df.type == 'JV11') | (df.type == 'JV12') | (df.type == 'MPP11') | (df.type == 'MPP12')]
#df = df[(df.type == 'JV6')| (df.type == 'MPP6')]

#transfer curve voltage difference. only valid for JV with BWD-FWD or FWD-BWD curves.
prefixes = ('JV5', 'JV7')
mask = df['type'].str.startswith(prefixes)
df.loc[mask, 'differenceFB'] = df.loc[mask].groupby('integer4725')['voltage'].diff()#*(-1)
prefixes = ('JV6', 'JV8')
mask = df['type'].str.startswith(prefixes)
df.loc[mask, 'differenceBF'] = df.loc[mask].groupby('integer4725')['voltage'].diff()*(-1)



# Display the result
# print(df)
# df.to_csv('Output.csv')

# ################# PLOTS ############################################



fig = plt.figure(figsize=(10, 10))
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


# plt.gca().xaxis.set_major_formatter(mdates.DateFormatter("%m/%d %H:%M"))
# plt.setp(plt.gca().get_xticklabels(), ha="right", rotation=45)


#plt.plot(dfMPP["time"],dfMPP["mpppower"], label='mpppower')#,color = '#8000ff')
#plt.plot(dfMPP["time"],dfMPP["power"], label='power')#,color = '#8000ff')
#plt.plot(df["time"],df["mpppower"], label='mpppowerfull')#,color = '#8000ff')
#plt.plot(df["time"],df["power"], label='powerfull')#,color = '#8000ff')

grouped = df.groupby('type')
for group_name, group_data in grouped:
    plt.plot(group_data['time'], group_data['power'], label=group_name)
#plt.legend(loc='best')


########################################################################
#EN 50530 integration
########################################################################
########################################################################
# Create a dictionary with data for theoretical curve1
xa =658.4
ya = 1.25
yb = 4.53
data = {'x': [xa, xa +300, xa +600,xa +900,xa +1200],
        'y': [ya, yb, yb,ya, ya]}
# Create a DataFrame
dfEN = pd.DataFrame(data)
# Plot the data
plt.plot(dfEN['x'], dfEN['y'])
# Calculate the numerical integral of 'y' with respect to 'x'
integralc1 = np.trapz(dfEN['y'], x=dfEN['x'])
print("The total numerical integral for perfect curve:", integralc1)


# Create a dictionary with data for theoretical curve2
xa =658.4
ya = 1.25
yb = 4.53
data = {'x': [xa, xa +300, xa +600,xa +900,xa +1200],
        'y': [ya, yb, yb,ya, ya]}
# Create a DataFrame
dfEN = pd.DataFrame(data)
# Plot the data
plt.plot(dfEN['x'], dfEN['y'])
# Calculate the numerical integral of 'y' with respect to 'x'
integralc2 = np.trapz(dfEN['y'], x=dfEN['x'])
print("The total numerical integral for perfect curve:", integralc2)

# Filter dfREAL to the specified range
subset_df = df[(df['time'] >= xa) & (df['time'] <= xa+1200)]

# Calculate the numerical integral of 'y_real' with respect to 'x' for the subset
integral_subset_df = np.trapz(subset_df['power'], x=subset_df['time'])

# Print the total numerical integral
print("Total numerical integral for dfREAL between {} and {}: {}".format(xa, xa+1200, integral_subset_df))

ratioMPPc1 = integral_subset_df / integralc1* 100
print("The EfficiencyMPP is: {}".format(ratioMPPc1))

ratioMPPc2 = integral_subset_df / integralc2* 100
print("The EfficiencyMPP is: {}".format(ratioMPPc2))



################# SUBPLOT8 #########################################
ax = plt.subplot(3,3,8)

#plt.title("Temperature")
plt.ylabel("Temperature (ºC)")
plt.xlabel("Time (s)")

plt.minorticks_on()


# plt.gca().xaxis.set_major_formatter(mdates.DateFormatter("%m/%d %H:%M"))
# plt.setp(plt.gca().get_xticklabels(), ha="right", rotation=45)

#plt.plot(df["time"],df["temperature"], label='temperaturefull')#,color = '#8000ff')

grouped = df.groupby('type')
for group_name, group_data in grouped:
    plt.plot(group_data['time'], group_data['temperature'], label=group_name)
#ax.set_ylim(35, 40)
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

#plt.title("Power - V curve")

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


#plt.legend(loc='best')


################# SUBPLOT3 #########################################
ax = plt.subplot(3,3,9)

#plt.title("DAC int vs Voltage cell (mV)")

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
    #plt.plot(group_data['integer4725'], group_data['difference'], label=group_name,color = '#8000ff')
    plt.plot(group_data['volt4725'], group_data['voltage'], label=group_name)
    plt.plot(group_data['volt4725'], group_data['differenceFB'], label=group_name,color = '#8000ff')
    plt.plot(group_data['volt4725'], group_data['differenceBF'], label=group_name,color = '#80ffff')


#plt.axhline(0, color='black', linestyle='--')
#ax.set_ylim(0, 950)
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
    #plt.plot(group_data['volt4725'], group_data['difference'], label=group_name,color = '#8000ff')

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



plt.show()




