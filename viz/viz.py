import numpy as np
from pandas import read_csv
import pandas as pd
from matplotlib import pyplot
import matplotlib.ticker as ticker

column_names = ['datestamp', 'date', 'time', 'weight']
series = read_csv('../output/datr20190926.csv', names=column_names)
small_window = ('rolling4', 4)
large_window = ('rolling36', 36)

series[small_window[0]] = series['weight'].rolling(small_window[1]).median()
series[large_window[0]] = series['weight'].rolling(large_window[1]).median()

series.plot(x='time', y=[small_window[0], large_window[0]])
series.set_index('time')

count = series['time'].count()
no_of_ticks = 24
size_of_segment = int(count / no_of_ticks)

# two lists, one is a list of indices regularly spread out across the series
indices = list()
tick_labels = list()
for i in range(0, no_of_ticks):
    position = (size_of_segment * i) + 42/2
    indices.append(position)
    tick_labels.append(series['time'][position])

pyplot.grid(True, which='major')
pyplot.xticks(labels=tick_labels, ticks=indices, rotation='vertical')
pyplot.tight_layout()
pyplot.show()
