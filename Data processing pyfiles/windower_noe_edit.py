import numpy as np
import scipy.io.wavfile as wavfile
import matplotlib.pyplot as plt


fs,data = wavfile.read("2B_in_twoPurrs.wav")    #Importing data file onto the platform
print(fs)
title = ('4s of 2nd purr pattern')
resolution_factor=256 #put this to 1 for 1Hz res, the bigger the factor the lower the resolution
windowLength = fs//resolution_factor #1Hz resolutionif window length = fs

data_norm = data[:,1] * 1/(2**15)        #Normalizing data, this depends on ADC used
print(len(data_norm))


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
#Z= np.zeros(((len(data_norm)-windowLength),int(fs/256)),int)


###run FFT####
for i in range(len(data_norm) - windowLength):
    #print(i)
    window = data_norm[i : (i + windowLength)]

    spect = np.fft.fft(window)/len(window) #Obtain frequency spectrum

    spect_abs = abs(spect[0:len(spect)//2]) #Absolute value of the values in the frequency domain and slice mirror freqs
    spect_dB = 20 * np.log10(spect_abs)     #Obtaining the decibel levels for each frequency
    
    for j in range(len(spect_dB)):
        Z[j][i]=spect_dB[j]

    
#fig1 = plt.figure()
#ax1 = fig1.add_subplot(111, projection='3d')
#
#ax1.set_title(title, loc='center', wrap=True)
#
#ax1.plot_surface(X, Y, Z, cmap='jet', linewidth=1)
#
#ax1.set_xlabel('time (s)')
#ax1.set_ylabel('frequencies (Hz)')
#ax1.set_zlabel('amplitude (dB)')
#ax1.set_zlim(-100,0)
#ax1.set_ylim(5000,0)


fig = plt.figure(figsize =(14, 9))
ax = plt.axes(projection ='3d')

my_cmap = plt.get_cmap('jet')

surf = ax.plot_surface(X, Y, Z, cmap = my_cmap, edgecolor ='none')

fig.colorbar(surf, ax = ax, shrink = 0.7, aspect = 7)

ax.set_title('Surface plot')

plt.show()
