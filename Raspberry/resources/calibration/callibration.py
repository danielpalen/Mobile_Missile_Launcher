import re
import matplotlib.pyplot as plt
import numpy as np
from scipy import stats

fig = plt.figure(figsize=(14, 4), dpi=300)

filename = 'vehicleTurn.txt'
direction = 'left'
left = [];
right = []; right_x = []; right_y = []

def parseFile(file, direction, min = 0):
    """ parse the vehicleTurn.txt file and return two arrays with the x
        and y values for the specified direction.

        Keyword Arguments:
        file      -- the file name of the file to parse
        direction -- the direction to parse the file for
        """
    p = re.compile( '(left|right){1,1}\s+time:\s+(\d+)\s+pixel:\s+-?(\d+)')
    temp = []; result_x = []; result_y = []

    for line in open(file, 'r'):
        m = p.match(line)
        if m.group(1) == direction:
            temp.append((int(m.group(2)),int(m.group(3))))
    temp .sort(key=lambda tup: tup[0])

    for a in temp:
        #if a[0] > min:
        result_x.append(a[0])
        result_y.append(a[1])
    return result_x, result_y

def plotGraph(filename, direction):
    """
        Plots the data from the graph it parses and also perfroms
        a linear regression and parses the graph

        Keyword Arguments:
        filename  -- the file name of the file to parse
        direction -- the direction to parse the file for

        return -- a = intersect, b = slope
        """
    x, y = parseFile(filename, direction)
    b, a, _, _, _ = stats.linregress(x,y)
    plt.plot(x, y, '.', color='#004E8A')
    plt.plot(x, a + b*x, 'k', color='black')
    fn_str = 'f(x)=' + str(a) + ' + ' + str(b) + 'x'
    plt.title(direction + " turn")
    plt.xlabel('roboter turning in milliseconds')
    plt.ylabel('result in pixels turned')
    print(direction + ': ' + fn_str)
    return a, b

plt.subplot(121)
plotGraph(filename, "left")
plt.subplot(122)
plotGraph(filename, "right")
plt.show()
