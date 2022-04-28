###Import wav file from the folder in which this code is saved and 
###computes the running FFT of the signal with a precision in Hz that can be set 
###by resolution_factor. The result is saved in 3 distinct csv files.
### the X file stores the time axis in seconds
### the Y file stores the frequency axis in Hz
### the Z file stores the amplitude of each frequency band at each time point.


import numpy as np
import scipy.io.wavfile as wavfile
import csv
from scipy import signal


fs,data = wavfile.read("LEA2_4s4.wav")    #Importing data file onto the platform
print(fs)
downsample_ratio=12
data=signal.decimate(data,downsample_ratio ,axis=0,zero_phase='True')
#data=signal.decimate(data,2 ,axis=0,zero_phase='True')
fs=fs//downsample_ratio
resolution_factor=20 #put this to 1 for 1Hz res, the bigger the factor the lower the resolution

windowLength = int(fs/resolution_factor) #1Hz resolutionif window length = fs
title = ('10s of 2nd purr pattern')

data_norm = data[:,1] * 1/(2**15)        #Normalizing data, this depends on ADC used, teensy uses 16bits


###Create Axis for plotting###
time = np.linspace(0,(len(data_norm)-windowLength)/fs,(len(data_norm)-windowLength))  #Setting up the time domain
frequency = np.linspace(0, fs/2, windowLength//2)    #Setting up frequency domain

X=np.zeros((1,len(time)),float)
Y=np.zeros((len(frequency),1),float)

for i in range(len (time)):
    X[0][i]=time[i]
    
for i in range(len(frequency)):
    Y[i][0]=frequency[i]
    
Z= np.zeros((fs//(2*resolution_factor),(len(data_norm)-windowLength)),float)

###run FFT####
for i in range(len(data_norm) - windowLength):
    #print(i)
    window = data_norm[i : (i + windowLength)]

    spect = np.fft.fft(window)/len(window) #Obtain frequency spectrum

    spect_abs = abs(spect[0:len(spect)//2]) #Absolute value of the values in the frequency domain and slice mirror freqs
    spect_dB = 20 * np.log10(spect_abs)     #Obtaining the decibel levels for each frequency
    
    for j in range(len(spect_dB)):
        Z[j][i]=spect_dB[j]


with open("X.csv", "w") as f1:
    writer = csv.writer(f1)
    writer.writerows(X)
    
with open("Y.csv", "w") as f2:
    writer = csv.writer(f2)
    writer.writerows(Y)

with open("Z.csv", "w") as f3:
    writer = csv.writer(f3)
    writer.writerows(Z)
