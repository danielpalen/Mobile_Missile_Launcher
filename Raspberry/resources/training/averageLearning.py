import matplotlib.pyplot as plt
import numpy as np
from matplotlib.legend_handler import HandlerLine2D

'''
    This little script just plots the results from the experiment.
'''

fig = plt.figure(figsize=(14, 5), dpi=100)
axes = plt.gca()
axes.set_ylim([-80,40])

x = np.array([1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20])
measurementsTwo = []
measurementsOne = []

with open("totalReward_epsilon02.txt") as f:
    content = f.readlines()
    for line in content:
        line = line[1:len(line)-2]
        #print(line)
        measurementsTwo.append(np.fromstring(line, dtype=int, sep=','))

with open("totalReward_epsilon10.txt") as f:
    content = f.readlines()
    for line in content:
        line = line[1:len(line)-2]
        #print(line)
        measurementsOne.append(np.fromstring(line, dtype=int, sep=','))


#print(measurements)
#measurements = [a,b,c,d]

def avg(measurements):
    result = [0] * len(measurements[0])
    for m in measurements:
        result += m
        #plt.plot(m)
    return result/len(measurements)

avgTwo = avg(measurementsTwo)
avgOne = avg(measurementsOne)

def stdDev(measurements, avg):
    result = [0] * len(measurements[0])
    for m in measurements:
        #print("start")
        #print(m - avg)
        result += (m - avg)**2
        #print(result)
        #print(np.sqrt(result))
        #print("end")
    return np.sqrt(result/len(measurements))

stdTwo = stdDev(measurementsTwo, avgTwo)
stdOne = stdDev(measurementsOne, avgOne)

#min = np.percentile(measurements, 25, axis=0)
#med = np.percentile(measurements, 50, axis=0)
#max = np.percentile(measurements, 75, axis=0)

#errmin = med-min
#errmax = max -med
plt.axhline(-40, color='lightgrey')
plt.axhline(-50, color='lightgrey')
plt.axhline(-60, color='lightgrey')
plt.axhline(-70, color='lightgrey')
plt.axhline(-30, color='lightgrey')
plt.axhline(-20, color='lightgrey')
plt.axhline(-10, color='lightgrey')
plt.axhline(0, color='lightgrey')
plt.axhline(10, color='lightgrey')
plt.axhline(20, color='lightgrey')
plt.axhline(30, color='lightgrey')


two = plt.errorbar(x, avgTwo, stdTwo,  linestyle='solid', marker='.', color='#004E8A', label='epsilon = 0.02', linewidth=2)
#one = plt.errorbar(x, avgOne, avgOne, linestyle='solid', marker='.', color='#B90F22',  label='epsilon = 0.1', linewidth=2)

#plt.errorbar(x, min,  linestyle='solid', marker='^')
#plt.errorbar(x, med, linestyle='solid', marker='^')
#plt.errorbar(x, med, yerr=[errmin,errmax], linestyle='solid', marker='^')
#plt.errorbar(x, max, linestyle='solid', marker='^')

def drawAll(measurements):
    for m in measurements:
        plt.errorbar(x, m, linestyle='dashed')

plt.xlabel('total reward per episode')
plt.ylabel('episodes')

#drawAll(measurementsTwo)


plt.show()
