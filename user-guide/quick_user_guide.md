# quick user guide
Perovskino is currently undergoing development; your patience is appreciated.


1. Install Python 3.x (preferably Anaconda or Miniconda distribution <https://docs.conda.io/projects/miniconda/en/latest/miniconda-install.html> ) along with dependencies, including Pandas, Matplotlib, and Pyserial, on your computer.
2. Connect the Perovskino to the USB port of the computer.
3. Identify the assigned address/port for Perovskino (typically /dev/ttyUSB0 on Linux) and modify the line number 8 in the file ``~/codes/02_MPP-algorithms/capture_and_grapher/capture-datos-mpp-temperature.py`` accordingly.
4. In the ``~/codes/02_MPP-algorithms/capture_and_grapher`` folder, execute ``python capture-data-mpp-temperature.py``
5. The serial data is both displayed on the terminal and stored in the dataraw folder as CSV files named date-time.csv.
6. Utilize your preferred graphing program or the grapherX.py scripts to plot the data.


Tips: 


1. Easily determine the serial address of the Arduino on the computer by installing the Arduino IDE ( <https://www.arduino.cc/en/software>)  ) and checking the tools/port menu. Later, this IDE is used for reprogramming the microcontroller to upload modified SOP or PO .ino sketches.
2. Stop capturing data using control + C.





