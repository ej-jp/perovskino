# quick user guide
Perovskino is currently undergoing development; your patience is appreciated.

**Data logging**

1. Install Python 3.x (preferably from the Anaconda or Miniconda distribution <https://docs.conda.io/projects/miniconda/en/latest/miniconda-install.html> ) 
2. Dependencies for data logging and plotting will be pyserial (pip install pyserial) and Pandas, Matplotlib and Scipy (last three dependencies installed via conda install ...) 
3. Connect a solar cell in the Perovskino and plug it in one USB port of the computer.
4. Identify the assigned serial port for Perovskino (typically /dev/ttyUSB0 on Linux and COM3 in Windows, see Tips section below) .
5. Using the Anaconda Prompt or plain terminal in linux run: ``python capture-data-mpp-temperature.py`` in the ~/codes/02_MPP-algorithms/capture_and_grapher folder. The serial data is simultaneously displayed on the terminal window and stored in the dataraw folder as CSV files named date-time_hour.csv.
6. Utilize your preferred program or the grapherX.py scripts to plot the the CSV files stored in the dataraw folder (it is the default folder, change this folder modifying the corresponding line in the grapherX.py scripts)


**Data plotting**
Once we have obtained and stored data in the CSV files named date-time.csv we have two options to plot the obtained data using the scripts grapherX.py:

1. For graphing the previously obtained data execute: ``python grapher2.py`` 
2. If we want to plot the data while they are being obtained, open another anaconda terminal and execute ``python grapher4_alive.py`` 


**Tips:**

1. Easily determine the serial address assigned to the Arduino on the computer by installing the Arduino IDE ( <https://www.arduino.cc/en/software>) and checking the tools/port menu. Later, this IDE is used for reprogramming the micro-controller to upload modified SOP or PO .ino sketches. Once the serial port is known, modify the line number 8 in the file ~/codes/02_MPP-algorithms/capture_and_grapher/capture-datos-mpp-temperature.py accordingly if necessary.
2. Stop capturing data using control + C
3. If we have a problem graphing with grapher4_alive.py related with “Lato font” open the grapher4_alive.py in notebook and comment the corresponding line #plt.reParams[“font.family”]=”Lato”
4. If there’s any package or library not included in the Anaconda an error message will show up. Install them using pip or conda as required.
5. Data logging is obtained without device area normalization. Modify the variable square centimeters 'area' in the grapherX.py scripts for area normalization. 


