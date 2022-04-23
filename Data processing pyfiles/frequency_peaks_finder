import numpy as np

def freq_peaks(freq,mag):
    i=0
    lenghts=mag.shape
    
    freq_length= lenghts[0]
    time_length=lenghts[1]
                   
    sums=np.empty(freq_length)
    
    while i < freq_length:
        j=0
        while j <time_length:
            sums[i]=sums[i]+mag[i,j]
            j=j+1
        i=i+1

    h=0
    peaks=np.array([])
    while h < freq_length-1:
        if h == 0:
            if sums[0]>sums[1]:
                peaks=np.append(peaks,freq[h])
                
        if h != 0:
            if sums[h] > sums[h-1] and sums[h] > sums[h+1]:
                peaks=np.append(peaks,freq[h])
        h=h+1
    print(peaks)
    
frequency = np.genfromtxt('Y.csv', delimiter=',')
magnitude = np.genfromtxt('Z.csv', delimiter=',')

Y=np.zeros((len(frequency),1),float)
for i in range(len(frequency)):
    Y[i][0]=frequency[i]
    
freq_peaks(Y,magnitude)