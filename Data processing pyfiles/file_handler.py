# -*- coding: utf-8 -*-

import numpy as np
import scipy.io.wavfile as wavfile
import matplotlib.pyplot as plt

f1 = 50
f2 = 2000

fs,data = wavfile.read("twoPurrs.wav")    #Importing data file onto the platform
print(fs)

data_norm = data * 1/(2**23)        #Normalizing data, this depends on ADC used
data_norm = data[49250:50500]
print(len(data_norm))

spect = np.fft.fft(data_norm)/len(data_norm) #Obtain frequency spectrum

spect_abs = abs(spect[0:len(spect)//2]) #Absolute value of the values in the frequency domain and slice mirror freqs
spect_dB = 20 * np.log10(spect_abs)     #Obtaining the decibel levels for each frequency
length=len(spect_dB)
k1=int(length/fs*200)
k2=int(length/fs*600)


time = np.linspace(0, len(data_norm)/fs, len(data_norm))  #Setting up the time domain
frequency = np.linspace(0, fs/2, len(spect_abs))    #Setting up frequency domain

fig, (ax1, ax2) = plt.subplots(2)
fig.tight_layout()

ax1.plot(time, data_norm)
ax1.set_title('Data signal in time domain')
ax1.set(xlabel='time/s', ylabel='amplitude')

ax2.plot(frequency, spect_dB)
ax2.set_title('Data signal in frequency domain')
#ax2.set_xscale('log')
ax2.set(xlabel='frequency/Hz', ylabel='dB')

plt.show()
