### Reads three csv files in the format generated by frequency analyser.py
### Plots the 3d graph representation of the associated recorded signal

import numpy as np
import matplotlib.pyplot as plt


time = np.genfromtxt('X.csv', delimiter=',')
frequency = np.genfromtxt('Y.csv', delimiter=',')
Z = np.genfromtxt('Z.csv', delimiter=',')

X=np.zeros((1,len(time)),float)
Y=np.zeros((len(frequency),1),float)

for i in range(len (time)):
    X[0][i]=time[i]
    
for i in range(len(frequency)):
    Y[i][0]=frequency[i]
fig = plt.figure(figsize =(14, 9))
ax = plt.axes(projection ='3d')

my_cmap = plt.get_cmap('jet')

surf = ax.plot_surface(X, Y, Z, cmap = my_cmap, edgecolor ='none')

fig.colorbar(surf, ax = ax, shrink = 0.7, aspect = 7)

ax.set_title('Surface plot')

plt.show()