#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Thu Mar  10 03:12:30 2022

@author: tomek
"""


import numpy as np
import matplotlib.pyplot as plt
import scipy.io.wavfile as wavfile
import fir_filter as ff
import fir1
import scipy.signal as signal


result=[]
Fs, pattern = wavfile.read("pattern.wav")               #load data
pattern_norm = pattern[:,1] * 1/(2**23)                 #normalise data
fs, data = wavfile.read("shortPurr.wav")           #load data
data_norm = data[:,1] * 1/(2**23)                       #normalise data


print(data.size)

sos_pattern = signal.butter(6, [5, 200], btype='bandpass', output='sos', fs = Fs)
sos_data = signal.butter(6, [5, 150], btype='bandpass', output='sos', fs = fs)

pattern_norm_no_drift = signal.sosfilt(sos_pattern, pattern_norm)
pattern_norm_no_drift = pattern_norm_no_drift/(max(pattern_norm_no_drift))
data_norm_no_drift = signal.sosfilt(sos_data, data_norm)
data_norm_no_drift = data_norm_no_drift/(max(data_norm_no_drift))



h_match = pattern_norm_no_drift[::-1]


#x_match = ff.FIR_filter(h_match)

f = fir1.Fir1(h_match)

counter = 0
detection = []
lastpeak = 0
totaltime = 0

trace = []
timescale = []
counter = 0

for i in range(len(data_norm_no_drift)):
    sample=data_norm_no_drift[i]
    detect = f.filter(sample)
    detect = detect * detect * detect * detect
    detection.append(detect)

    if detect > 0.2e8:
        if (i - lastpeak) > 500:
            counter = counter + 1
            if counter > 0:
                time = (i - lastpeak)/fs
                totaltime = i/fs
                timescale.append(totaltime)
                freq = 1/time
                #print(freq)
                trace.append(freq)
            lastpeak = i
            
            
plt.figure(1)
plt.plot(trace)
print(trace)
#plt.plot(timescale, detection[:10000])
#plt.plot(trace)
#print(detection)
#plt.title('Heart rate trace')
#plt.xlabel('time/s')
#plt.ylabel('heart rate/bpm')
#plt.figure(2)
#plt.plot(pattern_norm_no_drift)
plt.show()
