w### Reads three csv files in the format generated by frequency analyser.py
### Plots the 3d graph representation of the associated recorded signal

import numpy as np
import matplotlib.pyplot as plt
import sys

reference = sys.argv[1]

### set the distance between point for averaging
spacingParam = 2

spaceAllowed = 2

### maximum number of samples to average across
averageLength = 5

time = np.genfromtxt(reference + "_X.csv", delimiter=',')
# frequency = np.genfromtxt('Y.csv', delimiter=',')
# Z = np.genfromtxt('Z.csv', delimiter=',')
peaks = np.genfromtxt(reference + "_fres_analysis.csv", delimiter = ',')

# X=np.zeros((1,len(time)),float)
# Y=np.zeros((len(frequency),1),float)

traces  = np.zeros((peaks.shape[0], peaks.shape[1]), float)
tracesForComparison  = np.zeros((peaks.shape[0], peaks.shape[1]), float)

# for i in range(len (time)):
#     X[0][i]=time[i]
    
# for i in range(len(frequency)):
#     Y[i][0]=frequency[i]
    
### i is the column (time) index, j is the row (frequency) index
# for i in range(peaks.shape[1]):
#     for j in range(peaks.shape[0]):
#         if peaks[i][j]:
#             sumRunning = sumRunning + peaks[i][j]
            
### i is the column (time) index, peakPointer is the row (frequency) index
# peakPointer = 0
# sumRunning = 0

# for i in range(peaks.shape[1]):
#     peakPointer = 0
#     sumRunning = 0
#     while (peakPointer < peaks.shape[0]):
#         if peaks[peakPointer][i]:
#             if not sumRunning:
#                 startOfAverage = peakPointer
#             sumRunning = sumRunning + peaks[peakPointer][i]
#             peakPointer = peakPointer + spacingParam
#         else:
#             if sumRunning:
#                 average = (spacingParam * sumRunning)/(peakPointer - startOfAverage) #same as divide denominator by spacingParam
#                 location = (peakPointer + startOfAverage - spacingParam)//2
#                 tracesForComparison[location][i] = average
#                 sumRunning = 0
#             peakPointer = peakPointer + 1
            
            
### i is the column (time) index, j is the row (frequency) index
for i in range(peaks.shape[1]):
    sumRunning = 0
    sampleNumber = 0
    startOfAverage = 0
    lastPeak = 0
    for j in range(peaks.shape[0]):
        if peaks[j][i] and sample:
            if not sumRunning:
                startOfAverage = j
            sumRunning = sumRunning + peaks[j][i]
            sampleNumber = sampleNumber + 1
            lastPeak = j
        elif j - lastPeak >= spaceAllowed:
            if sumRunning:
                average = sumRunning / sampleNumber 
                location = (j - spaceAllowed + startOfAverage)//2
                traces[location][i] = average
                sumRunning = 0
                sampleNumber = 0

            
tracesTrans = np.transpose(traces)

fig = plt.figure(figsize =(14, 9))
ax = plt.axes()

for i in range(traces.shape[0]):
    ax.plot(traces[i], time,  ',', color='black')

# for i in range(tracesTrans.shape[0]):
#     if tracesTrans[i] > 0:
#         ax.plot(tracesTrans[i])

ax.set_title('plot of local peaks')

ax.set_xlabel('frequencies (Hz)')
ax.set_ylabel('time (s)')

plt.show()
