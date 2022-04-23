import numpy as np

def freq_peaks(freq,mag):
    i=0
    lenghts=mag.shape
    
    freq_length= lenghts[0]
    time_length=lenghts[1]
                   
    sums=np.empty(freq_length)
    peaks=np.zeros((time_length,freq_length), dtype='float')
    for i in range (time_length):
        for j in range (freq_length):
            if j==0:
                if mag[i,j]>mag[i,j+1]:
                    peaks[i,j]=freq[j]
            else:
                if mag[i,j] > mag[i,j-1] and mag[i,j] > sums[i,j+1]:
                    peaks[i,j]=freq[j]
            
    print(peaks)
    
frequency = np.genfromtxt('Y.csv', delimiter=',')
magnitude = np.genfromtxt('Z.csv', delimiter=',')

Y=np.zeros((len(frequency),1),float)
for i in range(len(frequency)):
    Y[i][0]=frequency[i]
    
freq_peaks(Y,magnitude)