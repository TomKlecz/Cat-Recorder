# -*- coding: utf-8 -*-

import numpy as np
import scipy.io.wavfile as wavfile
import matplotlib.pyplot as plt
import pandas as pd

f1 = 50
f2 = 2000

dataOut = []

fs,data = wavfile.read("2B_in_twoPurrs.wav")    #Importing data file onto the platform
print(fs)

windowLength = fs

data_norm = data[:,1] * 1/(2**23)        #Normalizing data, this depends on ADC used
#data_norm = data[49250:50500]
print(len(data_norm))

for i in range(len(data_norm) - windowLength):
    window = data_norm[i : (i + windowLength)]

    spect = np.fft.fft(window)/len(window) #Obtain frequency spectrum

    spect_abs = abs(spect[0:len(spect)//2]) #Absolute value of the values in the frequency domain and slice mirror freqs
    spect_dB = 20 * np.log10(spect_abs)     #Obtaining the decibel levels for each frequency
    dataOut.append(spect_dB)
    #plt.clf()
    #plt.plot(dataOut)
    #plt.show()

dataOut = np.array(dataOut)
print(dataOut.shape)
plt.specgram(data_norm,Fs = fs)
#plt.imshow(dataOut)
#df1 = pd.DataFrame(dataOut)
#df1.to_excel("spectrum_of_Bwave.xlsx")

time = np.linspace(0, len(window)/fs, len(window))  #Setting up the time domain
frequency = np.linspace(0, fs/2, len(window))    #Setting up frequency domain
#
#fig, (ax1, ax2) = plt.subplots(2)
#fig.tight_layout()
#
#ax1.plot(time, data_norm)
#ax1.set_title('Data signal in time domain')
#ax1.set(xlabel='time/s', ylabel='amplitude')
#
#ax2.plot(frequency, window)
#ax2.set_title('Data signal in frequency domain')
##ax2.set_xscale('log')
#ax2.set(xlabel='frequency/Hz', ylabel='dB')
#
plt.show()
