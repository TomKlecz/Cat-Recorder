import numpy as np
import csv
import sys

def freq_peaks(freq,mag):
    i=0
    lenghts=mag.shape
    
    freq_length= lenghts[0]
    time_length=lenghts[1]

    peaks=np.zeros((freq_length,time_length), dtype='float')
    for j in range (time_length-1):
        for i in range (freq_length-1):
            
            if i==0:
                if mag[i,j]>mag[i+1,j]:
                    peaks[i,j]=freq[i]
                    
            elif i==freq_length-1:
                if mag[i,j]>mag[i-1,j]:
                    peaks[i,j]=freq[i]

            else:
                if mag[i,j] > mag[i-1,j] and mag[i,j] > mag[i+1,j]:
                    
                    peaks[i,j]=freq[i]
            
    return(peaks)
    

reference = sys.argv[1]

frequency = np.genfromtxt(reference + '_Y.csv', delimiter=',')
magnitude = np.genfromtxt(reference + '_Z.csv', delimiter=',')

Y=np.zeros((len(frequency),1),float)
for i in range(len(frequency)):
    Y[i][0]=frequency[i]
    
    
freq_analysis=freq_peaks(Y,magnitude)

with open(reference + "_fres_analysis.csv", "w") as f:
    writer = csv.writer(f)
    writer.writerows(freq_analysis)
