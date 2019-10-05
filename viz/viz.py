import numpy as np
from pandas import read_csv
import pandas as pd
from matplotlib import pyplot
import matplotlib.ticker as ticker

column_names = ['datestamp', 'date', 'time', 'weight']
df = read_csv('../output/datr20190926.csv', names=column_names)
small_window = {'name': 'rolling4', 'size': 4}
large_window = {'name': 'rolling36', 'size': 36}
events = 'events'

df[small_window['name']] = df['weight'].rolling(small_window['size']).median()
df[large_window['name']] = df['weight'].rolling(large_window['size']).median()

# produce a series, based on the results of the large_window median filter,
# that looks for large changes that might be interesting.

df['pct_change'] = df[large_window['name']].pct_change()
df.plot(x='time', y=[large_window['name'], 'pct_change'], secondary_y=['pct_change'])

df.set_index('time')


#  pretty printing
count = df['time'].count()
no_of_ticks = 24
size_of_segment = int(count / no_of_ticks)

# two lists, one is a list of indices regularly spread out across the df
indices = list()
tick_labels = list()
for i in range(0, no_of_ticks):
    position = (size_of_segment * i) + 42/2
    indices.append(position)
    tick_labels.append(df['time'][position])

pyplot.grid(True, which='major')
pyplot.xticks(labels=tick_labels, ticks=indices, rotation='vertical')

pyplot.tight_layout()
pyplot.show()
